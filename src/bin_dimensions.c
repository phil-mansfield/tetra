#include <stdio.h>

#include "debug.h"
#include "gadget_types.h"
#include "io.h"
#include "geom.h"

int main(int argc, char **argv)
{
    check(argc != 2, "%s requires input file.", argv[1]);

    struct gadget_header_t *header = read_gadget_header(argv[1]);
    struct gadget_particle_t *ps = read_gadget_particles(argv[1]);

    uint32_t len = header->npart[1];
    uint64_t *corners = malloc(sizeof(*corners) * len * 3);

    for (uint32_t i = 0; i < len; i++) {
        tetra_corners(ps[i].id, 1024, 0, corners + 3 * i);
    }

    free(corners);
    free(header);
    free(ps);

    return 0;
}
