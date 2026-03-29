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

int create_exit(struct map *new_map, enum directions direction) {
    if (direction == SOUTH) {
        new_map->south_exit = (rand() % 76) + 2;
        return 0;        
    } else if (direction == WEST) {
        new_map->west_exit = (rand() % 17) + 2;
        return 0;        
    } else if (direction == NORTH) {
        do {
            new_map->north_exit = (rand() % 76) + 2;
        } while (new_map->north_exit == new_map->south_exit);
        return 0;        
    } else if (direction == EAST) {
        do {
            new_map->east_exit = (rand() % 17) + 2;
        } while (new_map->east_exit == new_map->west_exit);
        return 0; // Since 0 is false, why do I return it here? Should it be 1, 25 or whatever non 0?
    } else {
        return -1; // What do I return if not successful?
    }
}

int connect_exits(struct map *new_map) {
    int i;

    // *** Connect left and right exits.***
    int go_vertical = (rand() % (MAP_WIDTH - 20)) + 10;

    // West exit
    for (i = 0; i <= go_vertical; i++) {
        new_map->grid_array[new_map->west_exit][i] = ROAD;
    }

    // East exit
    for (i = go_vertical; i < MAP_WIDTH; i++) {
        new_map->grid_array[new_map->east_exit][i] = ROAD;
    }

    // connect with vertical line
    int lower_exit, higher_exit;

    if (new_map->west_exit > new_map->east_exit) {
        lower_exit = new_map->west_exit; // Lower exit has greater value
        higher_exit = new_map->east_exit;// Higher exit has lower value
    } else {
        lower_exit = new_map->east_exit; // Lower exit has greater value
        higher_exit = new_map->west_exit;// Higher exit has lower value
    }

    for (i = higher_exit; i <= lower_exit; i++) {
        new_map->grid_array[i][go_vertical] = ROAD;
    }

    // *** Connect top and bottom exits.***
    int go_horizontal = (rand() % (MAP_HEIGHT - 5)) + 2;

    // North exit
    for (i = 0; i <= go_horizontal; i++) {
        new_map->grid_array[i][new_map->north_exit] = ROAD;
    }

    // South exit
    for (i = go_horizontal; i < MAP_HEIGHT; i++) {
        new_map->grid_array[i][new_map->south_exit] = ROAD;
    }

    // connect with horizonta line
    int rightmost_exit, lefmost_exit;

    if (new_map->north_exit > new_map-> south_exit) {
        rightmost_exit = new_map->north_exit;
        lefmost_exit = new_map->south_exit;
    } else {
        rightmost_exit = new_map->south_exit;
        lefmost_exit = new_map->north_exit;
    }

    for (i = lefmost_exit; i <= rightmost_exit; i++) {
        new_map->grid_array[go_horizontal][i] = ROAD;
    }

    // *** Place Pokemart (M) by the vertical line***
    int y_range = lower_exit - higher_exit + 1;
    int pokemart_y = (rand() % y_range) + higher_exit;

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
       

    // *** Place Pokecenter (C) by the horizontal line ***
    int x_range = rightmost_exit - lefmost_exit + 1;
    int pokecenter_x = (rand() % x_range) + lefmost_exit;

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

    return 0;
}

void create_map(struct map *new_map) {
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
    create_exit(new_map, SOUTH);
    create_exit(new_map, NORTH);
    create_exit(new_map, WEST);
    create_exit(new_map, EAST);

    // Now I have to somehow connect the roads. I will create a helper function for this.
    connect_exits(new_map);
}

