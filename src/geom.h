#ifndef TETRA_SRC_GEOM_H_
#define TETRA_SRC_GEOM_H_

#include <stdint.h>

void tetra_corners(uint64_t id,
                   uint32_t grid_width,
                   uint32_t dir_flag,
                   uint64_t *out);

#endif /* TETRA_SRC_GEOM_H_ */
