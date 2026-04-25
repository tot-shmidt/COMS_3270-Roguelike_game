#include "terrain_generator.h"
#include "characters_players.h"
#include "database.h"
#include "queue.h"
#include <stdlib.h>

#define entity_type_total 8

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

int create_npcs(map *new_map, int npc_num, Database *database, int new_x, int new_y, struct world *this_world) {
    // 1. Allocate enough space for entity_array of map struct, to keep all entitiy pointers.
    new_map->entity_array = (NPC **) malloc(sizeof (NPC *) * npc_num);
    new_map->entity_num = npc_num;

    // 2. Initialize entity_map will NULL.
    int i,j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            new_map->entity_map[i][j] = NULL;
        }
    }

    // 3. Initialize NPCs.
    
    for (i = 0; i < npc_num; i++) {
        // Select type of this entity
        enum entity_type entity_type;

        if (i == 0) {
            entity_type = HIKER;
        } else if (i == 1) {
            entity_type = RIVAL;
        } else {
            int rand_type = rand() % 7 + 2;

            switch (rand_type) {
                case 2: entity_type = HIKER;break;
                case 3: entity_type = RIVAL;break;
                case 4: entity_type = PACER;break;
                case 5: entity_type = SWIMMER;break;
                case 6: entity_type = WANDERER;break;
                case 7: entity_type = SENTRIES;break;
                case 8: entity_type = EXPLORER;break;
            }
        }

        // Create NPC object in memory.
        new_map->entity_array[i] = new NPC();

        // Initialize this entity. Send the address of current memmory slot of entity in entity_array.
        initialize_npc(new_map->entity_array[i], entity_type);

        // 4. Find place for this new entity on the map->entity_map
        int new_x, new_y;

        do {
            // Random potential x and y coordinates. Offset of border + 1 step from each side.
            new_x = (rand() % (MAP_WIDTH - 4)) + 2;
            new_y = (rand() % (MAP_HEIGHT - 4)) + 2;

            // Check if this cell is valid for npc placement.
            int empty_cell;
            if (new_map->entity_map[new_y][new_x] == NULL) {
                empty_cell = 1;
            } else {
                empty_cell = 0;
            }

            // Check if x,y terrain type is sutable for current entity_type.
            int cost = calculate_cost(entity_type, (enum terrain_types) new_map->grid_array[new_y][new_x]);

            // If cell is empty and cost is not infinity - we found a spot.
            if (empty_cell && cost != MY_INFINITY) {
                break;
            }
        } while (1);

        // Set x,y coordinates in our entity_array for this specific entity.

        // A->B is 100% identical to (*A).B
        // A[i] is 100% identical to *(A + i).
        new_map->entity_array[i]->y = new_y;
        new_map->entity_array[i]->x = new_x;

        // Put this new entity into our 2D entity_map for collisions.
        // sugar: new_map->entity_map[new_y][new_x] = &(new_map->entity_array[i]);
        new_map->entity_map[new_y][new_x] = new_map->entity_array[i];

        // -------------------------------------------------------------------------------
        // From 1.08: give pokemons to new NPC
        // -------------------------------------------------------------------------------

        // Establish level of a new pokemon
        int distance = abs(this_world->current_map_x - 200) + abs(this_world->current_map_y - 200);
        int low_level, high_level;

        if (distance <= 200) {
            low_level = 1;
            high_level = distance / 2;
            
            if (high_level == 0) {
                high_level = 1;
            }
        } else {
            high_level = 100;
            low_level = (distance - 200) / 2;
            
            if (low_level == 0) {
                low_level = 50;
            }
        }
        int pokemon_level = (rand() % (high_level - low_level + 1)) + low_level;

        // Create first mandatory pokemon
        int random_row = rand() % database->pokemon.size();
        int random_id = database->pokemon[random_row].species_id;

        Pokemon new_pokemon(pokemon_level, random_id, database);
        new_map->entity_array[i]->available_pokemons.push_back(new_pokemon);

        // Take a change to create some extra pokemons
        while ((rand() % 100 < 60) && new_map->entity_array[i]->available_pokemons.size() < 6) {
            random_row = rand() % database->pokemon.size();
            random_id = database->pokemon[random_row].species_id;
            pokemon_level = (rand() % (high_level - low_level + 1)) + low_level;

            new_map->entity_array[i]->available_pokemons.push_back(Pokemon(pokemon_level, random_id, database));
        }
    }

    return 0;
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

void create_map(struct map *new_map, struct world *this_world, int npc_num, Database *database, int map_x, int map_y) {
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


    // 1.04: populate this map with NPCs.
    create_npcs(new_map, npc_num, database, map_x, map_y, this_world);

    // 1.06: initialize moves_queue
    initialize_moves_queue(&(new_map->queue), npc_num + 1);

    // Push all the newly generated local NPCs into the queue.
    for (i = 0; i < new_map->entity_num; i++) {
        struct queue_node npc_node;
        npc_node.current_time = 0;
        npc_node.entity = *(new_map->entity_array + i);
        mq_insert_node(&(new_map->queue), npc_node);
    }
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