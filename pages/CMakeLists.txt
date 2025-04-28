function(add_example name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} xmltreestatic)
endfunction()

add_example(noop-example)
add_example(print-example)
