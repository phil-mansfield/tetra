#include "geom.h"

#include <stdint.h>

uint64_t compress_coords(uint64_t x, uint64_t y, uint64_t z,
                         uint64_t dx, uint64_t dy, uint64_t dz,
                         uint32_t grid_width)
{
    uint64_t new_x = (x + dx + grid_width) % grid_width;
    uint64_t new_y = (y + dy + grid_width) % grid_width;
    uint64_t new_z = (z + dz + grid_width) % grid_width;
    uint64_t id = new_x + new_y *grid_width + new_z * grid_width * grid_width;
    return id;
}

void tetra_corners(uint64_t id,
                   uint32_t grid_width,
                   uint32_t dir_flag,
                   uint64_t *out)
{
    (void)dir_flag;

    int x = id % grid_width;
    int y = id % (grid_width * grid_width) / grid_width;
    int z = id / (grid_width * grid_width);

    out[0] = compress_coords(x, y, z, 0, 0, 1, grid_width);
    out[1] = compress_coords(x, y, z, 0, 1, 0, grid_width);
    out[2] = compress_coords(x, y, z, 1, 0, 0, grid_width);
}
