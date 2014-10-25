#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <float.h>

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

void float_min_max(float *xs, int len,
                   float *min, float *max)
{
    float min_x = FLT_MAX;
    float max_x = FLT_MIN;

    for (int i = 0; i < len; i++) {
        min_x = MIN(min_x, xs[i]);
        max_x = MAX(max_x, xs[i]);
    }

    *min = min_x;
    *max = max_x;
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

float *neighbor_distances(struct gadget_particle_t *ps,
                          struct gadget_particle_t *sorted_ps,
                          int32_t len, int *dist_len)
{
    uint64_t *corners = malloc(sizeof(*corners) * len * 3);

    for (int32_t i = 0; i < len; i++) {
        tetra_corners(ps[i].id, 1024, 0, corners + 3 * i);
    }
    
    struct gadget_particle_t *key = calloc(sizeof(*key), 1);
    struct gadget_particle_t *res;

    float *dists = malloc(sizeof(*dists) * 3 * len);
    int found = 0;

    for (int32_t i = 0; i < len; i++) {
        for (int32_t j = 0; j < 3; j++) {
            key->id = corners[j + i * 3];
            if ((res = bsearch(key, sorted_ps, len,
                               sizeof(*sorted_ps), id_cmp))) {

                key->id = corners[3 * i + j];         
                float d = dist(ps[i].pos, res->pos);

                dists[found] = d;
                found++;
            }
        }
    }

    *dist_len = found;

    free(key);
    free(corners);

    return dists;
}

int main(int argc, char **argv)
{
    check(argc == 2, "%s requires input file.", argv[1]);

    struct gadget_header_t *header = read_gadget_header(argv[1]);
    struct gadget_particle_t *ps = read_gadget_particles(argv[1]);

    uint32_t len = header->npart[1];

    struct gadget_particle_t *sorted_ps = malloc(sizeof(*sorted_ps) * len);
    check_mem(sorted_ps);
    memcpy(sorted_ps, ps, len * sizeof(*sorted_ps));
    qsort(sorted_ps, len, sizeof(*ps), id_cmp);

    int dist_len;
    float *dists = neighbor_distances(ps, sorted_ps, len, &dist_len);

    float max_dist, min_dist;
    float_min_max(dists, dist_len, &min_dist, &max_dist);

    printf("#%25s: %d / %d (%.2g percent)\n", "Neighobors in box",
           dist_len, len * 3, 100 * ((float) dist_len) / (3.0 * len));
    printf("#%25s: %g\n", "Maximum distance", max_dist);
    printf("#%25s: %g\n", "Minimum distance", min_dist);

    free(header);
    free(ps);
    free(sorted_ps);

    return 0;
}
