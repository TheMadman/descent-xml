add_library(xmltree SHARED classifier.c)
add_library(xmltreestatic STATIC classifier.c)

# hate
target_include_directories(xmltree
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_include_directories(xmltreestatic
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)

install(TARGETS xmltree
	DESTINATION lib)
install(DIRECTORY xmltree
	DESTINATION include)
