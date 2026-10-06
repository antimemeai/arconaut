#include <iostream>
#include <lua.hpp>
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  lua_State *L = luaL_newstate();
  if (!L)
    return 2;
  luaL_openlibs(L);
  lua_pushstring(L, argv[2]);
  lua_setglobal(L, "helper_path");
  int code = luaL_loadfile(L, argv[1]);
  if (code == LUA_OK)
    code = lua_pcall(L, 0, 0, 0);
  if (code != LUA_OK)
    std::cerr << lua_tostring(L, -1) << '\n';
  lua_close(L);
  return code == LUA_OK ? 0 : 1;
}
