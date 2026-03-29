#include "characters_players.h"
#include "terrain_generator.h"
#include "min_heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


int place_pc_on_road(struct entity *pc, struct map *map) {
    int latitude;   // row
    int j;          // column

    do {
        latitude = (rand() % (MAP_HEIGHT - 2)) + 1;

        for (j = 1; j <= MAP_WIDTH - 2; j++) {
            if (map->grid_array[latitude][j] == ROAD && map->entity_map[latitude][j] == NULL) {
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
        entity->pc = (struct pc_data*) malloc (sizeof (struct pc_data));
        
        // Temporary
        entity->pc->temp = 0;
        entity->npc = NULL;
    } else {
        // Temporary initialization for all PCs
        entity->npc = (struct npc_data*) malloc(sizeof (struct npc_data));
        entity->pc = NULL;
        entity->npc->temp = 0;
        entity->npc->direction = (enum directions) (rand() % 8);
        entity->npc->defeated = 0;

        entity->npc->direction = (enum directions) (rand() % 8); 
    }

    return 0;
}

/*  This function has to change position of NPC and return cost of that step.
 */
int move_npc(struct entity *entity, struct map *map, struct entity *pc) {
    // Freze if defeated
    if (entity->type != PC && entity->npc->defeated == 1) {
        return WAIT_COST; 
    }

    // Initial values to work with.
    enum entity_type entity_type = entity->type;

    // Current coordinates of the current NPC entity
    int current_x = entity->x;
    int current_y = entity->y;
    
    // Current coordinates changed by some delta in 8 possible directions.
    int check_x, check_y;

    // Coordinates of the neigbor with the minimum distance cost from entitie's position
    int new_x = current_x;
    int new_y = current_y;

    int min_cost = INFINITY;
    int neigbor_cost = INFINITY;

    // Two arrays forming possible move
    static const int dir_arr_y[8] = {1, 1, 0,-1,-1,-1, 0, 1};
    static const int dir_arr_x[8] = {0,-1,-1,-1, 0, 1, 1, 1};


    // Create moving logic.
    switch (entity_type) {
        case PC: {
            break;
        }
        case HIKER: {
            int i, j;
            // Two for loops to iterate around current cell. 
            for (i = -1; i < 2; i++) {
                for (j = -1; j < 2; j++) {
                    if (i == 0 && j == 0) {
                        continue;
                    }
                    
                    // Get new coordinates with current offset from for loops
                    check_x = current_x - j;
                    check_y = current_y - i;
    
                    // Check if we don't violate the bounds.
                    if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                        // Check if this new cell is not occupied.
                        if (map->entity_map[check_y][check_x] == NULL) {
                            // Get neigbor cost from the distance map
                            neigbor_cost = (*map).hiker_dist_map[check_y][check_x];
                            
                            // If current neighbor's cost is less then what we found at this point
                            if (neigbor_cost < min_cost) {
                                // Update tracking variables with minimum neighbor
                                min_cost = neigbor_cost;
                                new_x = check_x;
                                new_y = check_y;
                            }
                        }
                    }
                }
            }

            // Blocked by something case
            if (min_cost == INFINITY) {
                return WAIT_COST;
            // Found path case. Overwrite min_cost with the actual terrain cost, not value from dist_map.
            } else {
                enum terrain_types next_terrain = (enum terrain_types) map->grid_array[new_y][new_x];
                min_cost = calculate_cost(HIKER, next_terrain);
            }

            break;
        }

        case RIVAL:{
            int i, j;
            // Two for loops to iterate around current cell. 
            for (i = -1; i < 2; i++) {
                for (j = -1; j < 2; j++) {
                    if (i == 0 && j == 0) {
                        continue;
                    }
                    
                    // Get new coordinates with current offset from for loops
                    check_x = current_x - j;
                    check_y = current_y - i;
    
                    // Check if we don't violate the bounds.
                    if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                        // Check if this new cell is not occupied.
                        //*((*map).entity_map + check_y) - gives me a pointer to the first cell of a specific row
                        if (map->entity_map[check_y][check_x] == NULL) {
                            // Get neigbor cost from the distance map
                            neigbor_cost = (*map).rival_dist_map[check_y][check_x];
                            
                            // If current neighbor's cost is less then what we found at this point
                            if (neigbor_cost < min_cost) {
                                // Update tracking variables with minimum neighbor
                                min_cost = neigbor_cost;
                                new_x = check_x;
                                new_y = check_y;
                            }
                        }
                    }
                }
            }

            // Blocked by something case
            if (min_cost == INFINITY) {
                return WAIT_COST;
            // Found path case. Overwrite min_cost with the actual terrain cost, not value from dist_map.
            } else {
                enum terrain_types next_terrain = (enum terrain_types) map->grid_array[new_y][new_x];
                min_cost = calculate_cost(RIVAL, next_terrain);
            }

            break;
        }

        case PACER: {
            // Get current direction
            enum directions current_direction = (*((*entity).npc)).direction;

            // Get new cell with offset of current direction
            check_y = current_y + dir_arr_y[current_direction];
            check_x = current_x + dir_arr_x[current_direction];

            // ---
            // Moving forward
            if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                enum terrain_types next_terrain = (enum terrain_types) map->grid_array[check_y][check_x];

                // Find step cost for PACER to get to a new cell.
                neigbor_cost = calculate_cost(PACER, next_terrain);
                
                // If cell is not occupied and cost is not INFINITY - move. Else: turn around
                if  (*(*((*map).entity_map + check_y) + check_x) == NULL && neigbor_cost != INFINITY) {
                    new_x = check_x;
                    new_y = check_y;
                    min_cost = neigbor_cost;
                    break;
                }
            }

            // Turn around, forward is blocked.
            enum directions new_direction = (enum directions) ((current_direction + 4) % 8);
            check_y = current_y + dir_arr_y[new_direction];
            check_x = current_x + dir_arr_x[new_direction];

            if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                enum terrain_types prev_terrain = (enum terrain_types) map->grid_array[check_y][check_x];
                neigbor_cost = calculate_cost(PACER, prev_terrain);

                // If cell is empty and step cost is not INFINITY(reachable)
                if (map->entity_map[check_y][check_x] == NULL && neigbor_cost != INFINITY) {
                    // We can return to the previous cell.
                    entity->npc->direction = new_direction;
                    new_y = check_y;
                    new_x = check_x;
                    min_cost = neigbor_cost;
                } else {
                    return WAIT_COST;
                }

            } else {
                return WAIT_COST;
            }
            break;
        }

        case SWIMMER:
            return WAIT_COST;
            break;
        case WANDERER: {
            // get WANDERER's terrain and direction
            enum terrain_types first_terrain = (enum terrain_types) map->grid_array[current_y][current_x];
            enum directions current_direction = entity->npc->direction;
            
            // Get new cell with offset of current direction
            check_y = current_y + dir_arr_y[current_direction];
            check_x = current_x + dir_arr_x[current_direction];

            // Check forward
            if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                // Find terrain type of the next cell and its cost to move on.
                enum terrain_types next_terrain = (enum terrain_types) map->grid_array[check_y][check_x];
                neigbor_cost = calculate_cost(WANDERER, next_terrain);
                
                // Can move forward
                if (map->entity_map[check_y][check_x] == NULL && next_terrain == first_terrain && neigbor_cost != INFINITY) {
                    new_x = check_x;
                    new_y = check_y;
                    min_cost = neigbor_cost;
                    break;
                }
            }

            // 2. Go other direction. Forward is blocked
            int tries = 0;
            while (tries < 10) {
                // Find new direction and coordinates of that cell
                enum directions new_direction = (enum directions) (rand() % 8);
                check_y = current_y + dir_arr_y[new_direction];
                check_x = current_x + dir_arr_x[new_direction];

                // Check new direction
                if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                    // Check new terrain type
                    enum terrain_types next_terrain = (enum terrain_types) map->grid_array[check_y][check_x];
                    
                    // If true, we can move there.
                    if (next_terrain == first_terrain) {
                        entity->npc->direction = new_direction;

                        // If that cell is not occupied.
                        if (map->entity_map[check_y][check_x] == NULL) {
                            new_x = check_x;
                            new_y = check_y;
                            min_cost = calculate_cost(WANDERER, next_terrain);
                        
                            // Direction is valid, but cell is occupied. Just wait.  
                        } else {
                            return WAIT_COST;
                        }
                        break; 
                    }
                }
                tries++;
            }

            if (tries >= 10 || min_cost == INFINITY) {
                return WAIT_COST;
            }
            break;
        }
        case SENTRIES:
            return WAIT_COST;
            break;
        case EXPLORER: {
            enum directions current_direction = entity->npc->direction;

            check_y = current_y + dir_arr_y[current_direction];
            check_x = current_x + dir_arr_x[current_direction];

            // Check forward
            if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                enum terrain_types next_terrain = (enum terrain_types) map->grid_array[check_y][check_x];
                neigbor_cost = calculate_cost(EXPLORER, next_terrain);
                
                if (map->entity_map[check_y][check_x] == NULL && neigbor_cost != INFINITY) {
                    new_x = check_x;
                    new_y = check_y;
                    min_cost = neigbor_cost;
                    break;
                }
            }

            // No forward. Find new direction
            int tries = 0;
            while (tries < 10) {
                enum directions new_direction = (enum directions) (rand() % 8);
                check_y = current_y + dir_arr_y[new_direction];
                check_x = current_x + dir_arr_x[new_direction];
                
                // Check new direction
                if (check_x > 0 && check_x < MAP_WIDTH - 1 && check_y > 0 && check_y < MAP_HEIGHT - 1) {
                    enum terrain_types next_terrain = (enum terrain_types) map->grid_array[check_y][check_x];
                    neigbor_cost = calculate_cost(EXPLORER, next_terrain);
                    
                    if (neigbor_cost != INFINITY) {
                        entity->npc->direction = new_direction;

                        if (map->entity_map[check_y][check_x] == NULL) {
                            new_x = check_x;
                            new_y = check_y;
                            min_cost = neigbor_cost;
                        } else {
                            return WAIT_COST;
                        }
                        break;
                    }
                }
                tries++;
            }

            if (tries >= 10 || min_cost == INFINITY) {
                return WAIT_COST;
            }
            break;
        }
    }

    // After switch statement.
    if (new_x == pc->x && new_y == pc->y) {
        return -1;
    }

    // Erase this entity from the entity map
    map->entity_map[current_y][current_x] = NULL;

    // Assign new place to this entity
    map->entity_map[new_y][new_x] = entity;

    // Assign new coordinates to this entity
    entity->x = new_x;
    entity->y = new_y;

    // Return calculacted min_cost.
    return min_cost;
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
    } else if (e_type == PACER || e_type == WANDERER || e_type == EXPLORER || e_type == SENTRIES) {
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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y - 1][this_x]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y][this_x + 1]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y + 1][this_x]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y][this_x - 1]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y - 1][this_x + 1]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y + 1][this_x + 1]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y + 1][this_x - 1]);

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
            neighbor_cost = calculate_cost(npc_type, (enum terrain_types) map->grid_array[this_y - 1][this_x - 1]);

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