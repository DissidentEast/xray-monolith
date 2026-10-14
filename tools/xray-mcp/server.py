#!/usr/bin/env python3
"""xray-monolith MCP server (stdlib only, stdio JSON-RPC).

Exposes the repetitive build/test/log chores for this repo as MCP tools so
agents (and you, via the agent) stop flying blind:
  - list_configs   : solution configurations parsed from engine-vs2022.sln
  - build_solution : full MSBuild run, log to Temp\\opencode, error summary
  - build_errors   : parse an MSBuild log for errors/warnings
  - list_exes      : game bin .exe inventory (name/size/mtime)
  - game_log       : tail + verdict of xray_gamer.log (boot? script errors?)
  - run_smoke      : launch a game exe (sidecar-safe), wait, report log verdict
  - git_info       : branch / status / recent log (read-only)
  - lua_status     : in-game LuaHook hook status (game must run with -luahook)
  - lua_exec       : run Lua code/file on the game's lua_State via the hook
  - unpacked_list  : inventory of tools/_unpacked (incl. binary assets)
  - unpacked_read  : read a text file under tools/_unpacked
  - unpacked_grep  : regex search across unpacked text files
  - script_check   : LuaJIT-2.1 syntax check of game scripts (parse-only)
  - script_symbols : Lua function defs + callers across game scripts
  - ltx_get        : .ltx values with #include + inheritance resolved

Read-only by design (no tool writes to the game tree).

No third-party packages. Guardrails: all file access is confined to the
repo, the game dir, and the temp log dir; exes can only be launched from
the game bin dir; stage-overs always back up first.
"""
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import traceback
from datetime import datetime
from pathlib import Path

SERVER_NAME = "xray-monolith"
SERVER_VERSION = "0.2.0"
PROTOCOL_VERSION = "2024-11-05"

REPO = Path(__file__).resolve().parents[2]
SRC = REPO / "src"
SLN = SRC / "engine-vs2022.sln"
GAME = Path(os.environ.get("XRAY_GAME_DIR", r"C:\anomaly 1.5.3"))
BINDIR = GAME / "bin"
LOGDIR = GAME / "appdata" / "logs"
GAMELOG = LOGDIR / "xray_gamer.log"
UNPACKED = Path(os.environ.get("XRAY_UNPACKED_DIR",
                               str(GAME / "tools" / "_unpacked")))
LUACHECK = Path(os.environ.get("XRAY_LUACHECK",
                               str(Path(os.environ.get("TEMP") or r"C:\Temp")
                                   / "opencode" / "luacheck.exe")))
TMPLOG = Path(
    os.environ.get("XRAY_LOG_DIR")
    or str(Path(os.environ.get("TEMP") or r"C:\Temp") / "opencode")
)
MSBUILD_FIXED = (
    r"C:\Program Files\Microsoft Visual Studio\2022\Community"
    r"\MSBuild\Current\Bin\MSBuild.exe"
)

# CDB (Windows Kits debugger) candidates, used by the run_cdb tool to catch
# startup crashes with a stack instead of a bare exit code.
CDB_CANDIDATES = (
    r"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe",
    r"C:\Program Files\Windows Kits\10\Debuggers\x64\cdb.exe",
    r"C:\Program Files (x86)\Windows Kits\10\Debuggers\x86\cdb.exe",
    r"C:\Program Files (x86)\Windows Kits\10\Debuggers\arm64\cdb.exe",
)
# Where the .pdb next to the freshly built exe lives (for symbolization).
PDBDIR = Path(os.environ.get(
    "XRAY_PDB_DIR", str(REPO / "_build" / "_game" / "bin_dbg")))

ERR_RE = re.compile(r": error |error C\d+|error LNK\d+|error MSB\d+|fatal error", re.I)
WARN_RE = re.compile(r": warning |warning C\d+|warning LNK\d+|warning MSB\d+", re.I)

# Text formats inside tools/_unpacked worth reading/searching. Anything
# else (ogg, dds, ogf, ...) is inventory-only via unpacked_list.
TEXT_EXTS = frozenset({
    ".script", ".lua", ".ltx", ".txt", ".xml", ".cfg", ".ini", ".md",
    ".hlsl", ".glsl", ".ps", ".vs", ".shader", ".thm",
})


# ---------------------------------------------------------------- helpers
def _msbuild():
    env = os.environ.get("XRAY_MSBUILD")
    if env and Path(env).is_file():
        return env
    if Path(MSBUILD_FIXED).is_file():
        return MSBUILD_FIXED
    wh = shutil.which("MSBuild.exe") or shutil.which("msbuild")
    if wh:
        return wh
    vswhere = Path(
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    )
    if vswhere.is_file():
        try:
            out = subprocess.run(
                [str(vswhere), "-latest", "-requires", "Microsoft.Component.MSBuild",
                 "-find", r"MSBuild\**\Bin\MSBuild.exe"],
                capture_output=True, text=True, timeout=60,
            )
            for line in out.stdout.splitlines():
                if line.strip() and Path(line.strip()).is_file():
                    return line.strip()
        except Exception:
            pass
    raise RuntimeError("MSBuild.exe not found (set XRAY_MSBUILD)")


def _find_cdb():
    env = os.environ.get("XRAY_CDB")
    if env and Path(env).is_file():
        return env
    wh = shutil.which("cdb.exe")
    if wh:
        return wh
    for cand in CDB_CANDIDATES:
        if Path(cand).is_file():
            return cand
    raise RuntimeError(
        "cdb.exe not found (install the Windows Kits debuggers or set XRAY_CDB)")


def _sln_configs():
    cfgs = []
    try:
        text = SLN.read_text(encoding="utf-8-sig", errors="replace")
    except OSError as e:
        raise RuntimeError(f"cannot read {SLN}: {e}")
    m = re.search(r"SolutionConfigurationPlatforms.*?=(.*?)EndGlobalSection",
                  text, re.S)
    if not m:
        return cfgs
    for line in m.group(1).splitlines():
        mm = re.match(r"\s*([A-Za-z0-9\-\.]+)\|x64\s*=", line)
        if mm and mm.group(1) not in cfgs:
            cfgs.append(mm.group(1))
    return cfgs


def _summarize_msbuild_log(path, max_lines=40):
    errors, warnings = [], []
    n_err = n_warn = 0
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            for line in f:
                if ERR_RE.search(line):
                    n_err += 1
                    if len(errors) < max_lines:
                        errors.append(line.rstrip()[:300])
                elif WARN_RE.search(line):
                    n_warn += 1
                    if len(warnings) < max_lines:
                        warnings.append(line.rstrip()[:300])
    except OSError as e:
        return {"ok": False, "error": str(e)}
    return {"ok": True, "errors": n_err, "warnings": n_warn,
            "error_lines": errors, "warning_lines": warnings}


def _read_text_shared(path):
    # Game log may be locked for writing; reopen via bytes is fine on Windows
    # for readers as long as we just open read-only.
    with open(path, "rb") as f:
        return f.read().decode("utf-8", errors="replace")


def _analyze_log_text(text, tail_n=60, err_n=15):
    lines = text.splitlines()
    low = text.lower()
    tail = lines[-tail_n:] if tail_n > 0 else []
    err_lines = [l[:300] for l in lines
                 if l.lstrip().startswith("!") or "script error" in l.lower()
                 or "fatal" in l.lower()]
    return {
        "total_lines": len(lines),
        "booted": "starting main loop" in low,
        "script_errors": low.count("script error"),
        "exceptions": low.count("exception"),
        "bang_lines": sum(1 for l in lines if l.lstrip().startswith("!")),
        "last_errors": err_lines[-err_n:],
        "tail": tail,
    }


def _git(args, timeout=60):
    try:
        out = subprocess.run(["git", "-C", str(REPO)] + args,
                             capture_output=True, text=True, timeout=timeout)
        return out.stdout.strip(), out.returncode
    except Exception as e:
        return f"<git failed: {e}>", 1


# ---------------------------------------------------------------- unpacked
def _uresolve(rel):
    # Confine to the unpacked tree: no absolutes, no ".." escapes.
    if not rel or not isinstance(rel, str):
        raise ValueError("pass a relative path inside tools/_unpacked")
    p = Path(rel)
    if p.is_absolute() or ".." in p.parts or str(rel).startswith("~"):
        raise ValueError(f"path escapes unpacked root: {rel!r}")
    full = (UNPACKED / p)
    try:
        full.relative_to(UNPACKED)
    except ValueError:
        raise ValueError(f"path escapes unpacked root: {rel!r}")
    return full


def _decode(data):
    # Anomaly text is UTF-8 or CP1251 (Cyrillic comments/strings).
    try:
        return data.decode("utf-8-sig"), "utf-8"
    except UnicodeDecodeError:
        return data.decode("cp1251", errors="replace"), "cp1251"


def _read_text_capped(path, max_bytes=65536):
    with open(path, "rb") as f:
        data = f.read(max_bytes + 1)
    truncated = len(data) > max_bytes
    text, enc = _decode(data[:max_bytes])
    if b"\x00" in data[:8192]:
        raise ValueError(f"binary file, use unpacked_list for inventory: {path.name}")
    return text, enc, truncated


def _iter_text_files(root, exts=None, cap=5000):
    # Yield text-ish files under root (cap total visited for sanity).
    found, scanned = [], 0
    for dirpath, _dirnames, filenames in os.walk(root):
        for fn in filenames:
            scanned += 1
            if scanned > cap:
                return found, scanned, True
            fp = Path(dirpath) / fn
            if exts is not None and fp.suffix.lower() not in exts:
                continue
            try:
                with open(fp, "rb") as f:
                    head = f.read(8192)
                if b"\x00" in head:
                    continue
            except OSError:
                continue
            found.append(fp)
    return found, scanned, False


def t_unpacked_list(a):
    sub = a.get("subdir", "") or ""
    base = _uresolve(sub) if sub else UNPACKED
    if not base.is_dir():
        raise ValueError(f"not a directory: {sub!r}")
    depth = min(int(a.get("depth", 2) or 2), 5)
    ext = (a.get("ext") or "").lower()
    limit = min(int(a.get("limit", 200) or 200), 1000)
    entries, truncated = [], False
    base_depth = len(base.parts)
    for dirpath, dirnames, filenames in os.walk(base):
        dirnames.sort()
        rel_d = Path(dirpath).relative_to(UNPACKED).as_posix()
        if len(Path(dirpath).parts) - base_depth > depth:
            dirnames[:] = []
            continue
        for fn in sorted(filenames):
            if ext and not fn.lower().endswith(ext):
                continue
            fp = Path(dirpath) / fn
            try:
                kb = round(fp.stat().st_size / 1024, 1)
            except OSError:
                continue
            entries.append({"path": f"{rel_d}/{fn}" if rel_d != "." else fn,
                            "kb": kb})
            if len(entries) >= limit:
                truncated = True
                dirnames[:] = []
                break
        if truncated:
            break
    return {"root": str(UNPACKED), "entries": entries,
            "count": len(entries), "truncated": truncated}


def t_unpacked_read(a):
    p = _uresolve(a.get("path", ""))
    if not p.is_file():
        raise ValueError(f"not a file: {a.get('path')!r}")
    max_bytes = min(int(a.get("max_bytes", 65536) or 65536), 262144)
    text, enc, truncated = _read_text_capped(str(p), max_bytes)
    return {"path": p.relative_to(UNPACKED).as_posix(), "encoding": enc,
            "truncated": truncated, "text": text}


def t_unpacked_grep(a):
    try:
        rx = re.compile(a.get("pattern", ""))
    except re.error as e:
        raise ValueError(f"bad regex: {e}")
    if not a.get("pattern"):
        raise ValueError("pass pattern")
    sub = a.get("subdir", "") or ""
    base = _uresolve(sub) if sub else UNPACKED
    if not base.is_dir():
        raise ValueError(f"not a directory: {sub!r}")
    inc = a.get("include")
    exts = (frozenset(e.lower() if e.startswith(".") else "." + e.lower()
                      for e in inc) if inc else TEXT_EXTS)
    max_hits = min(int(a.get("max_hits", 50) or 50), 200)
    files, scanned, scan_cut = _iter_text_files(base, exts)
    hits = []
    for fp in files:
        try:
            with open(fp, "rb") as f:
                text, _enc = _decode(f.read())
        except OSError:
            continue
        for i, line in enumerate(text.splitlines(), 1):
            if rx.search(line):
                hits.append({
                    "file": fp.relative_to(UNPACKED).as_posix(), "line": i,
                    "text": line.strip()[:240]})
                if len(hits) >= max_hits:
                    break
        if len(hits) >= max_hits:
            break
    return {"pattern": a.get("pattern"), "files_scanned": scanned,
            "scan_truncated": scan_cut, "hits": hits,
            "hits_truncated": len(hits) >= max_hits}


def _run_luacheck(files, timeout=300):
    if not LUACHECK.is_file():
        raise RuntimeError(
            f"luacheck.exe missing at {LUACHECK} - "
            f"run tools/xray-mcp/build_luacheck.bat first")
    try:
        p = subprocess.run([str(LUACHECK)] + files, capture_output=True,
                           text=True, timeout=timeout,
                           cwd=str(UNPACKED / "scripts"))
    except subprocess.TimeoutExpired:
        raise RuntimeError("luacheck timed out")
    failed = []
    ok = 0
    for line in p.stdout.splitlines():
        if line.startswith("OK "):
            ok += 1
        elif line.startswith("FAIL "):
            rest = line[5:]
            fn, _, err = rest.partition(": ")
            failed.append({"file": fn, "error": err[:300]})
    return {"checked": ok + len(failed), "ok": ok, "failed": failed,
            "note": "lua_help.script is docs, not loadable Lua (pre-existing)"}


def t_script_check(a):
    if a.get("all"):
        files = sorted(fp.name for fp in (UNPACKED / "scripts").glob("*.script"))
        if not files:
            raise RuntimeError("no scripts found under tools/_unpacked/scripts")
        return _run_luacheck(files)
    p = _uresolve(a.get("file", ""))
    if not p.is_file():
        raise ValueError(f"not a file: {a.get('file')!r}")
    try:
        arg = p.relative_to(UNPACKED / "scripts").as_posix()
    except ValueError:
        arg = str(p)  # outside scripts/: absolute path still works
    return _run_luacheck([arg])


def t_script_symbols(a):
    name = (a.get("name") or "").strip()
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_.:]*", name or ""):
        raise ValueError("pass name (Lua identifier, . or : allowed)")
    scripts = sorted((UNPACKED / "scripts").glob("*.script"))
    if not scripts:
        raise RuntimeError("no scripts found under tools/_unpacked/scripts")
    base = re.escape(name)
    def_re = re.compile(
        rf"^\s*(?:local\s+)?function\s+{base}\b|^\s*{base}\s*=\s*function\b")
    call_re = re.compile(rf"(?<![\w.:]){base}\s*\(")
    defs, callers, limit = [], [], 100
    for fp in scripts:
        try:
            with open(fp, "rb") as f:
                text, _enc = _decode(f.read())
        except OSError:
            continue
        for i, line in enumerate(text.splitlines(), 1):
            s = line.strip()
            if def_re.search(line):
                if len(defs) < limit:
                    defs.append({"file": fp.name, "line": i,
                                 "text": s[:200]})
            elif call_re.search(line):
                if len(callers) < limit:
                    callers.append({"file": fp.name, "line": i,
                                    "text": s[:200]})
    return {"name": name, "defs": defs, "callers": callers,
            "callers_truncated": len(callers) >= limit}


_LTX_SEC = re.compile(r"^\s*\[([^\]]+)\]\s*(?::\s*(.+))?\s*$")
_LTX_INC = re.compile(r'^\s*#include\s+"([^"]+)"')
_LTX_KV = re.compile(r"^\s*([^;=\s][^=]*?)\s*=\s*(.*?)\s*$")


def _ltx_load(path, seen, depth=0):
    # (section, key, value, origin) stream; includes inlined in order,
    # later keys win. Cycle-safe.
    if depth > 10:
        return []
    try:
        key = str(path.resolve())
    except OSError:
        return []
    if key in seen:
        return []
    seen.add(key)
    try:
        with open(path, "rb") as f:
            text, _enc = _decode(f.read())
    except OSError:
        return []
    out, section = [], None
    for line in text.splitlines():
        s = line.strip()
        if not s or s.startswith(";"):
            continue
        m = _LTX_INC.match(s)
        if m:
            out += _ltx_load(path.parent / m.group(1), seen, depth + 1)
            continue
        m = _LTX_SEC.match(s)
        if m:
            section = m.group(1).strip()
            continue
        if section is None:
            continue
        m = _LTX_KV.match(s)
        if m:
            val = m.group(2).strip()
            cm = re.search(r"\s;", val)
            if cm:  # trailing "; comment" (LTX convention, space or tab)
                val = val[:cm.start()].rstrip()
            out.append((section, m.group(1).strip(), val,
                        f"{path.name}:{section}"))
    return out


def t_ltx_get(a):
    p = _uresolve(a.get("file", ""))
    if not p.is_file():
        raise ValueError(f"not a file: {a.get('file')!r}")
    section = (a.get("section") or "").strip()
    if not section:
        raise ValueError("pass section")
    stream = _ltx_load(p, set())
    merged = {}
    order = []
    for sec, k, v, origin in stream:
        if (sec, k) not in merged:
            order.append((sec, k))
        merged[(sec, k)] = (v, origin)
    # Inheritance: [child]:p1,p2 - child keys win, then parents in order.
    parents = {}
    try:
        with open(p, "rb") as f:
            text, _enc = _decode(f.read())
    except OSError:
        text = ""
    for line in text.splitlines():
        m = _LTX_SEC.match(line.strip())
        if m and m.group(2):
            parents[m.group(1).strip()] = [
                x.strip() for x in m.group(2).split(",") if x.strip()]
    key = (a.get("key") or "").strip() or None
    chain = [section] + parents.get(section, [])

    def lookup(sec, k):
        return merged.get((sec, k))

    if key:
        for sec in chain:
            hit = lookup(sec, key)
            if hit is not None:
                return {"file": p.relative_to(UNPACKED).as_posix(),
                        "section": section, "key": key, "value": hit[0],
                        "defined_in": hit[1], "chain": chain}
        return {"file": p.relative_to(UNPACKED).as_posix(),
                "section": section, "key": key, "value": None,
                "defined_in": None, "chain": chain}
    resolved, origins = {}, {}
    for sec in reversed(chain):
        for (s, k) in order:
            if s == sec and k not in resolved:
                resolved[k], origins[k] = merged[(s, k)]
    return {"file": p.relative_to(UNPACKED).as_posix(), "section": section,
            "keys": resolved, "origins": origins, "chain": chain}


# ---------------------------------------------------------------- tools
def t_list_configs(_a):
    cfgs = _sln_configs()
    return {"solution": str(SLN), "configs": cfgs, "platform": "x64"}


def t_build_solution(a):
    cfg = a.get("configuration", "DX11")
    platform = a.get("platform", "x64")
    jobs = int(a.get("jobs", 4) or 4)
    timeout_s = min(int(a.get("timeout_s", 3600) or 3600), 10800)
    log_name = a.get("log_name") or (
        f"mcp_DX_{cfg}_{datetime.now():%Y%m%d_%H%M%S}.log")
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9\-\._]*", cfg):
        raise ValueError("bad configuration name")
    if platform != "x64":
        raise ValueError("only x64 is supported")
    if cfg not in _sln_configs():
        raise ValueError(f"unknown configuration {cfg!r}")
    TMPLOG.mkdir(parents=True, exist_ok=True)
    log_path = TMPLOG / log_name
    msb = _msbuild()
    cmd = [msb, str(SLN), f"/p:Configuration={cfg}", f"/p:Platform={platform}",
           f"/m:{jobs}", "/v:q", "/fl",
           f"/flp:logfile={log_path};verbosity=normal"]
    try:
        p = subprocess.run(cmd, capture_output=True, text=True,
                           timeout=timeout_s, cwd=str(SRC))
        exit_code = p.returncode
    except subprocess.TimeoutExpired:
        return {"ok": False, "timeout": True, "log": str(log_path),
                "hint": "build exceeded timeout_s; rerun with larger timeout_s"}
    summary = _summarize_msbuild_log(str(log_path))
    return {"ok": exit_code == 0, "exit_code": exit_code,
            "configuration": cfg, "log": str(log_path),
            "errors": summary.get("errors"), "warnings": summary.get("warnings"),
            "error_lines": summary.get("error_lines", [])[:20]}


def t_build_errors(a):
    name = a.get("log_name") or a.get("log_path")
    if not name:
        raise ValueError("pass log_name (file in Temp\\opencode) or log_path")
    p = Path(name)
    if not p.is_absolute():
        p = TMPLOG / p.name  # confine bare names to the temp log dir
    if not p.is_file():
        raise ValueError(f"log not found: {p}")
    kind = (a.get("kind") or "error").lower()
    max_lines = min(int(a.get("max_lines", 40) or 40), 200)
    s = _summarize_msbuild_log(str(p), max_lines=max_lines)
    if kind == "error":
        s.pop("warning_lines", None)
    elif kind == "warning":
        s.pop("error_lines", None)
    s["log"] = str(p)
    return s


def t_list_exes(_a):
    if not BINDIR.is_dir():
        raise RuntimeError(f"bin dir missing: {BINDIR}")
    out = []
    for p in sorted(BINDIR.glob("*.exe")):
        st = p.stat()
        out.append({"name": p.name, "mb": round(st.st_size / 1e6, 1),
                    "mtime": datetime.fromtimestamp(st.st_mtime).isoformat(
                        timespec="minutes")})
    return {"bin": str(BINDIR), "exes": out}


def t_game_log(a):
    lines_n = min(int(a.get("lines", 60) or 60), 300)
    name = a.get("log") or str(GAMELOG)
    p = Path(name)
    if not p.is_absolute():
        p = LOGDIR / p.name
    if not p.is_file():
        raise ValueError(f"log not found: {p}")
    st = p.stat()
    info = _analyze_log_text(_read_text_shared(str(p)), tail_n=lines_n)
    info.update({"log": str(p),
                 "mtime": datetime.fromtimestamp(st.st_mtime).isoformat(
                     timespec="seconds"),
                 "bytes": st.st_size})
    return info


def t_run_smoke(a):
    exe_name = a.get("exe", "AnomalyDX11.exe")
    if not re.fullmatch(r"[A-Za-z0-9_\-\. ]+\.exe", exe_name or ""):
        raise ValueError("bad exe name")
    if Path(exe_name).name != exe_name:
        raise ValueError("exe must be a bare file name in the game bin dir")
    target = BINDIR / exe_name
    if not target.is_file():
        raise ValueError(f"exe not found: {target}")
    args = shlex.split(a.get("args", "-nointro") or "", posix=False)
    timeout_s = min(int(a.get("timeout_s", 150) or 150), 1200)
    kill_after = bool(a.get("kill_after", True))
    staged = None
    stage_from = a.get("stage_from")
    if stage_from:
        src = Path(stage_from)
        if not src.is_absolute() or not src.is_file():
            raise ValueError("stage_from must be an existing absolute path")
        bak = target.with_name(
            target.name + datetime.now().strftime(".bak-mcp-%Y%m%d-%H%M%S"))
        shutil.copy2(target, bak)
        shutil.copy2(src, target)
        staged = {"from": str(src), "backup": str(bak)}
    start_size = target and GAMELOG.is_file() and GAMELOG.stat().st_size or 0
    proc = subprocess.Popen([str(target)] + args, cwd=str(BINDIR))
    timed_out = False
    try:
        proc.wait(timeout=timeout_s)
        exit_code = proc.returncode
    except subprocess.TimeoutExpired:
        timed_out = True
        exit_code = None
        if kill_after:
            proc.kill()
            try:
                proc.wait(timeout=30)
                exit_code = proc.returncode
            except subprocess.TimeoutExpired:
                pass
    session_text = ""
    if GAMELOG.is_file():
        with open(GAMELOG, "rb") as f:
            f.seek(0, 2)
            end = f.tell()
            f.seek(min(start_size, end))
            session_text = f.read().decode("utf-8", errors="replace")
    verdict = _analyze_log_text(session_text or _read_text_shared(str(GAMELOG))
                                if GAMELOG.is_file() else "", tail_n=25)
    return {"exe": exe_name, "args": args, "pid": proc.pid,
            "timed_out": timed_out, "exit_code": exit_code,
            "killed": timed_out and kill_after, "staged": staged,
            "booted": verdict["booted"],
            "script_errors": verdict["script_errors"],
            "exceptions": verdict["exceptions"],
            "last_errors": verdict["last_errors"][-10:]}


def _cdb_crash_report(out, max_lines=120):
    """Pull the crash-relevant lines out of raw cdb output."""
    keep = []
    interesting = (
        "Access violation", "code c0000005", "exception", "ExceptionAddress",
        "FAULTING", "WARNING:", "*** ", "ERROR:", "Unable to load",
        "DbgBreakPoint", "Symbol file could not be found",
        "Cannot find", "Fatal",
    )
    lines = out.splitlines()
    # exception banner + faulting frame + our own module's frames
    hdr = [l for l in lines
           if any(k in l for k in interesting)]
    frames = [l for l in lines
              if re.search(r"^\s*(ChildEBP|Child-SP|RetAddr|Args to Child)", l)
              or re.match(r"^\s*[0-9a-f`]{6,} [0-9a-f`]{8,}\s", l)
              or re.search(r"\]\s+\S+\!.*\+\w+", l)]
    keep.extend(hdr[:40])
    keep.extend(frames[:60])
    # last lines are where the break landed (stack right after the stop)
    tail = lines[-25:]
    for l in tail:
        if l not in keep:
            keep.append(l)
    return keep[:max_lines]


def t_debug_crash(a):
    """Launch a game exe under CDB, stop on the first fatal exception and
    report address/offset + stack. This is the tool for the immediate
    0xC0000005-on-startup case, where exit codes say nothing."""
    exe_name = a.get("exe", "AnomalyDX11.exe")
    if not re.fullmatch(r"[A-Za-z0-9_\-\. ]+\.exe", exe_name or ""):
        raise ValueError("bad exe name")
    if Path(exe_name).name != exe_name:
        raise ValueError("exe must be a bare file name in the game bin dir")
    target = BINDIR / exe_name
    if not target.is_file():
        raise ValueError(f"exe not found: {target}")
    args = shlex.split(a.get("args", "-nointro") or "", posix=False)
    timeout_s = min(int(a.get("timeout_s", 240) or 240), 1200)
    stop_what = (a.get("stop_what") or "av").strip() or "av"
    cmds = (a.get("commands")
            or ".exr -1; kn 80; .frame /c 60; lm")
    debug_children = bool(a.get("debug_children", False))

    cdb = _find_cdb()
    # Symbol path: exe dir first (PDB usually lives beside the exe or, for
    # repo builds, in the build output dir XRAY_PDB_DIR).
    symdirs = []
    for d in (target.parent, PDBDIR):
        s = str(d)
        if Path(d).is_dir() and s not in symdirs:
            symdirs.append(s)
    sympath = ";".join(symdirs)

    argv = [cdb, "-g", "-G"]          # skip initial/final breakpoints
    if debug_children:
        argv.append("-o")
    argv += ["-lines", "-y", sympath,
             "-c", f".symopt+0x10; sxe {stop_what}; g; {cmds}; q",
             str(target)] + args

    logpath = TMPLOG / f"cdb_{exe_name}.log"
    timed_out = False
    exit_code = None
    try:
        logf = open(logpath, "w", encoding="utf-8", errors="replace")
    except OSError as e:
        raise RuntimeError(f"cannot write {logpath}: {e}")
    try:
        proc = subprocess.Popen(argv, cwd=str(BINDIR), stdin=subprocess.DEVNULL,
                                stdout=logf, stderr=subprocess.STDOUT)
    except OSError as e:
        logf.close()
        raise RuntimeError(f"cannot start cdb: {e}")
    try:
        proc.wait(timeout=timeout_s)
        exit_code = proc.returncode
    except subprocess.TimeoutExpired:
        timed_out = True
        subprocess.run(["taskkill", "/F", "/T", "/PID", str(proc.pid)],
                       capture_output=True)
        try:
            proc.wait(timeout=30)
        except subprocess.TimeoutExpired:
            pass
    finally:
        logf.close()

    out = ""
    try:
        out = logpath.read_text(encoding="utf-8", errors="replace")
    except OSError as e:
        out = f"<cannot read cdb log: {e}>"

    crashed = ("Access violation" in out or "code c0000005" in out
               or ".exr" in out and "ExceptionAddress" in out)
    booted = "starting main loop" in out.lower()
    return {
        "exe": exe_name, "args": args, "cdb": cdb,
        "sympath": sympath,
        "exit_code": exit_code, "timed_out": timed_out,
        "crashed": crashed, "booted": booted,
        "full_log": str(logpath),
        "crash_lines": _cdb_crash_report(out),
        "tail": out.splitlines()[-30:],
    }


def t_git_info(_a):
    branch, _ = _git(["branch", "--show-current"])
    status, _ = _git(["status", "--short"])
    log, _ = _git(["log", "--oneline", "-5"])
    status_lines = status.splitlines()
    return {"repo": str(REPO), "branch": branch,
            "dirty_files": len(status_lines),
            "status": status_lines[:30],
            "recent": log.splitlines()}


def _luahook_url():
    base = os.environ.get("XRAY_LUAHOOK_URL", "http://127.0.0.1:18751")
    return base.rstrip("/")


def _luahook_call(path, payload, timeout_s):
    import urllib.request
    import urllib.error
    url = _luahook_url() + path
    data = json.dumps(payload).encode("utf-8") if payload is not None else None
    req = urllib.request.Request(url, data=data,
                                 headers={"Content-Type": "application/json"},
                                 method="POST" if data is not None else "GET")
    try:
        with urllib.request.urlopen(req, timeout=timeout_s) as r:
            return json.loads(r.read().decode("utf-8", errors="replace"))
    except urllib.error.HTTPError as e:
        try:
            return json.loads(e.read().decode("utf-8", errors="replace"))
        except Exception:
            return {"ok": False, "error": f"http {e.code}"}
    except Exception as e:
        return {"ok": False,
                "error": f"game not reachable at {url} ({e}). "
                         f"Launch the game with -luahook first."}


def t_lua_status(_a):
    return _luahook_call("/status", None, 10)


def t_lua_exec(a):
    code = a.get("code", "")
    file = a.get("file", "")
    if not code and not file:
        raise ValueError("pass code or file")
    if len(code) > 256 * 1024:
        raise ValueError("code too large (256KB cap)")
    timeout_ms = min(int(a.get("timeout_ms", 5000) or 5000), 120000)
    payload = {"timeout_ms": timeout_ms}
    if file:
        payload["file"] = file
    else:
        payload["code"] = code
    # HTTP wait must cover queue + in-game exec + margin.
    return _luahook_call("/exec", payload, timeout_ms / 1000 + 30)


TOOLS = {
    "list_configs": (t_list_configs, "Solution configurations from engine-vs2022.sln.",
                     {"type": "object", "properties": {}}),
    "build_solution": (t_build_solution, "Full-solution MSBuild + summarized log.",
                       {"type": "object", "properties": {
                           "configuration": {"type": "string", "default": "DX11"},
                           "platform": {"type": "string", "default": "x64"},
                           "jobs": {"type": "integer", "default": 4},
                           "timeout_s": {"type": "integer", "default": 3600},
                           "log_name": {"type": "string"}}}),
    "build_errors": (t_build_errors, "Parse an MSBuild log for errors/warnings.",
                     {"type": "object", "properties": {
                         "log_name": {"type": "string"},
                         "log_path": {"type": "string"},
                         "kind": {"type": "string", "default": "error"},
                         "max_lines": {"type": "integer", "default": 40}}}),
    "list_exes": (t_list_exes, "Game bin .exe inventory with size/mtime.",
                  {"type": "object", "properties": {}}),
    "game_log": (t_game_log, "Tail + verdict of xray_gamer.log.",
                 {"type": "object", "properties": {
                     "lines": {"type": "integer", "default": 60},
                     "log": {"type": "string"}}}),
    "run_smoke": (t_run_smoke, "Launch a game exe, wait, report log verdict. Backs up before stage-over.",
                  {"type": "object", "properties": {
                      "exe": {"type": "string", "default": "AnomalyDX11.exe"},
                      "args": {"type": "string", "default": "-nointro"},
                      "timeout_s": {"type": "integer", "default": 150},
                      "kill_after": {"type": "boolean", "default": True},
                      "stage_from": {"type": "string"}}}),
    "run_cdb": (t_debug_crash, "Launch a game exe under CDB, break on the first fatal exception (default access violation) and report address/offset + call stack. Use for immediate startup crashes.",
                {"type": "object", "properties": {
                    "exe": {"type": "string", "default": "AnomalyDX11.exe"},
                    "args": {"type": "string", "default": "-nointro"},
                    "timeout_s": {"type": "integer", "default": 240},
                    "stop_what": {"type": "string", "default": "av"},
                    "commands": {"type": "string",
                                 "default": ".exr -1; kn 80; .frame /c 60; lm"},
                    "debug_children": {"type": "boolean", "default": False}}}),
    "git_info": (t_git_info, "Branch / dirty count / recent commits (read-only).",
                 {"type": "object", "properties": {}}),
    "lua_status": (t_lua_status, "In-game LuaHook status (needs game running with -luahook).",
                   {"type": "object", "properties": {}}),
    "lua_exec": (t_lua_exec, "Execute Lua code/file on the game's lua_State via the hook. Returns result or error.",
                 {"type": "object", "properties": {
                     "code": {"type": "string"},
                     "file": {"type": "string"},
                     "timeout_ms": {"type": "integer", "default": 5000}}}),
    "unpacked_list": (t_unpacked_list, "List files under tools/_unpacked (inventory incl. binary).",
                      {"type": "object", "properties": {
                          "subdir": {"type": "string", "default": ""},
                          "depth": {"type": "integer", "default": 2},
                          "ext": {"type": "string"},
                          "limit": {"type": "integer", "default": 200}}}),
    "unpacked_read": (t_unpacked_read, "Read a text file under tools/_unpacked.",
                      {"type": "object", "properties": {
                          "path": {"type": "string"},
                          "max_bytes": {"type": "integer", "default": 65536}}}),
    "unpacked_grep": (t_unpacked_grep, "Regex search across unpacked text files.",
                      {"type": "object", "properties": {
                          "pattern": {"type": "string"},
                          "subdir": {"type": "string", "default": ""},
                          "include": {"type": "array", "items": {"type": "string"}},
                          "max_hits": {"type": "integer", "default": 50}}}),
    "script_check": (t_script_check, "LuaJIT-2.1 syntax check (parse-only, never executes). Single file or all scripts.",
                     {"type": "object", "properties": {
                         "file": {"type": "string"},
                         "all": {"type": "boolean", "default": False}}}),
    "script_symbols": (t_script_symbols, "Find Lua function definitions and callers across game scripts.",
                       {"type": "object", "properties": {
                           "name": {"type": "string"}}}),
    "ltx_get": (t_ltx_get, "Read .ltx config values with #include + inheritance resolved.",
                {"type": "object", "properties": {
                    "file": {"type": "string"},
                    "section": {"type": "string"},
                    "key": {"type": "string"}}}),
}


# ---------------------------------------------------------------- protocol
def _send(obj):
    sys.stdout.write(json.dumps(obj, ensure_ascii=False) + "\n")
    sys.stdout.flush()


def _handle(msg):
    mid = msg.get("id")
    method = msg.get("method", "")
    params = msg.get("params") or {}

    def result(r):
        if mid is not None:
            _send({"jsonrpc": "2.0", "id": mid, "result": r})

    def error(code, message):
        if mid is not None:
            _send({"jsonrpc": "2.0", "id": mid,
                   "error": {"code": code, "message": message}})

    try:
        if method == "initialize":
            want = (params.get("protocolVersion") or PROTOCOL_VERSION)
            result({"protocolVersion": want,
                    "capabilities": {"tools": {}},
                    "serverInfo": {"name": SERVER_NAME,
                                   "version": SERVER_VERSION}})
        elif method in ("notifications/initialized", "notifications/cancelled"):
            pass
        elif method == "ping":
            result({})
        elif method == "tools/list":
            result({"tools": [
                {"name": n, "description": d, "inputSchema": s}
                for n, (fn, d, s) in TOOLS.items()]})
        elif method == "tools/call":
            name = params.get("name", "")
            entry = TOOLS.get(name)
            if not entry:
                error(-32602, f"unknown tool: {name}")
                return
            fn = entry[0]
            try:
                out = fn(params.get("arguments") or {})
                result({"content": [{"type": "text",
                                     "text": json.dumps(out, indent=1)[:12000]}]})
            except Exception as e:
                result({"content": [{"type": "text",
                                     "text": f"{name} failed: {e}"}],
                        "isError": True})
        else:
            if mid is not None:
                error(-32601, f"method not found: {method}")
    except Exception:
        error(-32603, traceback.format_exc(limit=3))


def main():
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            _handle(json.loads(line))
        except json.JSONDecodeError:
            continue


if __name__ == "__main__":
    main()
