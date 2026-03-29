#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401

struct map {
    int south_exit;
    int west_exit;
    int north_exit;
    int east_exit;

    char grid_array[MAP_HEIGHT][MAP_WIDTH];
};

struct world {
    struct map *world_maps[WORLD_HEIGHT][WORLD_WIDTH]; // 401 x 401
    int current_map_y;
    int current_map_x;
};

void create_map(struct map *new_map, struct world *this_world);
void initialize_world(struct world *new_world, struct map *first_map);

enum terrain_types {
    BOULDER = '%',
    MOUNTAINS = '%',
    TREE = '^',
    ROAD = '#',
    POK_CENTER = 'C',
    POK_MART = 'M',
    TALL_GRASS = ':',
    CLEARING = '.',
    WATER = '~'
};

enum directions {
    SOUTH,
    WEST,
    NORTH,
    EAST
};

#endif