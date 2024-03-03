cmake_minimum_required(VERSION 3.0)

project(HookDemo)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-DUNICODE)

include_directories(${PROJECT_SOURCE_DIR}/src/)

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
else()
SET(CMAKE_BUILD_TYPE "Debug")
endif()

SET(SYSTEM_TYPE "windows")
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(CMAKE_SYSTEM_PROCESSOR x64)
ELSE()
    SET(CMAKE_SYSTEM_PROCESSOR x86)
ENDIF()

add_executable(ConsoleHook ${CMAKE_SOURCE_DIR}/src/Test.cpp ${CMAKE_SOURCE_DIR}/src/LowerHook.cpp ${CMAKE_SOURCE_DIR}/src/JournalHook.cpp)
add_library(HookDemo SHARED ${CMAKE_SOURCE_DIR}/src/Hook.cpp)
