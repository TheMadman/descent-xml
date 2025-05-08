function(testcase target)
	add_executable(test_${target} ${target}.c)
	target_link_libraries(test_${target} descent_xml adt)
	add_test(NAME ${target} COMMAND test_${target})
endfunction()

testcase(descent_xml_classifier)
testcase(descent_xml_lex)
testcase(descent_xml_parse)
