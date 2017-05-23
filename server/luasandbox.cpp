#include "luasandbox.h"

#include <stdexcept>

LuaSandbox::LuaSandbox() {
	L = luaL_newstate();
	if (!L) {
		throw std::runtime_error("Couldn't allocate enough memory for a new lua state.");
	}

	luaL_openlibs(L);
	// Load lua library functions
	if (luaL_dofile(L, "./library.lua") != 0) {
		throw std::runtime_error("Coudln't open libary.lua.");
	};
}

LuaSandbox::~LuaSandbox() {
	lua_close(L);
}

std::string LuaSandbox::runScriptSafely(std::string chunk, int timeout) {
	std::string ret;

	// Call setupSafeEnvironment
	lua_getglobal(L, "setupSafeEnvironment");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return "Internal Error: couldn't run this script.";
	}
	if (lua_pcall(L, 0, 0, 0) != 0) {
		lua_pop(L, 1);
		return "Internal Error: couldn't run this script.";
	}

	// Call setTimeout
	if (timeout > 0) {
		lua_getglobal(L, "setTimeout");
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 1);
			return "Internal Error: couldn't run this script.";
		}
		lua_pushinteger(L, timeout);
		if (lua_pcall(L, 1, 0, 0) != 0) {
			lua_pop(L, 1);
			return "Internal Error: couldn't run this script.";
		}
	}

	// Load setfenv to use in loaded function
	lua_getglobal(L, "setfenv");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return "Internal Error: couldn't run this script.";
	}

	if (luaL_loadstring(L, chunk.c_str()) != 0) {
		// Error while loading compiling chunk
		ret = lua_tostring(L, -1);
		lua_pop(L, 2);
	} else {
		lua_getglobal(L, "LuaSandboxEnvironment");
		lua_call(L, 2, 1);

		int base = lua_gettop(L);
		lua_pushcfunction(L, traceback);
		lua_insert(L, base);
		
		if (lua_pcall(L, 0, 0, base) != 0) {
			ret = lua_tostring(L, -1);
		} else {
			lua_getglobal(L, "LuaSandboxEnvironment");
			if (!lua_istable(L, -1)) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}
			lua_pushstring(L, "_outputMessage");
			lua_gettable(L, -2);
			if (lua_isstring(L, -1)) {
				ret = lua_tostring(L, -1);
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 1);
		lua_remove(L, base);
	}

	return ret;
}

int LuaSandbox::traceback(lua_State *L) {
	if (!lua_isstring(L, 1)) return 1;

	lua_getglobal(L, "debug");
	if (!lua_istable(L, -1)) {
    	lua_pop(L, 1);
    	return 1;
  	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
  	}
  	lua_pushvalue(L, 1);  /* pass error message */
  	lua_pushinteger(L, 2);  /* skip this function and traceback */
  	lua_call(L, 2, 1);  /* call debug.traceback */
  	return 1;
}
