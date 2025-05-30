include(GenerateExportHeader)

set(SOURCES classifier.c lex.c parse.c validate.c)
set(CMAKE_SHARED_LIBRARY_ENABLE_EXPORTS True)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(descent_xmlobj OBJECT ${SOURCES})

add_library(descent_xml SHARED)
target_link_libraries(descent_xml descent_xmlobj)
generate_export_header(descent_xml)

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
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/descent_xml_export.h
	DESTINATION include/descent_xml)
