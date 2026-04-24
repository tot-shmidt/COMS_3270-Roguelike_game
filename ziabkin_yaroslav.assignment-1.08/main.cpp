#include "terrain_generator.h"
#include "characters_players.h"
#include "queue.h"
#include "moves_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>  // For usleep
#include <ncurses.h>

// For 1.07
#include <iostream>
#include <fstream>
#include <string>
#include "database.h"
#include "pokemon.h"

// Setup ncurses
void init_ncurses() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    // Create color pairs like in Prof. Sheafeer's code.
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(10, COLOR_WHITE, COLOR_RED);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
}

int parse_command_line_args(char **file_name, int argc, char *argv[], int *npc_num) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0) {
            if (i + 1 < argc) {
                *npc_num = atoi(argv[i+1]);
                break;
            }
        
    // 1) Adding arguments from 1.07: pokemon, moves, pokemon moves,pokemon species, experience, type names, pokemon stats, stats or pokemon types
        } else if ((strcmp(argv[i], "pokemon") == 0) || (strcmp(argv[i], "moves") == 0) || (strcmp(argv[i], "pokemon_moves") == 0) ||
                   (strcmp(argv[i], "pokemon_species") == 0) || (strcmp(argv[i], "experience") == 0) || (strcmp(argv[i], "type_names") == 0) ||
                   (strcmp(argv[i], "pokemon_stats") == 0) || (strcmp(argv[i], "stats")== 0) || (strcmp(argv[i], "pokemon_types") == 0)) {

                    // Save the actual file name. Not a path yet.
                    *file_name = argv[i];
                    break;
        }
    } 

    return 0;
}

// This one takes PC as a parameter, so can print a map with it
void display_map_with_pc(map *current_map, struct world *this_world, Player *pc) {
    int i, j;

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            
            // 1. Check for the PC
            if (pc->x == j && pc->y == i) {
                mvaddch(i + 1, j, PC | COLOR_PAIR(10));
            
            // 2. Check for NPCs
            } else if (current_map->entity_map[i][j] != NULL) {
                mvaddch(i + 1, j, current_map->entity_map[i][j]->type | COLOR_PAIR(COLOR_WHITE));
            
            // 3. Otherwise, draw the colored terrain
            } else {
                char terrain_char = current_map->grid_array[i][j];
                int color_to_use;

                // Decide which color pair to use based on the character
                switch(terrain_char) {
                    case BOULDER:
                    case '%':
                        terrain_char = '%'; 
                        color_to_use = COLOR_MAGENTA;
                        break;
                    case '^': color_to_use = COLOR_GREEN; break;
                    case '#': color_to_use = COLOR_YELLOW; break;
                    case ':':
                    case '.':
                        color_to_use = COLOR_GREEN; break;
                    case '~': color_to_use = COLOR_BLUE; break;
                    case 'C': 
                    case 'M': color_to_use = COLOR_CYAN; break;
                    default:  color_to_use = COLOR_CYAN; break;
                }

                // Print the character with its assigned color!
                mvaddch(i + 1, j, terrain_char | COLOR_PAIR(color_to_use));
            }
        }
    }

    // White default color for text
    mvprintw(0, 0, "Coordinates of this map: (%d, %d)", this_world->current_map_x - 200, this_world->current_map_y - 200);

    refresh();
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

// 0. Parse command line arguments, read database in memory.
    int npc_num = 10;

    // Redundant from 1.07
    char *file_name = NULL;
    parse_command_line_args(&file_name, argc, argv, &npc_num);
        
    // Read database in memory
    struct Database database;
    read_db_in_memory(&database);

// 1. Create world and first map.
    struct world this_world;
    map *current_map = (map *) malloc(sizeof(map));
    initialize_world(&this_world, current_map);
    create_map(current_map, &this_world, npc_num);

// 2. Create PC
    Player pc;
    initialize_pc(&pc);
    place_pc_on_road(&pc, current_map);

// 3. Insert PC
    struct queue_node pc_node;
    pc_node.current_time = 0;
    pc_node.entity = &pc;
    mq_insert_node(&(current_map->queue), pc_node);
    
    // Find initial dstance maps
    find_distance_map(&pc, HIKER, current_map->hiker_dist_map, current_map);
    find_distance_map(&pc, RIVAL, current_map->rival_dist_map, current_map);

// 4. Initialize ncureses
    init_ncurses();

// 5. Game loop
    int quit_game = 0;

    // Suggest 3 pokemons for PC at the start of the game
    select_initial_pokemon(&pc, &database);

    while (!quit_game) {
        // Extract node with minimum current_time
        struct queue_node deq_node;
        mq_extract_min(&(current_map->queue), &deq_node);

        // Find if current entity PC or NPC
        enum entity_type deq_entity_type = deq_node.entity->type;

        // Do action based on entity type
        if (deq_entity_type == PC) {
            display_map_with_pc(current_map, &this_world, &pc);

            int successful_move = 0;

            while (!quit_game && !successful_move) {
                int input = getch();
                
                int new_x = pc.x;
                int new_y = pc.y;

                // Reset message
                move(0,0);
                clrtoeol();

                switch(input) {

                    // ~~~ Move inputs ~~~

                    // Standing still
                    case '5':
                    case ' ':
                    case '.':
                        successful_move = 1; break;
                    // North
                    case '8':
                    case 'k':
                        new_y--; break;
                    // North-East
                    case '9':
                    case 'u':
                        new_y--; new_x++; break;
                    // East
                    case '6':
                    case 'l':
                        new_x++; break;
                    // South-East
                    case '3':
                    case 'n':
                        new_y++; new_x++; break;
                    // South
                    case '2':
                    case 'j':
                        new_y++; break;
                    // South-West
                    case '1':
                    case 'b':
                        new_y++; new_x--; break;
                    // West
                    case '4':
                    case 'h':
                        new_x--; break;
                    // North-West
                    case '7':
                    case 'y':
                        new_y--; new_x--; break;
                    
                    // ~~~ Non-move inputs ~~~

                    // Quit the game
                    case 'q':
                    case 'Q':
                        quit_game = 1; successful_move = 1; break;

                    // Fly somewhere
                    case 'f': {
                        int fly_y, fly_x;
                        
                        mvprintw(0, 0, "Provide \"x y\" to fly: ");
                        refresh();

                        echo();
                        scanw("%d %d", &fly_x, &fly_y); // ncurses version of scanf
                        noecho();                    
                        
                        int next_map_x = 200 + fly_x;
                        int next_map_y = 200 + fly_y;

                        if (next_map_x < 0 || next_map_y < 0 || next_map_x >= WORLD_WIDTH || next_map_y >= WORLD_HEIGHT) {
                            mvprintw(0, 0, "Moving out off bounds of the world!\n");
                            refresh();
                        } else {
                            // Update coordinates of current map in the world. 
                            this_world.current_map_x = next_map_x;
                            this_world.current_map_y = next_map_y;

                            // If next map does not exist - build it.
                            if (this_world.world_maps[next_map_y][next_map_x] == NULL) {
                                this_world.world_maps[next_map_y][next_map_x] = (map *) malloc(sizeof(map));
                                
                                create_map(this_world.world_maps[next_map_y][next_map_x], &this_world, (rand() % 10) + 2); 
                            }
                            
                            // Aim pointer
                            current_map = this_world.world_maps[next_map_y][next_map_x];

                            // Synchronize time of PC and NPCs in the next map.
                            if (current_map->queue.current_size > 0) {
                                struct queue_node sync_node;

                                // Extract NPC on the next map
                                mq_extract_min(&(current_map->queue), &sync_node);
                                deq_node.current_time = sync_node.current_time;
                                mq_insert_node(&(current_map->queue), sync_node);
                            } else {
                                deq_node.current_time = 0;
                            }
                            
                            // Place pc on a new map.
                            place_pc_on_road(&pc, current_map);
                            
                            successful_move = 1;
                        }

                        break;
                    }

                    // Enter poke-building
                    case '>':
                        if (current_map->grid_array[pc.y][pc.x] == POK_CENTER || current_map->grid_array[pc.y][pc.x] == POK_MART) {
                            clear();
                            mvprintw(0, 0, "Inside %c building. Press '<' to exit.", current_map->grid_array[pc.y][pc.x]);
                            refresh();

                            // Loading bar variables
                            int i = 0;
                            char a[5] = "\\|/-";
                            int exit_building = 0;

                            nodelay(stdscr, TRUE);

                            while (!exit_building) {
                                usleep(125000);
                                mvaddch(1, i / 4, a[i%4]);
                                refresh();

                                // We are trying to empty the imput buffer here every outer while loop, as user can press several keys, like "asdf<"
                                while ((input = getch()) != ERR) {
                                    if (input == '<') {
                                        exit_building = 1;
                                    }
                                }

                                i++;

                                if (i / 4 >= MAP_WIDTH) {
                                    i = 0;
                                    move(1, 0);
                                    clrtoeol();
                                }
                            }

                            nodelay(stdscr, FALSE);

                            // Redraw the map
                            clear();
                            display_map_with_pc(current_map, &this_world, &pc);
                        } else {
                            mvprintw(0, 0, "You have to stand on a poke-building!");
                            refresh();
                        }
                        break;
                    // Tainer list
                    case 't': {
                        int exit_menu = 0;
                        // How many lines are reserved to print trainers
                        int rows_for_trainers = 13;
                        // How many times scroll down was pressed
                        int current_scroll = 0;
                        
                        while (!exit_menu) {
                            // Where I start printing trainers
                            int current_row = 5;
                            int column;
                            int row;

                            // Drawing blackbox
                            for (row = 3; row <= 19; row++) { // I have 17 rows.
                                for (column = 19; column <= 60; column++) {
                                    mvaddch(row, column, ' ');
                                }
                            }

                            // Placing header
                            mvprintw(4, 21, " ~ Trainers Locations ~");

                            // Loop through entities array
                            int i;
                            for (i = current_scroll; i < current_map->entity_num && i < current_scroll + rows_for_trainers; i++) {
                                Character *npc = current_map->entity_array[i];

                                int delta_x = pc.x - npc->x;
                                int delta_y = pc.y - npc->y;

                            
                                const char *n_s = ""; 
                                const char *e_w = "";

                                // North/South
                                if (delta_y > 0) {
                                    n_s = "North";
                                } else if (delta_y < 0) {
                                    n_s = "South";
                                }

                                // East/West
                                if (delta_x > 0) {
                                    e_w = "West"; 
                                } else if (delta_x < 0) {
                                    e_w = "East";
                                }

                                // Print correct string based
                                if (delta_y != 0 && delta_x != 0) {
                                    mvprintw(current_row, 21, "%c, %d %s and %d %s", npc->type, abs(delta_y), n_s, abs(delta_x), e_w);
                                } else if (delta_y != 0) {
                                    mvprintw(current_row, 21, "%c, %d %s", npc->type, abs(delta_y), n_s);
                                } else if (delta_x != 0) {
                                    mvprintw(current_row, 21, "%c, %d %s", npc->type, abs(delta_x), e_w);
                                }

                                current_row++;
                            }

                            mvprintw(18, 21, "Use UP/DOWN or ESC to exit");
                            refresh();

                            int menu_input = getch();
                            
                            switch(menu_input) {
                                case 27:
                                    exit_menu = 1;
                                    break;
                                case KEY_UP:
                                    if (current_scroll > 0) {
                                        current_scroll--;
                                    }
                                    break;
                                case KEY_DOWN:
                                    if (current_scroll + rows_for_trainers < current_map->entity_num) {
                                        current_scroll++;
                                    }
                                    break;
                            }
                        }

                        clear();
                        display_map_with_pc(current_map, &this_world, &pc);
                        break;

                    }
                    default:
                        mvprintw(0, 0, "No such command!");
                        refresh();
                        break;
                }

                // Check if move key was pressed.
                if (pc.x != new_x || pc.y != new_y) {
                    // get cost of new terrain for pc.
                    int terrain_cost = calculate_cost(PC, (enum terrain_types) current_map->grid_array[new_y][new_x]);

                    //  ~~~ Check if we move to a gate ~~~
                    enum directions gate_direction;
                    int next_map_y = this_world.current_map_y;
                    int next_map_x = this_world.current_map_x;

                    // if our next cell is a gate.
                    if ((new_x == MAP_WIDTH - 1 || new_x == 0 || new_y == 0 || new_y == MAP_HEIGHT - 1) && ((enum terrain_types) current_map->grid_array[new_y][new_x]) == ROAD) { 
                        // I have to find if we are going N,E,S or W.
                        if (new_x == MAP_WIDTH - 1) {
                            // Going East
                            gate_direction = EAST;
                            next_map_x++;
                        } else if (new_x == 0) {
                            // Going West
                            gate_direction = WEST;
                            next_map_x--;
                        } else if (new_y == 0) {
                            // Going North
                            gate_direction = NORTH;
                            next_map_y--;
                        } else if (new_y == MAP_HEIGHT - 1) {
                            // Going South
                            gate_direction = SOUTH;
                            next_map_y++;
                        } else {
                            mvprintw(0, 0, "Something went wrong with gates");
                            refresh();
                            break;
                        }

                        // Check if new map is not out of the bounds of the world.
                        if (next_map_x >= WORLD_WIDTH || next_map_x < 0 || next_map_y >= WORLD_HEIGHT || next_map_y < 0) {
                            mvprintw(0, 0, "Moving out off bounds of the world!\n");
                            refresh();
                        // If here, we are not going ouside of the world.
                        } else {
                            // Update coordinates of current map in the world. 
                            this_world.current_map_x = next_map_x;
                            this_world.current_map_y = next_map_y;

                            // Coordinates of PC before entering the gate
                            //int old_x = pc.x;
                            //int old_y = pc.y;

                            // If next map does not exist.
                            if (this_world.world_maps[next_map_y][next_map_x] == NULL) {

                                this_world.world_maps[next_map_y][next_map_x] = (map *) malloc(sizeof(map));
                                create_map(this_world.world_maps[next_map_y][next_map_x], &this_world, (rand() % 10) + 2);

                                // Make new map as our current map.
                                current_map = this_world.world_maps[next_map_y][next_map_x];

                                // Syncronize time of PC and NPS in the next map.
                                if (current_map->queue.current_size > 0) {
                                    struct queue_node sync_node;

                                    // Extract NPC on the next map
                                    mq_extract_min(&(current_map->queue), &sync_node);

                                    deq_node.current_time = sync_node.current_time;

                                    mq_insert_node(&(current_map->queue), sync_node);
                                } else {
                                    deq_node.current_time = 0;
                                }
                                
                                // Place pc on a new map.
                                if (gate_direction == NORTH) {
                                    pc.y = MAP_HEIGHT - 2;
                                } else if (gate_direction == SOUTH) {
                                    pc.y = 1;
                                } else if (gate_direction == EAST) {
                                    pc.x = 1;
                                } else if (gate_direction == WEST) {
                                    pc.x = MAP_WIDTH - 2;
                                } else {
                                    mvprintw(0, 0, "Can't assign new x, y for pc\n");
                                    refresh();
                                    break;
                                }

                            // Next map already exists. Just need to move there.
                            } else {
                                // Assign already existing map as the next one.
                                current_map = this_world.world_maps[next_map_y][next_map_x];

                                // Syncronize time of PC and NPS in the next map.
                                if (current_map->queue.current_size > 0) {
                                    struct queue_node sync_node;

                                    // Extract NPC on the next map
                                    mq_extract_min(&(current_map->queue), &sync_node);

                                    deq_node.current_time = sync_node.current_time;

                                    mq_insert_node(&(current_map->queue), sync_node);
                                } else {
                                    deq_node.current_time = 0;
                                }

                                // Place pc on a new map.
                                if (gate_direction == NORTH) {
                                    pc.y = MAP_HEIGHT - 2;
                                } else if (gate_direction == SOUTH) {
                                    pc.y = 1;
                                } else if (gate_direction == EAST) {
                                    pc.x = 1;
                                } else if (gate_direction == WEST) {
                                    pc.x = MAP_WIDTH - 2;
                                } else {
                                    mvprintw(0, 0, "Can't assign new x, y for pc\n");
                                    refresh();
                                    break;
                                }
                            }
                            
                            successful_move = 1;
                        }
                    }

                    // ~~~ Check we move to borders but not a gate.~~~
                    else if ((new_x == MAP_WIDTH - 1 || new_x == 0 || new_y == 0 || new_y == MAP_HEIGHT - 1)) {
                        mvprintw(0, 0, "Going out of bounds 777808");
                        refresh();
                    }
                    // Check if terrain is not passible
                    else if (terrain_cost == MY_INFINITY) {
                        mvprintw(0, 0, "You can't cross this impassible object.");
                        refresh();
                    }
                    // Check if we encountered a trainer
                    else if (current_map->entity_map[new_y][new_x] != NULL) {
                        NPC *bumped_npc = (NPC *) current_map->entity_map[new_y][new_x];
                        
                        // If not defeated - battle
                        if (bumped_npc->defeated == 0) {
                            mvprintw(0, 0, "Battle! Press 'Esc' to exit.");
                            refresh();

                            while (getch() != 27) {
                                ;
                            }
                            
                            bumped_npc->defeated = 1;
                            
                            clear();
                            display_map_with_pc(current_map, &this_world, &pc);
                        } else {
                            mvprintw(0, 0, "You have defeated this trainer!");
                            refresh();
                        }
                    // ----------------------------------------------------------------------------------------------------------
                    // Valid place to make a move.
                    // ----------------------------------------------------------------------------------------------------------
                    } else {
                        successful_move = 1;
                        pc.x = new_x;
                        pc.y = new_y;

                        find_distance_map(&pc, RIVAL, current_map->rival_dist_map, current_map);
                        find_distance_map(&pc, HIKER, current_map->hiker_dist_map, current_map);

                        // Pokemon encountering logic in TALL GRASS
                        if (current_map->grid_array[new_y][new_x] == TALL_GRASS) {
                            if (rand() % 100 < 10) {
                                // choose random pokemon id from the database by first selecting row, and then id from thar row.
                                int random_row = rand() % database.pokemon.size();
                                int id_from_row = database.pokemon[random_row].species_id;

                                int manh_distance = abs(this_world.current_map_x - 200) + abs(this_world.current_map_y - 200);
                                int lowest_level, highest_level;

                                if (manh_distance <= 200) {
                                    lowest_level = 1;
                                    highest_level = manh_distance / 2;

                                    if (highest_level == 0) {
                                        highest_level = 1;
                                    }
                                } else {
                                    lowest_level = (manh_distance - 200) / 2;
                                    highest_level = 100;
                                }

                                // Calculate new lavel in the given range
                                int new_level = lowest_level + (rand() % (highest_level - lowest_level + 1));

                                // Create pokemon
                                Pokemon new_poke(new_level, id_from_row, &database);

                                clear();
                                mvprintw(2, 5, "A wild pokemon %s appeared!", new_poke.identifier.c_str());
                                mvprintw(4, 5, "Level: %d", new_poke.current_level);
                                mvprintw(5, 5, "Shiny: %s", new_poke.shiny ? "Yes" : "No");
                                
                                mvprintw(7, 5, "Stats:");
                                mvprintw(8, 5, "HP: %d", new_poke.actual_stats[0]);
                                mvprintw(9, 5, "Attack: %d", new_poke.actual_stats[1]);
                                mvprintw(10, 5, "Defense: %d", new_poke.actual_stats[2]);
                                mvprintw(11, 5, "Speed: %d", new_poke.actual_stats[3]);
                                mvprintw(12, 5, "Sp. Atk: %d", new_poke.actual_stats[4]);
                                mvprintw(13, 5, "Sp. Def: %d", new_poke.actual_stats[5]);

                                mvprintw(15, 5, "Moves:");
                                for (size_t i = 0; i < new_poke.move_set.size(); i++) {
                                    mvprintw(16 + i, 5, "%d) %s",(int) i + 1, new_poke.move_set[i].c_str());
                                }

                                mvprintw(20, 5, "Press ESC to exit.");
                                refresh();

                                // Wait for the user to press ESC
                                while (getch() != 27) {
                                    ;
                                }

                                clear();
                                display_map_with_pc(current_map, &this_world, &pc);
                            }
                        }
                    }
                }
            }

            // Update time for PC node before enque it back
            int next_step_cost = calculate_cost(PC, (enum terrain_types) current_map->grid_array[pc.y][pc.x]);
            deq_node.current_time += next_step_cost;

            // Insert updated node back in the priority queue.
            mq_insert_node(&(current_map->queue), deq_node);
        
        // This is not PC
        } else {
            int step_cost = move_npc((NPC *) deq_node.entity, current_map, &pc);

            if (step_cost == -1) {
                mvprintw(0, 0, "Trainer got you! Press 'Esc' to exit");
                refresh();
                
                while (getch() != 27) {
                    ;
                }
                
                ((NPC *)deq_node.entity)->defeated = 1;
                step_cost = WAIT_COST;
                
                clear();
                display_map_with_pc(current_map, &this_world, &pc);
            }

            deq_node.current_time += step_cost;

            mq_insert_node(&(current_map->queue), deq_node);
        }
    }

// 6. Return terminal to the default settings
    endwin();

    return 0;
}