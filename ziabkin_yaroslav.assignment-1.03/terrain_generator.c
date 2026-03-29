#include "terrain_generator.h"
#include "queue.h"
#include <stdlib.h>

void place_seeds(struct queue *q, struct map *new_map, int seeds_amount, char terrain) {
    int i;
    for (i = 0; i < seeds_amount; i++) {
        int random_x, random_y;

        do {
            random_x = (rand() % (MAP_WIDTH - 2)) + 1;
            random_y = (rand() % (MAP_HEIGHT - 2)) + 1;
        } while (new_map->grid_array[random_y][random_x] != ' ');

        new_map->grid_array[random_y][random_x] = terrain;

        queue_enqueue(q, random_x, random_y);
    }
}

int create_exit(struct map *new_map, enum directions direction, struct world *this_world) {
    // Current coordinates of the map under construction.
    int y = this_world->current_map_y;
    int x = this_world->current_map_x;

    if (direction == SOUTH) {
        if (y == WORLD_HEIGHT - 1) {
            new_map->south_exit = -1;
            return 0;
        }
        // Case when a map to the bottom already exists.
        if (y + 1 < WORLD_HEIGHT && this_world->world_maps[y + 1][x] != NULL) {
            new_map->south_exit = this_world->world_maps[y + 1][x]->north_exit;
        // No such map to the bottom. Randomise exit position.
        } else {
            new_map->south_exit = (rand() % 76) + 2;
        }
        return 0;

    } else if (direction == WEST) {
        if (x == 0) {
            new_map->west_exit = -1;
            return 0;
        }

        if (x - 1 >= 0 && this_world->world_maps[y][x - 1] != NULL) {
            new_map->west_exit = this_world->world_maps[y][x - 1]->east_exit;
        } else {
            new_map->west_exit = (rand() % 17) + 2;
        }
        return 0;
        
    } else if (direction == NORTH) {
        if (y == 0) {
            new_map->north_exit = -1;
            return 0;
        }

        if (y - 1 >= 0 && this_world->world_maps[y - 1][x] != NULL) {
            new_map->north_exit = this_world->world_maps[y - 1][x]->south_exit;
        } else {
            do {
                new_map->north_exit = (rand() % 76) + 2;
            } while (new_map->north_exit == new_map->south_exit);
        }
        return 0;

    } else if (direction == EAST) {
        if (x == WORLD_WIDTH - 1) {
            new_map->east_exit = -1;
            return 0;
        }

        if (x + 1 < WORLD_WIDTH && this_world->world_maps[y][x + 1] != NULL) {
            new_map->east_exit = this_world->world_maps[y][x + 1]->west_exit;
        } else {
            do {
                new_map->east_exit = (rand() % 17) + 2;
            } while (new_map->east_exit == new_map->west_exit);
        }
        return 0;
    } else {
        return -1; // if not successful?
    }
}

int connect_exits(struct map *new_map, struct world *this_world) {
    int i;
    int go_vertical = (rand() % (MAP_WIDTH - 20)) + 10;     // For west-east road connection
    int go_horizontal = (rand() % (MAP_HEIGHT - 5)) + 2;    // For north-south road connection 

    // ************************************
    // *** Connect west and east exits ***
    // ************************************

    // West exit horizontal leg
    if (new_map->west_exit != -1) {
        for (i = 0; i <= go_vertical; i++) {
            new_map->grid_array[new_map->west_exit][i] = ROAD;   // тут можно добавить логику, что бы первое было не дорога,
                                                                 // а ворота!
        }
    }
    // East exit horizontal leg
    if (new_map->east_exit != -1) {
        for (i = go_vertical; i < MAP_WIDTH; i++) {
            new_map->grid_array[new_map->east_exit][i] = ROAD;
        }
    }

    int y_minimum = go_horizontal;
    int y_maximum = go_horizontal;

    if (new_map->west_exit != -1) {
        if (new_map->west_exit < y_minimum) {
            y_minimum = new_map->west_exit;
        }
        if (new_map->west_exit > y_maximum) {
            y_maximum = new_map->west_exit;
        }
    }

    if (new_map->east_exit != -1) {
        if (new_map->east_exit < y_minimum) {
            y_minimum = new_map->east_exit;
        }
        if (new_map->east_exit > y_maximum) {
            y_maximum = new_map->east_exit;
        }
    }

    for (i = y_minimum; i <= y_maximum; i++) {
        new_map->grid_array[i][go_vertical] = ROAD; // '|' or ROAD --------------------
    }

    // *************************************
    // *** Connect north and south exits ***
    // ************************************* 

    // North exit
    if (new_map->north_exit != -1) {
        for (i = 0; i <= go_horizontal; i++) {
            new_map->grid_array[i][new_map->north_exit] = ROAD;
        }
    }

    // South exit
    if (new_map->south_exit != -1) {
        for (i = go_horizontal; i < MAP_HEIGHT; i++) {
            new_map->grid_array[i][new_map->south_exit] = ROAD;
        }
    }

    int x_minimum = go_vertical;
    int x_maximum = go_vertical;

    // Check north
    if (new_map->north_exit != -1) {
        if (new_map->north_exit < x_minimum) {
            x_minimum = new_map->north_exit;
        }
        if (new_map->north_exit > x_maximum) {
            x_maximum = new_map->north_exit;
        }
    }

    // Check south
    if (new_map->south_exit != -1) {
        if (new_map->south_exit < x_minimum) {
            x_minimum = new_map->south_exit;
        }
        if (new_map->south_exit > x_maximum) {
            x_maximum = new_map->south_exit;
        }
    }

    for (i = x_minimum; i <= x_maximum; i++) {
        new_map->grid_array[go_horizontal][i] = ROAD; //'-' or ROAD ---------
    }

    // *** Place Pokemart (M) by the vertical line***
    int manhattan_dist = abs(this_world->current_map_x - 200) + abs(this_world->current_map_y - 200);
    int chance;

    if (manhattan_dist > 200) {
        chance = 5;
    } else {
        chance = ((-45 * manhattan_dist) / 200 + 50);
    }
    if (manhattan_dist == 0) {
        chance = 100;
    }

    if (chance > (rand() % 100)) {
        int y_range = y_maximum - y_minimum + 1;
        int pokemart_y = (rand() % y_range) + y_minimum;
    
        // this checks to the left of vertical line
        if (new_map->grid_array[pokemart_y][go_vertical - 1] != ROAD) {
            new_map->grid_array[pokemart_y][go_vertical - 1] = POK_MART;
        // this cheks to the right of the verticalline    
        } else if (new_map->grid_array[pokemart_y][go_vertical + 1] != ROAD) {
            new_map->grid_array[pokemart_y][go_vertical + 1] = POK_MART;
            
        // Shift up by one row and place it on the Left
        } else {
            new_map->grid_array[pokemart_y - 1][go_vertical - 1] = POK_MART;
        }
    }

    // *** Place Pokecenter (C) by the horizontal line ***
    if (chance > (rand() % 100)) {
        int x_range = x_maximum - x_minimum + 1;
        int pokecenter_x = (rand() % x_range) + x_minimum;
    
        // go_horizonatal - 1 checks the space above the road.
        if (new_map->grid_array[go_horizontal - 1][pokecenter_x] != POK_MART && 
            new_map->grid_array[go_horizontal - 1][pokecenter_x] != ROAD) {
            new_map->grid_array[go_horizontal - 1][pokecenter_x] = POK_CENTER;
        } 
        // Check bellow (+1)
        else if (new_map->grid_array[go_horizontal + 1][pokecenter_x] != POK_MART && 
                new_map->grid_array[go_horizontal + 1][pokecenter_x] != ROAD) {
            
            new_map->grid_array[go_horizontal + 1][pokecenter_x] = POK_CENTER;
        }
        // If both blocked: shift left by one col and place Above
        else {
            new_map->grid_array[go_horizontal - 1][pokecenter_x - 1] = POK_CENTER;
        }
    }

    return 0;
}

void create_map(struct map *new_map, struct world *this_world) {
    struct queue q;
    queue_init(&q);

    // Fill everything with NOTHING(' ') as background.
    int i, j;

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            new_map->grid_array[i][j] = ' '; 
        }
    }

    // Create walls with BOULDER(%).
    for (i = 0; i < MAP_WIDTH; i++) {
        new_map->grid_array[0][i] = BOULDER;
        new_map->grid_array[MAP_HEIGHT - 1][i] = BOULDER;
    }

    for (i = 0; i < MAP_HEIGHT; i++) {
        new_map->grid_array[i][0] = BOULDER;
        new_map->grid_array[i][MAP_WIDTH - 1] = BOULDER;
    }

    // Seed terrain types.
    int clearing_regions = 2 + (rand() % 10);
    int water_regions = (rand() % 10) + 1;
    int tall_grass_regions = 2 + (rand() % 10);
    int forest_regions = (rand() % 10);
    int mountain_regions = (rand() % 7);

    place_seeds(&q, new_map, clearing_regions, CLEARING);
    place_seeds(&q, new_map, water_regions, WATER);
    place_seeds(&q, new_map, tall_grass_regions, TALL_GRASS);
    place_seeds(&q, new_map, forest_regions, TREE);
    place_seeds(&q, new_map, mountain_regions, MOUNTAINS);

    // Grow seeds.
    while (queue_has_items(&q)) {
        int current_x, current_y;
        queue_dequeue(&q, &current_x, &current_y);

        char current_type = new_map->grid_array[current_y][current_x];

        // Check left
        if (current_x - 1 > 0 && new_map->grid_array[current_y][current_x - 1] == ' ') {
            new_map->grid_array[current_y][current_x - 1] = current_type;
            queue_enqueue(&q, current_x - 1, current_y);
        }

        // Check right
        if (current_x + 1 < MAP_WIDTH - 1 && new_map->grid_array[current_y][current_x + 1] == ' ') {
            new_map->grid_array[current_y][current_x + 1] = current_type;
            queue_enqueue(&q, current_x + 1, current_y);
        }
        
        // CHeck up
        if (current_y - 1 > 0 && new_map->grid_array[current_y - 1][current_x] == ' ') {
            new_map->grid_array[current_y - 1][current_x] = current_type;
            queue_enqueue(&q, current_x, current_y - 1);
        }   

        // Check down
        if (current_y + 1 < MAP_HEIGHT - 1 && new_map->grid_array[current_y + 1][current_x] == ' ') {
            new_map->grid_array[current_y + 1][current_x] = current_type;
            queue_enqueue(&q, current_x, current_y + 1);
        }
    }

    // Clean leftover empty spots.
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (new_map->grid_array[y][x] == ' ') {
                new_map->grid_array[y][x] = CLEARING;
            }
        }
    }

    // Create 4 exits.
    create_exit(new_map, SOUTH, this_world);
    create_exit(new_map, NORTH, this_world);
    create_exit(new_map, WEST, this_world);
    create_exit(new_map, EAST, this_world);

    // Now I have to somehow connect the roads. I will create a helper function for this.
    connect_exits(new_map, this_world);
}

void initialize_world(struct world *new_world, struct map *first_map) {
    int i, j;
    for (i = 0; i < WORLD_HEIGHT; i++) {
        for (j = 0; j < WORLD_WIDTH; j++) {
            new_world->world_maps[i][j] = NULL;
        }
    }

    new_world->world_maps[200][200] = first_map;

    new_world->current_map_y = 200;
    new_world->current_map_x = 200;
}