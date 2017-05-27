#ifndef LUASANDBOX_H
#define LUASANDBOX_H

#include <string>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

class LuaSandbox {
	public:
		LuaSandbox();
		~LuaSandbox();

		std::string runScriptSafely(std::string chunk, int timeout = 30);

	private:
		lua_State* L;

		static int traceback (lua_State* L);
};

#endif
