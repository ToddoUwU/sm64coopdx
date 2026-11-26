// Syntax-checks Lua files with the game's Lua library: loads (compiles) each file without running it.
#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
int main(int argc, char **argv) {
    int failures = 0;
    for (int i = 1; i < argc; i++) {
        lua_State *L = luaL_newstate();
        if (luaL_loadfile(L, argv[i]) != LUA_OK) {
            printf("FAIL %s\n", lua_tostring(L, -1));
            failures++;
        } else {
            printf("ok   %s\n", argv[i]);
        }
        lua_close(L);
    }
    return failures != 0;
}
