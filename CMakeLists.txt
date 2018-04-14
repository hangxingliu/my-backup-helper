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

# ============================
#     Check dependencies
execute_process(
	COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/check.sh
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	RESULT_VARIABLE CHECK_DEPS_OK
	OUTPUT_VARIABLE CHECK_DEPS_STDOUT
	ERROR_VARIABLE CHECK_DEPS_STDERR)
if(NOT CHECK_DEPS_OK STREQUAL "0")
	message(FATAL_ERROR "\ncheck dependencies failed!\n"
		"stdout:\n${CHECK_DEPS_STDOUT}stderr:\n${CHECK_DEPS_STDERR}\n")
else()
	message(STATUS ${CHECK_DEPS_STDOUT})
endif()
# ============================


file(GLOB_RECURSE SOURCE_FILES .
	"libs/*.cc"
	"libs/*.hpp"
	"src/*.hpp"
	"src/*.cc")
add_executable(${TARGET_NAME} ${SOURCE_FILES})
install(TARGETS ${TARGET_NAME} DESTINATION bin)
