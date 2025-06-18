#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>

#include <descent-xml.h>

typedef struct libadt_lptr ptr_t;
typedef struct libadt_const_lptr cptr_t;
#define cptr libadt_const_lptr
#define allocated libadt_const_lptr_allocated

typedef struct descent_xml_lex token_t;
#define init descent_xml_lex_init
#define valid descent_xml_validate_document

ptr_t map_file(const char *const path)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return (ptr_t) { 0 };

	size_t file_size = lseek(fd, 0, SEEK_END);
	return (ptr_t) {
		.buffer = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0),
		.size = sizeof(char),
		.length = file_size,
	};
}

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");
	if (argc < 2)
		return EXIT_FAILURE;

	argc--, argv++;
	for (; *argv; argv++) {
		ptr_t file = map_file(*argv);
		cptr_t ptr = cptr(file);

		if (!allocated(ptr))
			return EXIT_FAILURE;

		token_t token = init(ptr);
		if (!valid(token))
			return EXIT_FAILURE;
	}
}
