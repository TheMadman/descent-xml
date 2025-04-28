\page tutorial-01 First Tutorial: Make it Run

This tutorial assumes you already have `xmltree` and `libadt` set up in your environment.

This program will be the basis for future tutorials.

It will:

- initialize a token from an XML string
- iterate the token from the beginning of the XML to the end
- exit the program

Save it to a file called `noop-example.c` and compile with `cc -o noop-example noop-example.c -lxmltreestatic`.

\include{lineno} noop-example.c

The program includes a pretty long preamble, which just removes namespacing noise from `libadt` and `xmltree` functions and data types.

Otherwise, we've defined a few functions for convenience:

- `is_end_type` checks if the token given to it reflects an end-of-file
- `is_error_type` checks if the token given is of one of two error types:
  - `xmltree_classifier_unexpected` means the lexer encountered a character in an unexpected location (for example, an end-of-file inside an element started with `<`)
  - `xmltree_parse_error` means that the parser encountered an error unrelated to the document. For example, the C-string parsing interface allocates memory to copy values, which can fail.

Once you've gotten the program to compile and run, you can progress on to \ref tutorial-02.
