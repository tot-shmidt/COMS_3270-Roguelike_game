#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401

class Character;
class Player;
class NPC;


class map {
    public:
        int south_exit;
        int west_exit;
        int north_exit;
        int east_exit;

        // This holds entire map of terrain chars.
        char grid_array[MAP_HEIGHT][MAP_WIDTH];

        // ~~~ NPC/PC managment ~~~

        // Array to store all the NPCs pointers on the current map and total number of NPC.
        Character* entity_array;
        int entity_num;

        // 2D array to store locations of all the NPCs(not PC)
        Character* entity_map[MAP_HEIGHT][MAP_WIDTH];

        // Hiker and Rival distance maps
        int hiker_dist_map[MAP_HEIGHT][MAP_WIDTH];
        int rival_dist_map[MAP_HEIGHT][MAP_WIDTH];
};


struct world {
    map *world_maps[WORLD_HEIGHT][WORLD_WIDTH]; // 401 x 401
    int current_map_y;
    int current_map_x;
};

void create_map(map *new_map, struct world *this_world, int npc_num);
void initialize_world(struct world *new_world, map *first_map);

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