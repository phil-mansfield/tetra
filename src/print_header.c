#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gadget_types.h"
#include "io.h"

void print_header(char *file_name)
{
    struct gadget_header_t *header = read_gadget_header(file_name);

    printf("%20s: %u/%llu\n", "Particles", header->npart[1],
           ((uint64_t*)header->npart_total)[0]);
    printf("%20s: %g\n", "Mass", header->mass[1]);
    printf("%20s: %g\n", "Redshift", header->redshift);
    printf("%20s: %g\n", "Box Size", header->box_size);
    printf("%20s: %g\n", "Omega M", header->omega_0);
    printf("%20s: %g\n", "Omega L", header->omega_lambda);
    printf("%20s: %g\n", "Hubble", header->hubble_param);
    printf("%20s: %d\n", "Snapshot Files", header->num_files);

    free(header);
}

int main(int argc, char **argv)
{
    if(argc <= 1) {
        fprintf(stderr, "%s requires input Gadget particle files.\n", argv[0]);
    }

    for (int i = 1; i < argc; i++) {
        print_header(argv[i]);
    }

    return 0;
}
