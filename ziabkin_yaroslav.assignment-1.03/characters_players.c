#include "characters_players.h"
#include "terrain_generator.h"
#include "min_heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define INFINITY 1000000

int place_pc_on_road(struct entity *pc, struct map *map) {
    int latitude;   // row
    int j;          // column

    do {
        latitude = (rand() % (MAP_HEIGHT - 2)) + 1;

        for (j = 1; j <= MAP_WIDTH - 2; j++) {
            if (map->grid_array[latitude][j] == ROAD) {
                pc->x = j;
                pc->y = latitude;

                break;
            } 
        }
    } while (pc->x == -1 || pc->y == -1);
    
    return 0;
}

void print_dist_map(enum entity_type npc_type, int distance_map[MAP_HEIGHT][MAP_WIDTH]) {
    int i, j;

    if (npc_type == RIVAL) {
        printf("Rival distance map:\n");
    } 
    if (npc_type == HIKER) {
        printf("Hiker distance map:\n");   
    } 

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (distance_map[i][j] == INFINITY) {
                printf("-- ");
            } else {
                printf("%02d ", distance_map[i][j] % 100);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int initialize_entity(struct entity *entity, enum entity_type type) {
    entity->x = -1;
    entity->y = -1;

    entity->type = type;
    
    if (type == PC) {
        entity->pc = malloc (sizeof (struct pc_data));
        
        // Temporary
        entity->pc->temp = 0;
        entity->npc = NULL;
    } else {
        // Temporary initialization for all PCs
        entity->npc = malloc(sizeof (struct npc_data));
        entity->pc = NULL;
        entity->npc->temp = 0;
    }

    return 0;
}

int calculate_cost(enum entity_type e_type, enum terrain_types t_type) {
    if (e_type == PC) {
        switch (t_type) {
            case BOULDER:       return INFINITY;
            case MOUNTAINS:     return INFINITY;
            case TREE:          return INFINITY;
            case ROAD:          return 10;
            case POK_CENTER:    return 10;
            case POK_MART:      return 10;
            case TALL_GRASS:    return 20;
            case CLEARING:      return 10;
            case WATER:         return INFINITY;
        }
    } else if (e_type == HIKER) {
        switch (t_type) {
            case BOULDER:       return INFINITY;
            case MOUNTAINS:     return 15;
            case TREE:          return 15;
            case ROAD:          return 10;
            case POK_CENTER:    return 50;
            case POK_MART:      return 50;
            case TALL_GRASS:    return 15;
            case CLEARING:      return 10;
            case WATER:         return INFINITY;
        }
    } else if (e_type == RIVAL) {
        switch (t_type) {
            case BOULDER:       return INFINITY;
            case MOUNTAINS:     return INFINITY;
            case TREE:          return INFINITY;
            case ROAD:          return 10;
            case POK_CENTER:    return 50;
            case POK_MART:      return 50;
            case TALL_GRASS:    return 20;
            case CLEARING:      return 10;
            case WATER:         return INFINITY;
        }
    } else if (e_type == SWIMMER) {
        switch (t_type) {
            case BOULDER:       return INFINITY;
            case MOUNTAINS:     return INFINITY;
            case TREE:          return INFINITY;
            case ROAD:          return INFINITY;
            case POK_CENTER:    return INFINITY;
            case POK_MART:      return INFINITY;
            case TALL_GRASS:    return INFINITY;
            case CLEARING:      return INFINITY;
            case WATER:         return 7;
        }
    } else if (e_type == OTHER) {
        switch (t_type) {
            case BOULDER:       return INFINITY;
            case MOUNTAINS:     return INFINITY;
            case TREE:          return INFINITY;
            case ROAD:          return INFINITY;
            case POK_CENTER:    return INFINITY;
            case POK_MART:      return INFINITY;
            case TALL_GRASS:    return INFINITY;
            case CLEARING:      return INFINITY;
            case WATER:         return INFINITY;
        }
    } else {
        return INFINITY;
    }

    return 0;
}

void find_distance_map(struct entity *pc, enum entity_type npc_type, int distance[MAP_HEIGHT][MAP_WIDTH], struct map *map) {
    // Initialize priority queue.
    struct min_heap priority_queue;
    initialize_min_heap(&priority_queue);

    // Initialize distances to infinity.
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            distance[i][j] = INFINITY;
        }
    }

    // Put PC into the heap and in distance map
    struct heap_node pc_heap_node;
    pc_heap_node.x = pc->x;
    pc_heap_node.y = pc->y;
    pc_heap_node.path_cost = 0;

    distance[pc->y][pc->x] = 0; // Distance from PC to PC is 0.

    // We do lazy loding by not adding all 1680 heap_nodes during initialization.
    insert_node(&priority_queue, pc_heap_node);

    while (priority_queue.current_size > 0) {
        // Extract minimal node from priority queue and save its fields
        struct heap_node min_node;
        extract_min(&priority_queue, &min_node);

        int this_x = min_node.x;
        int this_y = min_node.y;
        int this_cost = min_node.path_cost;

        // Variables to find new path_cost
        int neighbor_cost;
        int new_path_cost;

        // 1) Check north neighbor.
        if (this_y - 1 >= 0) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y - 1][this_x]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = this_cost + neighbor_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y - 1][this_x]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y - 1][this_x] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x;
                        new_heap_node.y = this_y - 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);
                        
                        // Update distance map
                        distance[this_y - 1][this_x] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y - 1][this_x] = new_path_cost;
                        decrease_priority(this_x, this_y - 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 2) Check east neighbor.
        if (this_x + 1 < MAP_WIDTH) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y][this_x + 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y][this_x + 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y][this_x + 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x + 1;
                        new_heap_node.y = this_y;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y][this_x + 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y][this_x + 1] = new_path_cost;
                        decrease_priority(this_x + 1, this_y, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 3) Check south neighbor.
        if (this_y + 1 < MAP_HEIGHT) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y + 1][this_x]);

            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                if (new_path_cost < distance[this_y + 1][this_x]) {
                    if (distance[this_y + 1][this_x] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x;
                        new_heap_node.y = this_y + 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y + 1][this_x] = new_path_cost;
                    } else {
                        distance[this_y + 1][this_x] = new_path_cost;
                        decrease_priority(this_x, this_y + 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 4) Check west neighbor.
        if (this_x - 1 >= 0) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y][this_x - 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y][this_x - 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y][this_x - 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x - 1;
                        new_heap_node.y = this_y;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y][this_x - 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y][this_x - 1] = new_path_cost;
                        decrease_priority(this_x - 1, this_y, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 5) Check north-east neighbor.
        if (this_x + 1 < MAP_WIDTH && this_y - 1 >= 0) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y - 1][this_x + 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y - 1][this_x + 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y - 1][this_x + 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x + 1;
                        new_heap_node.y = this_y - 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y - 1][this_x + 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y - 1][this_x + 1] = new_path_cost;
                        decrease_priority(this_x + 1, this_y - 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 6) Check south-east neighbor.
        if (this_x + 1 < MAP_WIDTH && this_y + 1 < MAP_HEIGHT) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y + 1][this_x + 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y + 1][this_x + 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y + 1][this_x + 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x + 1;
                        new_heap_node.y = this_y + 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y + 1][this_x + 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y + 1][this_x + 1] = new_path_cost;
                        decrease_priority(this_x + 1, this_y + 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 7) Check south-west neighbor.
        if (this_x - 1 >= 0 && this_y + 1 < MAP_HEIGHT) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y + 1][this_x - 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y + 1][this_x - 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y + 1][this_x - 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x - 1;
                        new_heap_node.y = this_y + 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y + 1][this_x - 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y + 1][this_x - 1] = new_path_cost;
                        decrease_priority(this_x - 1, this_y + 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }

        // 8) Check north-west neighbor.
        if (this_x - 1 >= 0 && this_y - 1 >= 0) {
            neighbor_cost = calculate_cost(npc_type, map->grid_array[this_y - 1][this_x - 1]);

            // We work with this cell only if it is reachable
            if (neighbor_cost != INFINITY) {
                new_path_cost = neighbor_cost + this_cost;

                // If we found a shorter path to this cell.
                if (new_path_cost < distance[this_y - 1][this_x - 1]) {
                    // If distance of neighbor is INFINITY, it is not in the priority_queue yet.
                    if (distance[this_y - 1][this_x - 1] == INFINITY) {
                        // Create a new heap_node for it and insert it.
                        struct heap_node new_heap_node;
                        new_heap_node.x = this_x - 1;
                        new_heap_node.y = this_y - 1;
                        new_heap_node.path_cost = new_path_cost;

                        insert_node(&priority_queue, new_heap_node);

                        // Update distance map
                        distance[this_y - 1][this_x - 1] = new_path_cost;
                    } else {
                        // This neighbor is already in queue, his distance is not NULL
                        distance[this_y - 1][this_x - 1] = new_path_cost;
                        decrease_priority(this_x - 1, this_y - 1, new_path_cost, &priority_queue);
                    }
                }
            }
        }
    }

    // Dealocate heap memory of the heap array.
    delete_heap(&priority_queue);
}