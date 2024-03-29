cmake_minimum_required(VERSION 2.8)

project(LuaSandboxServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Lua REQUIRED)
find_package(Boost COMPONENTS program_options system iostreams REQUIRED)
find_package(Websocketpp REQUIRED)

add_executable(server server.cpp luasandbox.cpp luasandboxserver.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR} ${LUA_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(server ${LUA_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
