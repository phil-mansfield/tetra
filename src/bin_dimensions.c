#include <stdio.h>

#include "debug.h"
#include "gadget_types.h"
#include "io.h"
#include "geom.h"

#define MIN(x, y) ((x) < (y)? (x): (y))
#define MAX(x, y) ((x) > (y)? (x): (y))

int id_cmp (const void * a, const void * b)
{
    return (((struct gadget_particle_t*)a)->id - 
            ((struct gadget_particle_t*)b)->id);
}

int main(int argc, char **argv)
{
    check(argc != 2, "%s requires input file.", argv[1]);

    struct gadget_header_t *header = read_gadget_header(argv[1]);
    struct gadget_particle_t *ps = read_gadget_particles(argv[1]);

    uint32_t len = header->npart[1];
    uint64_t *corners = malloc(sizeof(*corners) * len * 3);

    float min_x = header->box_size;
    float max_x = 0;

    for (uint32_t i = 0; i < len; i++) {
        tetra_corners(ps[i].id, 1024, 0, corners + 3 * i);
        max_x = MAX(max_x, ps[i].pos[0]);
        min_x = MIN(min_x, ps[i].pos[0]);
    }

    printf("# Approximate cell width: %g Mpc", max_x - min_x);

    qsort(ps, len, sizeof(*ps), id_cmp);

    free(corners);
    free(header);
    free(ps);

    return 0;
}
