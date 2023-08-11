cmake_minimum_required(VERSION 3.5)
project(kkd-cpp VERSION 0.1 DESCRIPTION "KKD's C++ Library")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wpedantic	\
	-Wno-gnu-zero-variadic-macro-arguments -Wno-nested-anon-types		\
	-Wno-gnu-anonymous-struct -Wno-variadic-macros				\
	-Wno-zero-length-array -Wno-c99-designator"
)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(KKD_CPP_OPT_BUILD_STATIC "Build static library and executable" OFF)
option(KKD_CPP_OPT_BUILD_TESTS "Build test suite" OFF)
option(KKD_CPP_OPT_ASYNC "Asynchronous runtime support" ON)

message("-- Build options:")
message("--   KKD_CPP_OPT_BUILD_STATIC=${KKD_CPP_OPT_BUILD_STATIC}")
message("--   KKD_CPP_OPT_BUILD_TESTS=${KKD_CPP_OPT_BUILD_TESTS}")
message("--   KKD_CPP_OPT_ASYNC=${KKD_CPP_OPT_ASYNC}")

if (KKD_CPP_OPT_BUILD_STATIC)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(BUILD_SHARED_LIBS OFF)
endif (KKD_CPP_OPT_BUILD_STATIC)

add_subdirectory(src)
if (KKD_CPP_OPT_BUILD_TESTS)
	add_subdirectory(tests)
endif (KKD_CPP_OPT_BUILD_TESTS)
