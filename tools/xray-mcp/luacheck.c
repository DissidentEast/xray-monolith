/* luacheck: parse-only Lua syntax check using the repo's LuaJIT.
** Usage: luacheck.exe file1.script [file2.script ...]
** Prints "OK <file>" or "FAIL <file>: <message>" per file.
** Exit 0 when all parse, 1 otherwise. Never executes anything.
** Build with the same mode flags as lua51.lib (LUAJIT_DISABLE_GC64). */
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main(int argc, char** argv)
{
    int i, fails = 0;
    lua_State* L;

    if (argc < 2) {
        fprintf(stderr, "usage: luacheck.exe file...\n");
        return 2;
    }
    L = luaL_newstate();
    if (!L) {
        fprintf(stderr, "luacheck: cannot create Lua state\n");
        return 2;
    }
    for (i = 1; i < argc; i++) {
        int st = luaL_loadfile(L, argv[i]);
        if (st != 0) {
            const char* msg = lua_tostring(L, -1);
            printf("FAIL %s: %s\n", argv[i], msg ? msg : "?");
            lua_pop(L, 1);
            fails++;
        } else {
            printf("OK %s\n", argv[i]);
            lua_pop(L, 1); /* drop the compiled chunk */
        }
        fflush(stdout);
    }
    lua_close(L);
    return fails ? 1 : 0;
}
