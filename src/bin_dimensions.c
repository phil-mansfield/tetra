#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include "debug.h"
#include "gadget_types.h"
#include "io.h"
#include "string.h"
#include "geom.h"

#define MIN(x, y) ((x) < (y)? (x): (y))
#define MAX(x, y) ((x) > (y)? (x): (y))

int id_cmp (const void * a, const void * b)
{
    return (((struct gadget_particle_t*)a)->id - 
            ((struct gadget_particle_t*)b)->id);
}

int bin_idx(float x, float width, float min_x)
{
    return (int) ((x - min_x) / width);
}

void bin_data(float *xs, int len, int bins,
              float min_x, float max_x,
              float *centers, int *counts)
{
    float width = (max_x - min_x) / bins;
    for (int i = 0; i < len; i++) {
        int idx = bin_idx(xs[i], width, min_x);
        counts[MIN(idx, bins - 1)]++;
    }

    for (int i = 0; i < bins; i++) {
        centers[i] = 0.5 + width * i;
    }
}

float dist(float *xs1, float *xs2)
{
    float sum = 0.0;
    for (int i =0; i < 3; i++) {
        float diff = xs1[i] - xs2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

int main(int argc, char **argv)
{
    check(argc == 2, "%s requires input file.", argv[1]);

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

    printf("# Approximate cell width: %g Mpc\n", max_x - min_x);

    struct gadget_particle_t *sorted_ps = malloc(sizeof(*sorted_ps) * len);
    check_mem(sorted_ps);
    memcpy(sorted_ps, ps, len * sizeof(*sorted_ps));

    qsort(sorted_ps, len, sizeof(*ps), id_cmp);

    int found = 0;
    struct gadget_particle_t *key = calloc(sizeof(*key), 1);
    struct gadget_particle_t *res;

    float *dists = malloc(sizeof(*dists) * 3 * len);

    for (uint32_t i = 0; i < len; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            key->id = corners[3 * i + j];         
            if ((res = bsearch(key,sorted_ps,len,sizeof(*sorted_ps),id_cmp))) {
                float d = dist(ps[i].pos, res->pos);
                max_x = MAX(max_x, d);
                min_x = MIN(min_x, d);

                dists[found] = log10(d);

                found++;
            }
        }
    }

    printf("# Neighbor Fraciton in Box: %g\n", ((float)found) / len * 3);
    printf("# Max distance: %g\n.", max_x);
    printf("# Min distance: %g\n.", min_x);
    printf("#%15s %15s\n", "log10(d)", "n");

    int bins = 400;
    float *centers = calloc(bins, sizeof(*centers));
    int *counts = calloc(bins, sizeof(*counts));

    bin_data(dists, len, bins, min_x, max_x, centers, counts);

    for (int i = 0; i < bins; i++) {
        printf("  %15g %15d\n", centers[i], counts[i]);
    }

    free(key);
    free(corners);
    free(header);
    free(ps);

    return 0;
}
