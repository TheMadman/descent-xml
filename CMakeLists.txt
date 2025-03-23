cmake_minimum_required(VERSION 3.30.5)
project(XMLTree)

add_subdirectory(src)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
