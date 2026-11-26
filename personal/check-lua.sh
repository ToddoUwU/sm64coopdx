#!/bin/bash
# Syntax-checks Lua mod files with the game's own Lua 5.3 library (compiles them without running).
#
#   personal/check-lua.sh                 every .lua file under mods/
#   personal/check-lua.sh FILE...         only these files
set -euo pipefail
cd "$(dirname "$0")/.."
CHECKER=build/personal-luacheck
if [[ ! -x $CHECKER || personal/tools/luacheck.c -nt $CHECKER ]]; then
    mkdir -p build
    gcc -O1 -Ilib/lua/include personal/tools/luacheck.c lib/lua/linux/liblua53.a -lm -ldl -o "$CHECKER"
fi
if [[ $# -eq 0 ]]; then
    mapfile -t FILES < <(find mods -name '*.lua' | sort)
else
    FILES=("$@")
fi
"$CHECKER" "${FILES[@]}" | grep -v '^ok ' || true
"$CHECKER" "${FILES[@]}" > /dev/null && echo "all ${#FILES[@]} files compile"
