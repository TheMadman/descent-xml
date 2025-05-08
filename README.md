# Descent XML - An XML Parser-Helper Library

Descent XML provides a tokenizer and helper functions
for writing an XML parser.

## Building

### Dependencies

- `libadt` - https://github.com/TheMadman/libadt

### Configuration

Descent XML and libadt will both build with default compiler options, but they tag external functions with `DESCENT_XML_EXPORT` and `EXPORT` macros, respectively.

For debugging, defaults are fine:

```bash
mkdir build
cd build
# Add other cmake command options to taste, e.g.
# -DBUILD_TESTING=True -DBUILD_EXAMPLES=True
cmake ..
make install
```

For production shared objects, it is expected you configure your compiler to omit symbols not explicitly tagged with `DESCENT_XML_EXPORT` (and `EXPORT` for libadt):

```bash
# Example for GCC
CFLAGS+="-fvisibility=hidden -fvisibility-inline-hidden -DDESCENT_XML_EXPORT='"'__attribute__((visibility("default")))'"' -DEXPORT=DESCENT_XML_EXPORT"
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
```

Link with `-lxmltree -ladt`. For static linking, use `-lxmltreestatic -ladtstatic`.

# Documentation

Tutorials and reference documentation can be found at https://themadman.github.io/xmltree/. Documentation can be built using `doxygen`, which will generate a `html/index.html` that can be opened.

# Bugs/Shortcomings

- Currently, Descent XML just uses the application's encoding. It doesn't support reading the encoding provided in the XML and parsing it, separately from the application's `CTYPE` locale setting. This should be fixed in `lex.h`.
- There isn't an easy interface to parse partial XML, for example from a partially-filled buffer.
