set(SOURCES classifier.c lex.c parse.c validate.c)

add_library(descent_xmlobj OBJECT ${SOURCES})

set_property(TARGET descent_xmlobj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(descent_xml SHARED)
target_link_libraries(descent_xml descent_xmlobj)

add_library(descent_xmlstatic STATIC)
target_link_libraries(descent_xmlstatic descent_xmlobj adtstatic)

# hate
target_include_directories(descent_xml
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_include_directories(descent_xmlstatic
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)

install(TARGETS descent_xml descent_xmlstatic
	DESTINATION lib)
install(FILES descent_xml.h
	DESTINATION include)
install(DIRECTORY descent_xml
	DESTINATION include)
