set(SOURCES classifier.c lex.c parse.c validate.c)

add_library(xmltreeobj OBJECT ${SOURCES})

set_property(TARGET xmltreeobj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(xmltree SHARED)
target_link_libraries(xmltree xmltreeobj)

add_library(xmltreestatic STATIC)
target_link_libraries(xmltreestatic xmltreeobj adtstatic)

# hate
target_include_directories(xmltree
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_include_directories(xmltreestatic
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)

install(TARGETS xmltree xmltreestatic
	DESTINATION lib)
install(DIRECTORY xmltree
	DESTINATION include)
