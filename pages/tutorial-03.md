\page tutorial-03 Third Tutorial: Recursion

One approach to handling nested elements is using recursive calls to descent_xml_parse() or descent_xml_parse_cstr(). For example, you can write an `element_handler` function for a parent element, and an `element_handler` for child elements, and have the parent `element_handler` call descent_xml_parse_cstr() with the child `element_handler`. Alternatively, if you need to perform the same operation on every element, you can write one `element_handler` which calls descent_xml_parse_cstr() on child elements, passing itself.

# Parent/Child Example - Print Authors of Fictional Works

The following example demonstrates a program which goes through an XML document, containing a `library` of `book`s, some of which are fiction and some of which are non-fiction. If the `book` has `type="fiction"`, it calls descent_xml_parse_cstr() on a child handler, which then looks for an `author` element and prints its contents.

\include books-example.c

This example should print the result:

```
Author: Raymond E. Feist
Author: Anthony Horowitz
```

In this example, each `element_handler` is responsible for its own closing tag. You will notice that the `element_handler`s each loop until they find a closing tag, then iterate the token once more with an empty call to descent_xml_parse_cstr(). If the element handler didn't iterate again, it would return _a_ closing tag token to the parent, which would then terminate the loop.

This parser still doesn't do structure validation: each element handler just assumes that the next closing tag, _any_ closing tag, is correct. You can test this by modifying one of the `</author>`s to an invalid value, such as `</date>`.

This is fine if you know the XML you're parsing will always be valid, but if the XML comes from a system you don't control, validation must be performed. The easiest way to validate a full document is with descent_xml_validate_document().
