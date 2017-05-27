local function shallowCopy(t)
	local ret = {}
	for i, x in pairs(t) do
		ret[i] = x
	end
	return ret
end

function setupSafeEnvironment()
	LuaSandboxEnvironment = shallowCopy(_G) -- Safe Environment
	LuaSandboxEnvironment._G = LuaSandboxEnvironment

	local unsafe = {"LuaSandboxEnvironment", "setupSafeEnvironment", "setTimeout", "_outputMessage", "collectgarbage", "dofile", "load", "loadfile", "loadstring", "rawequal", "rawget", "rawset", "setfenv", "getfenv", "module", "require", "package", "debug", "io", {"bit32"}, {"table"}, {"coroutine"}, {"math"}, {"os", "date", "execute", "exit", "getenv", "remove", "rename", "setlocale", "tmpname"}, {"string", "dump"}}
	for _, value in ipairs(unsafe) do
		if type(value) == "table" then
			if type(LuaSandboxEnvironment[value[1]]) == "table" then
				LuaSandboxEnvironment[value[1]] = shallowCopy(LuaSandboxEnvironment[value[1]])
				for i = 2, #value do
					LuaSandboxEnvironment[value[1]][value[i]] = nil
				end
			end
		else
			LuaSandboxEnvironment[value] = nil
		end
	end

	LuaSandboxEnvironment.print = function(...)
		local args = {...}
		for i = 1, #args do
			args[i] = tostring(args[i]) or "nil"
		end
		LuaSandboxEnvironment._outputMessage = (LuaSandboxEnvironment._outputMessage or "") .. table.concat(args, '\t') .. "\n"
	end

	LuaSandboxEnvironment.io = {
		write = function(...)
			local args = {...}
			for i = 1, #args do
				args[i] = tostring(args[i]) or "nil"
			end
			LuaSandboxEnvironment._outputMessage = (LuaSandboxEnvironment._outputMessage or "") .. table.concat(args) .. "\n"
		end
	}
end

local safeString = false
if not safeString then
	safeString = true
	debug.getmetatable("").__metatable = false
end

if not setfenv then -- Lua 5.2
	-- based on http://lua-users.org/lists/lua-l/2010-06/msg00314.html
	-- this assumes f is a function
	local function findenv(f)
		local level = 1
		repeat
			local name, value = debug.getupvalue(f, level)
			if name == '_ENV' then return level, value end
			level = level + 1
		until name == nil
		return nil
	end

	getfenv = function (f) return(select(2, findenv(f)) or _G) end
	setfenv = function (f, t)
		local level = findenv(f)
		if level then debug.setupvalue(f, level, t) end
		return f
	end
end

function setTimeout(seconds)
	local t = os.clock()
	function check()
		if os.clock() - t > seconds then
			debug.sethook()
			error("Timeout!")
		end
	end
	debug.sethook(check, "", 100000);
end
