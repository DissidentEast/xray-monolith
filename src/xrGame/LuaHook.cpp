#include "pch_script.h"
#include "LuaHook.h"
#include "ai_space.h"
#include "../xrServerEntities/script_engine.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <cstring>

#pragma comment(lib, "Ws2_32.lib")

// LuaHook: loopback HTTP hook for automated Lua testing (MCP lua_exec).
// Off unless "-luahook[=port]" is present. Listener thread only queues;
// Pump() executes on the main thread. See LuaHook.h.

namespace {
// ---------------------------------------------------------------- state
struct Request
{
	xr_string code;
	xr_string file;
	u32 timeout_ms;
	xr_string result;
	xr_string error;
	bool ok;
	bool done;
	bool abandoned;
	HANDLE event;

	Request() : timeout_ms(5000), ok(false), done(false), abandoned(false),
	            event(nullptr)
	{
		event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}
};

enum
{
	QueueCap = 16,
	MaxBody = 1024 * 1024,
	MaxCode = 256 * 1024,
	MaxResults = 16,
	MaxResultChars = 4096,
	MaxStringChars = 512,
	SelectSliceMs = 200
};

CRITICAL_SECTION g_cs;
bool g_cs_live = false;
bool g_started = false;
bool g_dead = false;
bool g_stop = false;
SOCKET g_listen = INVALID_SOCKET;
u16 g_port = LuaHook::DefaultPort;
volatile bool g_ready_cache = false;
Request* g_queue[QueueCap] = {};
int g_qcount = 0;
u64 g_hook_deadline = 0; // GetTickCount64 value, main thread only

void Lock() { EnterCriticalSection(&g_cs); }
void Unlock() { LeaveCriticalSection(&g_cs); }

lua_State* GameLua()
{
	if (!g_ai_space)
		return nullptr;
	return g_ai_space->script_engine().lua();
}

// ---------------------------------------------------------------- timeout
void __cdecl TimeoutHook(lua_State* L, lua_Debug*)
{
	if (GetTickCount64() > g_hook_deadline)
		luaL_error(L, "luahook: instruction timeout");
}

// ---------------------------------------------------------------- repr
void AppendEscaped(xr_string& out, const char* s, size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		char c = s[i];
		switch (c)
		{
		case '"': out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default:
			if (c >= 0 && c < 0x20)
			{
				char buf[8];
				xr_sprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c);
				out += buf;
			}
			else
				out += c;
			break;
		}
	}
}

void ReprValue(lua_State* L, int idx, xr_string& out)
{
	if (out.size() >= MaxResultChars)
		return;
	int t = lua_type(L, idx);
	switch (t)
	{
	case LUA_TNIL: out += "nil"; break;
	case LUA_TBOOLEAN: out += lua_toboolean(L, idx) ? "true" : "false"; break;
	case LUA_TNUMBER:
	{
		char buf[64];
		xr_sprintf(buf, sizeof(buf), "%.14g", lua_tonumber(L, idx));
		out += buf;
		break;
	}
	case LUA_TSTRING:
	{
		size_t n = 0;
		const char* s = lua_tolstring(L, idx, &n);
		if (n > MaxStringChars)
			n = MaxStringChars;
		out += "\"";
		AppendEscaped(out, s ? s : "", n);
		out += "\"";
		if (lua_objlen(L, idx) > MaxStringChars)
			out += "...";
		break;
	}
	case LUA_TTABLE:
	{
		char buf[64];
		xr_sprintf(buf, sizeof(buf), "table: %p #%u", lua_topointer(L, idx),
		           (unsigned)lua_objlen(L, idx));
		out += buf;
		break;
	}
	default:
	{
		char buf[64];
		xr_sprintf(buf, sizeof(buf), "%s: %p", lua_typename(L, t),
		           lua_topointer(L, idx));
		out += buf;
		break;
	}
	}
	if (out.size() > MaxResultChars)
		out.resize(MaxResultChars);
}

// ---------------------------------------------------------------- exec core
bool ExecCore(lua_State* L, const char* code, size_t code_len,
              const char* chunk, u32 timeout_ms, xr_string& result,
              xr_string& error)
{
	lua_Hook old_h = lua_gethook(L);
	int old_m = lua_gethookmask(L);
	int old_c = lua_gethookcount(L);

	int st = luaL_loadbuffer(L, code, code_len, chunk);
	if (st != 0)
	{
		const char* msg = lua_tostring(L, -1);
		error.assign(msg ? msg : "load error");
		if (error.size() > 2048)
			error.resize(2048);
		lua_pop(L, 1);
		return false;
	}
	if (timeout_ms > 0)
	{
		g_hook_deadline = GetTickCount64() + (u64)timeout_ms;
		lua_sethook(L, TimeoutHook, LUA_MASKCOUNT, 1000);
	}
	int base = lua_gettop(L);
	st = lua_pcall(L, 0, LUA_MULTRET, 0);
	lua_sethook(L, old_h, old_m, old_c);
	if (st != 0)
	{
		const char* msg = lua_tostring(L, -1);
		error.assign(msg ? msg : "runtime error");
		if (error.size() > 2048)
			error.resize(2048);
		lua_pop(L, 1);
		return false;
	}
	int top = lua_gettop(L);
	int n = top - base + 1;
	if (n > MaxResults)
		n = MaxResults;
	result.clear();
	for (int i = 0; i < n; ++i)
	{
		if (i > 0)
			result += ", ";
		ReprValue(L, base + i, result);
	}
	lua_settop(L, base - 1);
	return true;
}

bool ReadFileCapped(const char* path, xr_string& out, xr_string& error)
{
	FILE* f = nullptr;
	if (fopen_s(&f, path, "rb") != 0 || !f)
	{
		error = "cannot open file";
		return false;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (size < 0 || (size_t)size > MaxCode)
	{
		fclose(f);
		error = "file too large (256KB cap)";
		return false;
	}
	out.resize((size_t)size);
	if (size > 0 && fread(&out[0], 1, (size_t)size, f) != (size_t)size)
	{
		fclose(f);
		error = "file read failed";
		return false;
	}
	fclose(f);
	return true;
}

bool ValidScriptPath(const char* path)
{
	if (!path || !path[0])
		return false;
	// Absolute Windows path only (drive or UNC).
	bool abs = (path[0] != 0 && path[1] == ':') ||
	           (path[0] == '\\' && path[1] == '\\');
	if (!abs)
		return false;
	const char* ext = strrchr(path, '.');
	if (!ext)
		return false;
	return xr_strcmp(ext, ".script") == 0 || xr_strcmp(ext, ".lua") == 0 ||
	       xr_strcmp(ext, ".txt") == 0;
}

bool Execute(Request* r)
{
	lua_State* L = GameLua();
	if (!L)
	{
		r->error = "script engine not ready (no level / engine starting?)";
		return false;
	}
	if (!r->file.empty())
	{
		if (!ValidScriptPath(r->file.c_str()))
		{
			r->error = "file must be an absolute path ending .script/.lua/.txt";
			return false;
		}
		xr_string body, err;
		if (!ReadFileCapped(r->file.c_str(), body, err))
		{
			r->error = err;
			return false;
		}
		xr_string chunk = "@";
		chunk += r->file;
		return ExecCore(L, body.c_str(), body.size(), chunk.c_str(),
		                r->timeout_ms, r->result, r->error);
	}
	return ExecCore(L, r->code.c_str(), r->code.size(), "@luahook",
	                r->timeout_ms, r->result, r->error);
}

// ---------------------------------------------------------------- http/json
bool SendAll(SOCKET s, const char* buf, int len)
{
	int sent = 0;
	while (sent < len)
	{
		int n = send(s, buf + sent, len - sent, 0);
		if (n <= 0)
			return false;
		sent += n;
	}
	return true;
}

void SendJson(SOCKET s, const xr_string& body, int code = 200)
{
	const char* reason = code == 200 ? "OK" : (code == 404 ? "Not Found" : "Error");
	char head[256];
	xr_sprintf(head, sizeof(head),
	           "HTTP/1.0 %d %s\r\nContent-Type: application/json\r\n"
	           "Content-Length: %u\r\nConnection: close\r\n\r\n",
	           code, reason, (unsigned)body.size());
	SendAll(s, head, (int)xr_strlen(head));
	SendAll(s, body.c_str(), (int)body.size());
}

bool ReadHttp(SOCKET s, xr_string& method, xr_string& path, xr_string& body)
{
	char buf[8192];
	xr_string head;
	for (;;)
	{
		int n = recv(s, buf, sizeof(buf), 0);
		if (n <= 0)
			return false;
		head.append(buf, (size_t)n);
		if (head.size() > 65536)
			return false;
		size_t eoh = head.find("\r\n\r\n");
		if (eoh != xr_string::npos)
		{
			xr_string headers = head.substr(0, eoh);
			body = head.substr(eoh + 4);
			size_t sp1 = headers.find(' ');
			size_t sp2 = headers.find(' ', sp1 + 1);
			if (sp1 == xr_string::npos || sp2 == xr_string::npos)
				return false;
			method = headers.substr(0, sp1);
			path = headers.substr(sp1 + 1, sp2 - sp1 - 1);
			size_t cl = headers.find("Content-Length:");
			size_t want = 0;
			if (cl != xr_string::npos)
				want = (size_t)strtoul(headers.c_str() + cl + 15, nullptr, 10);
			if (want > MaxBody)
				return false;
			while (body.size() < want)
			{
				n = recv(s, buf, sizeof(buf), 0);
				if (n <= 0)
					return false;
				body.append(buf, (size_t)n);
				if (body.size() > MaxBody)
					return false;
			}
			if (body.size() > want)
				body.resize(want);
			return true;
		}
	}
}

// Parse a JSON "key": "string" value (escapes incl. \uXXXX). False if absent.
bool JsonString(const xr_string& json, const char* key, xr_string& out)
{
	xr_string pat = "\"";
	pat += key;
	pat += "\"";
	size_t k = json.find(pat.c_str());
	if (k == xr_string::npos)
		return false;
	size_t p = json.find(':', k + pat.size());
	if (p == xr_string::npos)
		return false;
	++p;
	while (p < json.size() && (json[p] == ' ' || json[p] == '\t'))
		++p;
	if (p >= json.size() || json[p] != '"')
		return false;
	++p;
	out.clear();
	while (p < json.size() && json[p] != '"')
	{
		char c = json[p];
		if (c == '\\' && p + 1 < json.size())
		{
			char e = json[++p];
			switch (e)
			{
			case 'n': out += '\n'; break;
			case 'r': out += '\r'; break;
			case 't': out += '\t'; break;
			case 'u':
			{
				unsigned cp = 0;
				for (int i = 0; i < 4 && p + 1 < json.size(); ++i)
				{
					char h = json[++p];
					cp <<= 4;
					if (h >= '0' && h <= '9')
						cp += (unsigned)(h - '0');
					else if (h >= 'a' && h <= 'f')
						cp += (unsigned)(h - 'a' + 10);
					else if (h >= 'A' && h <= 'F')
						cp += (unsigned)(h - 'A' + 10);
				}
				if (cp < 0x80)
					out += (char)cp;
				else if (cp < 0x800)
				{
					out += (char)(0xC0 | (cp >> 6));
					out += (char)(0x80 | (cp & 0x3F));
				}
				else
				{
					out += (char)(0xE0 | (cp >> 12));
					out += (char)(0x80 | ((cp >> 6) & 0x3F));
					out += (char)(0x80 | (cp & 0x3F));
				}
				break;
			}
			default: out += e; break;
			}
			++p;
		}
		else
		{
			out += c;
			++p;
		}
	}
	return true;
}

bool JsonUint(const xr_string& json, const char* key, u32& out)
{
	xr_string pat = "\"";
	pat += key;
	pat += "\"";
	size_t k = json.find(pat.c_str());
	if (k == xr_string::npos)
		return false;
	size_t p = json.find(':', k + pat.size());
	if (p == xr_string::npos)
		return false;
	++p;
	while (p < json.size() && (json[p] == ' ' || json[p] == '\t'))
		++p;
	if (p >= json.size() || json[p] < '0' || json[p] > '9')
		return false;
	out = (u32)strtoul(json.c_str() + p, nullptr, 10);
	return true;
}

xr_string StatusBody()
{
	char buf[512];
	xr_sprintf(buf, sizeof(buf),
	           "{\"ok\":true,\"ready\":%s,\"queue\":%d,\"port\":%u,"
	           "\"lua\":\"%s\",\"game\":\"Anomaly\"}",
	           g_ready_cache ? "true" : "false", g_qcount, (unsigned)g_port,
	           LUA_RELEASE);
	return xr_string(buf);
}

void HandleConn(SOCKET c)
{
	xr_string method, path, body;
	if (!ReadHttp(c, method, path, body))
	{
		closesocket(c);
		return;
	}
	if (method == "GET" && path == "/status")
	{
		SendJson(c, StatusBody());
		closesocket(c);
		return;
	}
	if (!(method == "POST" && path == "/exec"))
	{
		SendJson(c, "{\"ok\":false,\"error\":\"use POST /exec or GET /status\"}", 404);
		closesocket(c);
		return;
	}
	Request* r = new Request();
	u32 t = 0;
	if (JsonUint(body, "timeout_ms", t))
		r->timeout_ms = t;
	if (r->timeout_ms == 0)
		r->timeout_ms = 5000;
	JsonString(body, "file", r->file);
	if (!JsonString(body, "code", r->code))
	{
		// Plain-text body convenience (curl without JSON).
		if (!body.empty() && body[0] != '{')
			r->code = body;
	}
	if (r->code.empty() && r->file.empty())
	{
		delete r;
		SendJson(c, "{\"ok\":false,\"error\":\"pass code or file\"}", 400);
		closesocket(c);
		return;
	}
	if (r->code.size() > MaxCode)
	{
		delete r;
		SendJson(c, "{\"ok\":false,\"error\":\"code too large (256KB cap)\"}", 400);
		closesocket(c);
		return;
	}
	Lock();
	if (g_qcount >= QueueCap)
	{
		Unlock();
		delete r;
		SendJson(c, "{\"ok\":false,\"error\":\"queue full\"}", 503);
		closesocket(c);
		return;
	}
	g_queue[g_qcount++] = r;
	Unlock();

	u32 wait_ms = r->timeout_ms + 15000;
	if (wait_ms < 1000)
		wait_ms = 1000;
	if (wait_ms > 120000)
		wait_ms = 120000;
	DWORD wr = WaitForSingleObject(r->event, wait_ms);
	Lock();
	bool done = r->done;
	if (!done)
		r->abandoned = true; // Pump() will delete it.
	Unlock();
	xr_string resp;
	if (wr == WAIT_OBJECT_0 && done)
	{
		resp = "{\"ok\":";
		resp += r->ok ? "true,\"result\":\"" : "false,\"error\":\"";
		AppendEscaped(resp, r->ok ? r->result.c_str() : r->error.c_str(),
		              r->ok ? r->result.size() : r->error.size());
		resp += "\"}";
		CloseHandle(r->event);
		delete r;
	}
	else
	{
		resp = "{\"ok\":false,\"error\":\"timeout waiting for game thread "
		       "(paused, loading, or hook idle?)\"}";
		// Request frees itself in Pump(); event stays valid until then.
	}
	SendJson(c, resp);
	closesocket(c);
}

void ListenerThread(void*)
{
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
	{
		Msg("! luahook: WSAStartup failed");
		return;
	}
	SOCKET ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ls == INVALID_SOCKET)
	{
		Msg("! luahook: socket failed");
		WSACleanup();
		return;
	}
	sockaddr_in a;
	memset(&a, 0, sizeof(a));
	a.sin_family = AF_INET;
	a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	a.sin_port = htons(g_port);
	int one = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof(one));
	if (bind(ls, (sockaddr*)&a, sizeof(a)) != 0 || listen(ls, 4) != 0)
	{
		Msg("! luahook: bind/listen failed on 127.0.0.1:%u", (unsigned)g_port);
		closesocket(ls);
		WSACleanup();
		return;
	}
	Lock();
	g_listen = ls;
	Unlock();
	Msg("* luahook: listening on 127.0.0.1:%u (POST /exec, GET /status)",
	    (unsigned)g_port);
	for (;;)
	{
		if (g_stop)
			break;
		fd_set rf;
		FD_ZERO(&rf);
		FD_SET(ls, &rf);
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = (long)(SelectSliceMs * 1000);
		int n = select(0, &rf, nullptr, nullptr, &tv);
		if (n < 0)
			break;
		if (n == 0)
			continue;
		SOCKET c = accept(ls, nullptr, nullptr);
		if (c == INVALID_SOCKET)
		{
			if (g_stop)
				break;
			continue;
		}
		HandleConn(c);
	}
	Lock();
	if (g_listen != INVALID_SOCKET)
	{
		closesocket(g_listen);
		g_listen = INVALID_SOCKET;
	}
	Unlock();
	// No WSACleanup: process-lifetime debug listener; OS reclaims on exit.
}

bool WantHook(u16& port)
{
	LPCSTR p = strstr(Core.Params, "-luahook");
	if (!p)
		return false;
	port = LuaHook::DefaultPort;
	unsigned v = 0;
	if (sscanf(p, "-luahook=%u", &v) == 1 || sscanf(p, "-luahook %u", &v) == 1)
	{
		if (v > 0 && v < 65536)
			port = (u16)v;
	}
	return true;
}

void Start()
{
	if (!g_cs_live)
	{
		InitializeCriticalSection(&g_cs);
		g_cs_live = true;
	}
	g_port = LuaHook::DefaultPort;
	u16 port = g_port;
	WantHook(port);
	g_port = port;
	thread_spawn(ListenerThread, "LuaHook", 0, nullptr);
	g_started = true;
}
} // namespace

namespace LuaHook {
void Pump()
{
	if (g_dead)
		return;
	if (!g_started)
	{
		u16 dummy = 0;
		if (!WantHook(dummy))
			return;
		Start();
		if (!g_started)
			return;
	}
	g_ready_cache = (GameLua() != nullptr);

	Request* batch[QueueCap];
	int n = 0;
	Lock();
	while (g_qcount > 0 && n < QueueCap)
	{
		for (int i = 0; i + 1 < g_qcount; ++i)
			g_queue[i] = g_queue[i + 1];
		--g_qcount;
		batch[n++] = g_queue[g_qcount];
		g_queue[g_qcount] = nullptr;
	}
	Unlock();
	for (int i = 0; i < n; ++i)
	{
		Request* r = batch[i];
		Lock();
		bool ab = r->abandoned;
		Unlock();
		if (ab)
		{
			CloseHandle(r->event);
			delete r;
			continue;
		}
		r->ok = Execute(r);
		Lock();
		if (r->abandoned)
		{
			Unlock();
			CloseHandle(r->event);
			delete r;
		}
		else
		{
			r->done = true;
			SetEvent(r->event);
			Unlock();
		}
	}
}

void Shutdown()
{
	if (!g_started || g_dead)
		return;
	g_dead = true;
	g_stop = true;
	Lock();
	if (g_listen != INVALID_SOCKET)
	{
		closesocket(g_listen);
		g_listen = INVALID_SOCKET;
	}
	for (int i = 0; i < g_qcount; ++i)
	{
		if (g_queue[i])
		{
			CloseHandle(g_queue[i]->event);
			delete g_queue[i];
			g_queue[i] = nullptr;
		}
	}
	g_qcount = 0;
	Unlock();
}

bool Ready()
{
	return GameLua() != nullptr;
}

bool ExecDirect(const char* code, unsigned timeout_ms, xr_string& result,
                xr_string& error)
{
	lua_State* L = GameLua();
	if (!L)
	{
		error = "script engine not ready";
		return false;
	}
	if (!code || !code[0])
	{
		error = "empty code";
		return false;
	}
	return ExecCore(L, code, xr_strlen(code), "@luahook_console", timeout_ms,
	                result, error);
}
} // namespace LuaHook
