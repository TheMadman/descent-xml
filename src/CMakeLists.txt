set(SOURCES classifier.c lex.c parse.c validate.c)

add_library(descent-xmlobj OBJECT ${SOURCES})
add_library(descent-xml SHARED)
target_link_libraries(descent-xml descent-xmlobj)
add_library(descent-xmlstatic STATIC)
target_link_libraries(descent-xmlstatic descent-xmlobj adtstatic)

target_include_directories(descent-xmlobj
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS descent-xml descent-xmlstatic
	DESTINATION lib)
install(FILES descent-xml.h
	DESTINATION include)
install(DIRECTORY descent-xml
	DESTINATION include)
