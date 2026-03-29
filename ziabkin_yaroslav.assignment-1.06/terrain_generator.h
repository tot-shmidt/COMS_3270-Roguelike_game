#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

// Forward declaration of characters_players.h struct
struct entity;

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

    // This is new for npc managment:

    // Array to store all the NPCs pointers on the current map and total number of NPC.
    struct entity *entity_array; 
    int entity_num;

    // 2D array to store locations of all the NPCs(not PC)
    struct entity *entity_map[MAP_HEIGHT][MAP_WIDTH];

    // Hiker and Rival distance maps
    int hiker_dist_map[MAP_HEIGHT][MAP_WIDTH];
    int rival_dist_map[MAP_HEIGHT][MAP_WIDTH];
};

struct world {
    struct map *world_maps[WORLD_HEIGHT][WORLD_WIDTH]; // 401 x 401
    int current_map_y;
    int current_map_x;
    // Here I will have 2 distance maps(for Hiker and Huilo).
};

void create_map(struct map *new_map, struct world *this_world, int npc_num);
void initialize_world(struct world *new_world, struct map *first_map);

enum terrain_types {
    BOULDER = 'b',
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
    SOUTH = 0,
    SOUTH_WEST = 1,
    WEST = 2,
    NORTH_WEST = 3,
    NORTH = 4,
    NORTH_EAST = 5,
    EAST = 6,
    SOUTH_EAST = 7
};

#endif