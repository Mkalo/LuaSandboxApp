cmake_minimum_required(VERSION 2.8)

project(LuaVM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Lua REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(Websocketpp REQUIRED)

add_executable(luavm luavm.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${LUA_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(luavm ${LUA_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
