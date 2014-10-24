#include <stdlib.h>
#include <stdio.h>

#include "gadget_types.h"
#include "io.h"

void print_header(char *file_name)
{
    struct gadget_header_t *header = read_gadget_header(file_name);

    printf("%20s %g\n", "Mass:", header->mass[1]);

    free(header);
}

int main(int argc, char **argv)
{
    if (argc <= 1) {
        fprintf(stderr, "%s requires target file names as input.\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_header(argv[i]);
    }

    return 0;
}
