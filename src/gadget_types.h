#ifndef TETRA_SRC_GADGET_TYPES_H
#define TETRA_SRC_GADGET_TYPES_H


struct gadget_header_t {
    unsigned int npart[6]; /*!< npart[1] gives the number of particles in the present file, other particle types are ignored */
    double mass[6]; /*!< mass[1] gives the particle mass */
    double time; /*!< time (=cosmological scale factor) of snapshot */
    double redshift; /*!< redshift of snapshot */
    int flag_sfr; /*!< flags whether star formation is used (not available in L-Gadget2) */
    int flag_feedback; /*!< flags whether feedback from star formation is included */
    unsigned int npart_total[6]; /*!< npart[1] gives the total number of particles in the run. If this number exceeds 2^32, the npartTotal[2] stores
                                  the result of a division of the particle number by 2^32, while npartTotal[1] holds the remainder. */
    int flag_cooling; /*!< flags whether radiative cooling is included */
    int num_files; /*!< determines the number of files that are used for a snapshot */
    double box_size; /*!< Simulation box size (in code units) */
    double omega_0; /*!< matter density */
    double omega_lambda; /*!< vacuum energy density */
    double hubble_param; /*!< little 'h' */
    int flag_stellarage; /*!< flags whether the age of newly formed stars is recorded and saved */
    int flag_metals; /*!< flags whether metal enrichment is included */
    int hashtabsize; /*!< gives the size of the hashtable belonging to this snapshot file */
    unsigned int npartTotalHighWord[6]; /*!< High word of the total number of particles of each type */
    char fill[60];
};

struct gadget_particle_t {
    float pos[3];
    float vel[3];
    long id;
    int flag;
};

#endif /* TETRA_SRC_GADGET_TYPES_H */
