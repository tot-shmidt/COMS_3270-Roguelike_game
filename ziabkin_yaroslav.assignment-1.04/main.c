#include "terrain_generator.h"
#include "characters_players.h"
#include "queue.h"
#include "moves_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>  // For usleep

// This does not accout for PC
void display_map(struct map *current_map, struct world *this_world) {
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (current_map->grid_array[i][j] == BOULDER) {
                putchar('%');
            } else {
                printf("%c", current_map->grid_array[i][j]);
            }
        }
        printf("\n");
    }

    printf("Current coordinates: (%d, %d)\n", this_world->current_map_x - 200, this_world->current_map_y - 200);
}

// This one takes PC as a parameter, so can print a map with it
void display_map_with_pc(struct map *current_map, struct world *this_world, struct entity *pc) {
    int i, j;
    printf("\033[H\033[J");

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            // If true, we are at the cell where PC is currently located. Print it.
            if (pc->x == j && pc->y == i) {
                putchar(PC);
            // Print impassible BOULDERS
            } else if (current_map->grid_array[i][j] == BOULDER) {
                putchar('%');
            // Print NPC entity
            } else if (current_map->entity_map[i][j] != NULL) {
                putchar(current_map->entity_map[i][j]->type);
            // Print emtpty terrain.
            } else {
                printf("%c", current_map->grid_array[i][j]);
            }
        }
        printf("\n");
    }

    //printf("Current coordinates: (%d, %d)\n", this_world->current_map_x - 200, this_world->current_map_y - 200);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

// 0. Parse command line arguments
    int npc_num = 10;

    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0) {
            if (i + 1 < argc) {
                npc_num = atoi(argv[i+1]);
                break;
            }
        }
    }

// 1. Create world and first map.
    struct world this_world;
    struct map first_map;
    initialize_world(&this_world, &first_map);
    create_map(&first_map, &this_world, npc_num);

// 2. Create PC
    struct entity pc;
    initialize_entity(&pc, PC);
    place_pc_on_road(&pc, &first_map);

// 3. Initialize moves_queue and populate it with all PC and NPC
    struct moves_queue moves_queue;
    initialize_moves_queue(&moves_queue, npc_num + 1);

    // Insert PC
    struct queue_node pc_node;
    pc_node.current_time = 0;
    pc_node.entity = &pc;
    mq_insert_node(&moves_queue, pc_node);

    // Insert everybody else
    for (i = 0; i < first_map.entity_num; i++) {
        struct queue_node npc_node;
        npc_node.current_time = 0;
        npc_node.entity = first_map.entity_array + i;
        mq_insert_node(&moves_queue, npc_node);
    }
    
    // Find initial dstance maps
    find_distance_map(&pc, HIKER, first_map.hiker_dist_map, &first_map);
    find_distance_map(&pc, RIVAL, first_map.rival_dist_map, &first_map);

// 4. Game loop
    while (1) {
        // Extract node with minimum current_time
        struct queue_node deq_node;
        mq_extract_min(&moves_queue, &deq_node);

        // Find if current current entity PC or NPC
        enum entity_type deq_entity_type = deq_node.entity->type;

        // Do action based on entity type
        if (deq_entity_type == PC) {
            // Now PC is just staying still. Draw the map.
            display_map_with_pc(&first_map, &this_world, &pc);

            // Pause so, we can see updates not too fast.
            usleep(250000);

            // Add WAIT_TIME fee.
            deq_node.current_time += WAIT_COST;

            // Insert updated node back in the priority queue.
            mq_insert_node(&moves_queue, deq_node); 
        } else {
            // Get step cost of this NPC and make a move.
            int step_cost = move_npc(deq_node.entity, &first_map);

            // Update current_time in this queue_node.
            deq_node.current_time += step_cost;

            // Insert updated node back in the priority queue.
            mq_insert_node(&moves_queue, deq_node);
        }
    }

    return 0;


    // 4. Display the map
    // display_map_with_pc(&first_map, &this_world, &pc);
    
    // 4. Call path_finder for Hiker and Rival
    // int hiker_dist_map[MAP_HEIGHT][MAP_WIDTH];
    // find_distance_map(&pc, HIKER, hiker_dist_map, &first_map);

    //int rival_dist_map[MAP_HEIGHT][MAP_WIDTH];
    //find_distance_map(&pc, RIVAL, rival_dist_map, &first_map);

    // 5. Print maps for hiker and rival
    // print_dist_map(HIKER, hiker_dist_map);
    // print_dist_map(RIVAL, rival_dist_map);



    /*
    // 2. Game loop
    int game_continues = 1;

    do {
        // a. Take input from the user.
        printf("Enter command: ");

        char command;
        scanf(" %c", &command);

        // b. Validate input, calculate new place.
        int next_map_y = this_world.current_map_y;
        int next_map_x = this_world.current_map_x;
        int fly_y, fly_x;

        switch (command) {
            case 'n':
                next_map_y = this_world.current_map_y - 1; break;
            case 'e':
                next_map_x = this_world.current_map_x + 1; break;
            case 's':
                next_map_y = this_world.current_map_y + 1; break;
            case 'w':
                next_map_x = this_world.current_map_x - 1; break;
            case 'f':
                scanf(" %d %d", &fly_x, &fly_y);
                next_map_x = 200 + fly_x;
                next_map_y = 200 + fly_y;
                break;
            case 'q':
                game_continues = 0;
                continue;
            default:
                printf("Uncorrect input!\n");
                continue;
        }

        if (next_map_x < 0 || next_map_y < 0 || next_map_x >= WORLD_WIDTH || next_map_y >= WORLD_HEIGHT) {
            printf("Moving out off bounds of the world!\n");
        } else {
            this_world.current_map_y = next_map_y;
            this_world.current_map_x = next_map_x;

            // If map does not exist.
            if (this_world.world_maps[next_map_y][next_map_x] == NULL) {
                this_world.world_maps[next_map_y][next_map_x] = malloc(sizeof(struct map));
                create_map(this_world.world_maps[next_map_y][next_map_x], &this_world);
            }

            display_map(this_world.world_maps[next_map_y][next_map_x], &this_world);
        }
    } while (game_continues);
    */
}