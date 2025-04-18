# XMLTree - An XML Parser-Helper Library

XMLTree provides a tokenizer and helper functions
for writing an XML parser.

## Building

### Dependencies

- `libadt` - https://github.com/TheMadman/libadt

### Configuration

XMLTree and libadt will both build with default compiler options, but they tag external functions with `XMLTREE_EXTERNAL` and `EXTERNAL` macros, respectively.

For debugging, defaults are fine:

```bash
mkdir build
cd build
# Add other cmake command options to taste
# -DBUILD_TESTING=True
cmake ..
make install
```

For production shared objects, it is expected you configure your compiler to omit symbols not explicitly tagged with `XMLTREE_EXTERNAL` (and `EXTERNAL` for libadt):

```bash
# Example for GCC
CFLAGS+="-fvisibility=hidden -fvisibility-inline-hidden -DXMLTREE_EXPORT='"'__attribute__((visibility("default")))'"'"
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
```

Link with `-lxml -ladt`.
