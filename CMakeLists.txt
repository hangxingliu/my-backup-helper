cmake_minimum_required(VERSION 2.8)

project(my-backup-helper)
set(TARGET_NAME "backup-helper")


# ============================
#     Compiler Options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -O2")
# for gason
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
# ============================

file(GLOB_RECURSE SOURCE_FILES .
        "libs/*.cc"
        "libs/*.hpp"
        "src/*.hpp"
	    "src/*.cc")
add_executable(${TARGET_NAME} ${SOURCE_FILES})
install(TARGETS ${TARGET_NAME} DESTINATION bin)
