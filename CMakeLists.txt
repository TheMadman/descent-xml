cmake_minimum_required(VERSION 3.29.0)
project(DescentXML)

add_subdirectory(src)

if (BUILD_EXAMPLES)
	add_subdirectory(pages)
endif()

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
