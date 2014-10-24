#ifndef TETRA_SRC_GADGET_TYPES_H
#define TETRA_SRC_GADGET_TYPES_H

struct gadget_header_t {
    unsigned int npart[6]; /* npart[1] gives the number of particles */
    double mass[6]; /* mass[1] gives the particle mass */
    double time; 
    double redshift;
    int flag_sfr;
    int flag_feedback;
    unsigned int npart_total[6]; /* npart[1] gives the total number of
                                  * particles in the run. If this number
                                  * exceeds 2^32, the npartTotal[2] stores
                                  * the result of a division of the particle
                                  * number by 2^32, while npartTotal[1] holds
                                  * the remainder. */
    int flag_cooling; 
    int num_files; /* the number of snapshot files */
    double box_size; /* simulation box size (in code units) */
    double omega_0; /* matter density */
    double omega_lambda;
    double hubble_param; /* little 'h' */
    int flag_stellarage;
    int flag_metals;
    int hashtabsize;
    unsigned int npartTotalHighWord[6];
    char fill[60];
};

struct gadget_particle_t {
    float pos[3];
    float vel[3];
    long id;
    int flag;
};

#endif /* TETRA_SRC_GADGET_TYPES_H */
