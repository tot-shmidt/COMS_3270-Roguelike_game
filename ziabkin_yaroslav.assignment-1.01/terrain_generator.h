#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 21

struct map {
    int south_exit;
    int west_exit;
    int north_exit;
    int east_exit;

    char grid_array[MAP_HEIGHT][MAP_WIDTH];
};

void create_map(struct map *new_map);

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