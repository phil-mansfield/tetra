#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "gadget_types.h"
#include "io.h"

void print_particle(struct gadget_particle_t *p)
{
    printf("%12"PRIu64" (%g, %g, %g) (%g, %g, %g)\n", p->id,
           p->pos[0], p->pos[1], p->pos[2],
           p->vel[0], p->vel[1], p->vel[2]);
}

void print_header(char *file_name)
{
    struct gadget_header_t *header = read_gadget_header(file_name);

    uint64_t total = ((((uint64_t)header->npart_total[0]) << 32) +
                      header->npart_total[1]);

    printf("%20s: %u/%"PRIu64"\n", "Particles", header->npart[1], total);
    printf("%20s: %g\n", "Mass", header->mass[1]);
    printf("%20s: %g\n", "Redshift", header->redshift);
    printf("%20s: %g\n", "Box Size", header->box_size);
    printf("%20s: %d\n", "Snapshot Files", header->num_files);

    struct gadget_particle_t *ps = read_gadget_particles(file_name);

    for (int i = 0; i < 20; i++) print_particle(ps + i);

    free(header);
    free(ps);
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
