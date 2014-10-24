#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "debug.h"
#include "gadget_types.h"

struct gadget_header_t *read_gadget_header(char *file_name)
{
    struct gadget_header_t *header = malloc(sizeof(*header));
    check_mem(header);

    FILE *fp;
    if (!(fp = fopen(file_name, "r"))) {
        fprintf(stderr, "Cannot open %s\n", file_name);
        exit(1);
    }
    
    if (!fread(&header, sizeof(*header), 1, fp)) {
        fprintf(stderr, "Problem reading header of %s.\n", file_name);
    }

    return header;
}
