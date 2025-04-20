function(testcase target)
	add_executable(test_${target} ${target}.c)
	target_link_libraries(test_${target} xmltree adt)
	add_test(NAME ${target} COMMAND test_${target})
endfunction()

testcase(xmltree_classifier)
testcase(xmltree_lex)
testcase(xmltree_parse)
