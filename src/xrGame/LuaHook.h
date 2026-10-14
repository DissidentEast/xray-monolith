#pragma once

// LuaHook: loopback HTTP hook for executing Lua snippets/files on the game's
// own lua_State from automation (MCP lua_exec) or the in-game console.
//
//   - Off unless "-luahook[=port]" is on the command line (default 18751).
//   - POST /exec {"code":"...","timeout_ms":5000} -> {"ok":..,"result"/"error"}
//   - POST /exec {"file":"C:\\...\\x.script","timeout_ms":15000}
//   - GET  /status -> {"ok":true,"ready":..,"queue":n,...}
//
// Threading: the listener thread only parses HTTP and queues requests. All
// Lua execution happens in Pump(), which must be called every frame on the
// main thread (Lua states are not thread-safe). Callers block on a
// per-request event until the next Pump() runs the request.

struct lua_State;

namespace LuaHook
{
	// Default TCP port (loopback only) when -luahook carries no port.
	enum { DefaultPort = 18751 };

	// Pump queued requests. Call once per frame on the main thread.
	// Also lazily starts the listener when -luahook is present.
	void Pump();

	// Idempotent shutdown (safe to call when never started).
	void Shutdown();

	// Ready when the script engine owns a live lua_State.
	bool Ready();

	// Execute code right now on the calling thread. The caller MUST be the
	// main thread. Returns true on success (result filled) or false (error).
	bool ExecDirect(const char* code, unsigned timeout_ms, xr_string& result, xr_string& error);
} // namespace LuaHook
