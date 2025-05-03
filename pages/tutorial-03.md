\page tutorial-03 Recursion

One approach to handling nested elements is using recursive calls to xmltree_parse() or xmltree_parse_cstr(). For example, you can write an `element_handler` function for a parent element, and an `element_handler` for child elements, and have the parent `element_handler` call xmltree_parse_cstr() with the child `element_handler`. Alternatively, if you need to perform the same operation on every element, you can write one `element_handler` which calls xmltree_parse_cstr() on child elements, passing itself.

# Parent/Child Example - Print Authors of Fictional Works

The following example demonstrates a program which goes through an XML document, containing a `library` of `book`s, some of which are fiction and some of which are non-fiction. If the `book` has `type="fiction"`, it calls xmltree_parse_cstr() on a child handler, which then looks for an `author` element and prints its contents.

\include books-example.c


