# xray-mcp — project MCP server (no more blind runs)

Stdlib-only Python MCP server over stdio. No `pip install` needed.

Tools: `list_configs`, `build_solution`, `build_errors`, `list_exes`,
`game_log`, `run_smoke`, `git_info`, `lua_status`, `lua_exec`,
`unpacked_list`, `unpacked_read`, `unpacked_grep`, `script_check`,
`script_symbols`, `ltx_get`. Full descriptions live in `server.py`.

## Unpacked game data (`XRAY_UNPACKED_DIR`, default `C:\anomaly 1.5.3\tools\_unpacked`)

Read-only coverage of the unpacked tree (~48k files, mostly binary):

- `unpacked_list` — inventory by subdir/depth/extension (the only tool
  that touches binary assets: name + size, never content).
- `unpacked_read` — one text file (Lua, LTX, shaders, XML...), encoding
  auto-detected (UTF-8 or CP1251), 64KB default cap.
- `unpacked_grep` — regex across text files (3802 configs scan in seconds).
- `script_check` — syntax-checks game scripts with the repo's own
  LuaJIT 2.1 (`tools/xray-mcp/luacheck.c`, parse-only, never executes).
  Needs `build_luacheck.bat` run once (exe lives in `%TEMP%\opencode`,
  override via `XRAY_LUACHECK`). Baseline: 412/413 parse clean; the one
  failure (`lua_help.script`) is a docs file with pseudo-syntax, never
  loaded by the engine — pre-existing, not a regression.
- `script_symbols` — Lua function definitions + callers across scripts
  (e.g. every `math.mod` caller the 2.1 shim covers).
- `ltx_get` — `.ltx` values with `#include` inlining and `[child]:parent`
  inheritance resolved; reports `defined_in` + chain. Trailing `;` comments
  stripped per LTX convention.

Confinement: all paths must be relative inside the unpacked root —
absolute paths and `..` escapes are rejected.

Safety rules (enforced in code):
- exes only launch from `C:\anomaly 1.5.3\bin` (override: `XRAY_GAME_DIR`)
- build logs only under `%TEMP%\opencode` (override: `XRAY_LOG_DIR`)
- `run_smoke.stage_from` must be an absolute path; the target exe is
  backed up to `.bak-mcp-<timestamp>` before any copy
- `git_info` is read-only

Repo wiring: root `opencode.jsonc` registers it as server `xray-monolith`.
After pulling, run `opencode mcp list` — expect `xray-monolith connected`.
If it shows otherwise, `opencode service restart` then list again.

## In-game Lua hook (`lua_status` / `lua_exec`)

`src/xrGame/LuaHook.{h,cpp}` — a loopback HTTP listener inside the game
(127.0.0.1 only) that executes Lua on the game's own `lua_State`:

- Launch with `-luahook` (optional port: `-luahook=18752`, default 18751).
- `lua_status` → `{"ready": true/false, ...}` (`ready` = script engine owns
  a live state; false in menu before init or with no level — still queryable).
- `lua_exec` with `code` (snippet) or `file` (absolute `.script`/`.lua`/
  `.txt` path) → `{"ok": true, "result": "..."}` with up to 16 return
  values repr'd, or `{"ok": false, "error": "..."}` (compile/runtime error,
  engine-not-ready, or instruction timeout).
- Snippets run under an instruction-count watchdog (`timeout_ms`, default
  5000, 0 = unbounded) so a hung test can't freeze the game.
- Manual twin: in-game console `lua_hook <code>` prints the same result.
- Override URL: `XRAY_LUAHOOK_URL` (default `http://127.0.0.1:18751`).

Off by default: without `-luahook` the game spawns no thread, opens no
socket, and only pays one `strstr` per frame.

