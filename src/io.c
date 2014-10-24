#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

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

    int dummy;
    fread(&dummy, sizeof(dummy), 1, fp);
    
    if (!fread(header, sizeof(*header), 1, fp)) {
        fprintf(stderr, "Problem reading header of %s.\n", file_name);
    }

    fclose(fp);

    return header;
}


void wrap_pos(float *pos, struct gadget_header_t *header)
{
    for (int i = 0; i < 3; i++) pos[i] = fmod(pos[i], header->box_size);
}

void read_positions(FILE *fp,
                    struct gadget_header_t *header,
                    float *buf,
                    struct gadget_particle_t *particles)
{
    uint32_t dummy;
    fread(&dummy, sizeof(dummy), 1, fp);

    uint32_t len = header->npart[1];
    fread(buf, sizeof(*buf), 3 * len, fp);

    for (uint32_t i = 0; i < len; i++) {
        memcpy(particles[i].pos, buf + i * 3, sizeof(*buf) * 3);
        wrap_pos(particles[i].pos, header);
    }

    fread(&dummy, sizeof(dummy), 1, fp);
    return;
}

void convert_to_peculiar(float *vel, struct gadget_header_t *header)
{
    double roota = sqrt(header->time);
    for (int i = 0; i < 3; i++) vel[i] *= roota;
}


void read_velocities(FILE *fp,
                     struct gadget_header_t *header,
                     float *buf,
                     struct gadget_particle_t *particles)
{
    int dummy;
    fread(&dummy, sizeof(dummy), 1, fp);

    uint32_t len = header->npart[1];
    fread(buf, sizeof(*buf), 3 * len, fp);

    for (uint32_t i = 0; i < len; i++) {
        memcpy(particles[i].vel, buf + i * 3, sizeof(*buf) * 3);
        convert_to_peculiar(particles[i].vel, header);
    }

    fread(&dummy, sizeof(dummy), 1, fp);

    return;
}

void read_ids(FILE *fp,
              struct gadget_header_t *header,
              void *buf,
              struct gadget_particle_t *particles)
{
    uint32_t bytes;
    fread(&bytes, sizeof(bytes), 1, fp);

    uint32_t len = header->npart[1];
    
    if (len * 4 == bytes) {
        uint32_t *buf32 = (uint32_t *) buf;
        fread(buf32, sizeof(*buf32), len, fp);
        for (uint32_t i = 0; i < len; i++) particles[i].id = buf32[i];
    } else if (len * 8 == bytes) {
        uint64_t *buf64 = (uint64_t *) buf;     
        fread(buf64, sizeof(*buf64), len, fp);
        for (uint32_t i = 0; i < len; i++) particles[i].id = buf64[i];
    } else {
        check(0, "Invalid byte count %d for len %"PRIu32".\n", bytes, len);
    }
}

struct gadget_particle_t *read_gadget_particles(char *file_name)
{
    FILE *fp;
    if (!(fp = fopen(file_name, "r"))) {
        fprintf(stderr, "Cannot open %s\n", file_name);
        exit(1);
    }

    struct gadget_header_t *header = malloc(sizeof(*header));
    check_mem(header);

    int dummy;
    fread(&dummy, sizeof(dummy), 1, fp);
    fread(header, sizeof(*header), 1, fp);
    fread(&dummy, sizeof(dummy), 1, fp);

    uint32_t len = header->npart[1];

    struct gadget_particle_t *particles = malloc(sizeof(*particles) * len);
    void *buf = malloc(sizeof(float) * 3 * len);
    check_mem(buf);
    check_mem(particles);
    
    read_positions(fp, header, buf, particles);
    read_velocities(fp, header, buf, particles);
    read_ids(fp, header, buf, particles);

    free(header);
    free(buf);

    fclose(fp);

    return particles;
}
