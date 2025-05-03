\page tutorial-02 Second Tutorial: A Simple Printer

The previous program worked, but wasn't very interesting. In this tutorial, we're going to use the C-string interface to write a parser that prints what it encounters.

This involves using \ref xmltree_parse_cstr, passing it an `element_handler` and a `text_handler`.

The signature of an element handler function is given in \ref xmltree_parse_element_cstr_fn, and the signature of a text handler is given in \ref xmltree_parse_text_cstr_fn. We're going to write an element handler that prints the element name and its attributes, and a text handler that prints the text content encountered.

Save the following file as `print-example.c` and compile it with `cc -o print-example print-example.c -lxmltreestatic`.

\include{lineno} print-example.c

Running this program will produce the following output:

```
$ ./print-example
element_name: ?xml
attribute: version=1.0
empty element (ends with /> or ?>): true
text node:

element_name: element
attribute: attr=val
attribute: attr2=
empty element (ends with /> or ?>): false
text node:
        Hello, world!

```

This example reveals a few things:
- \ref xmltree_parse_cstr doesn't do intelligent parsing: in XML, spaces between elements _are_ Text nodes containing whitespace, and this is reflected in XMLTree. Similarly, XMLTree doesn't convert entities for you, and it doesn't trim whitespace around Text nodes that also include non-whitespace content.
- The `attributes` array will always be a multiple of two, for each attribute=value pair, plus a null terminator. An attribute with an empty value will have an empty-string value.
- XMLTree does not provide a callback for closing tags, and doesn't call the element handler for closing tags. The next tutorial will cover checking for correctly-nested elements and closing tags.

XMLTree also doesn't perform structure validation: it is up to the `element_handler`s to validate structure, if at all.

Proceed to the next tutorial, \ref tutorial-03.
