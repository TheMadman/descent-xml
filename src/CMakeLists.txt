set(SOURCES classifier.c lex.c parse.c validate.c)

add_library(descent_xmlobj OBJECT ${SOURCES})

add_library(descent_xml SHARED)
target_link_libraries(descent_xml descent_xmlobj)
add_library(descent_xmlstatic STATIC)
target_link_libraries(descent_xmlstatic descent_xmlobj adtstatic)

target_include_directories(descent_xmlobj
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS descent_xml descent_xmlstatic
	DESTINATION lib)
install(FILES descent_xml.h
	DESTINATION include)
install(DIRECTORY descent_xml
	DESTINATION include)
