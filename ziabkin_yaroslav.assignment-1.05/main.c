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
}

// This one takes PC as a parameter, so can print a map with it
void display_map_with_pc(struct map *current_map, struct world *this_world, struct entity *pc) {
    int i, j;

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            // If true, we are at the cell where PC is currently located. Print it.
            if (pc->x == j && pc->y == i) {
                // I add 1 to i as we need to reserve frist row for other stuff
                mvaddch(i + 1, j, PC);
            // Print impassible BOULDERS. Internally boulder is 'b' but we draw it as mountains '%', so they look the same.
            } else if (current_map->grid_array[i][j] == BOULDER) {
                mvaddch(i + 1, j, '%');
            // Print NPC entities
            } else if (current_map->entity_map[i][j] != NULL) {
                mvaddch(i + 1, j, current_map->entity_map[i][j]->type);
            // Print emtpty terrain.
            } else {
                mvaddch(i + 1, j, current_map->grid_array[i][j]);
            }
        }
    }

    mvprintw(0, 0, "Coordinates of this map: (%d, %d)", this_world->current_map_x - 200, this_world->current_map_y - 200);

    // Move changes from ncurses buffer to the terminal screen.
    refresh();
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

// 4. Initialize ncureses
    init_ncurses();

// 5. Game loop
    int quit_game = 0;

    while (!quit_game) {
        // Extract node with minimum current_time
        struct queue_node deq_node;
        mq_extract_min(&moves_queue, &deq_node);

        // Find if current entity PC or NPC
        enum entity_type deq_entity_type = deq_node.entity->type;

        // Do action based on entity type
        if (deq_entity_type == PC) {
            display_map_with_pc(&first_map, &this_world, &pc);

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
                    // Enter poke-building
                    case '>':
                        if (first_map.grid_array[pc.y][pc.x] == POK_CENTER || first_map.grid_array[pc.y][pc.x] == POK_MART) {
                            clear();
                            mvprintw(0, 0, "Inside %c building. Press '<' to exit.", first_map.grid_array[pc.y][pc.x]);
                            refresh();

                            // Loading bar variables
                            int i = 0;
                            char a[4] = "\\|/-";
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
                            display_map_with_pc(&first_map, &this_world, &pc);
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
                            for (i = current_scroll; i < first_map.entity_num && i < current_scroll + rows_for_trainers; i++) {
                                struct entity *npc = &first_map.entity_array[i];

                                int delta_x = pc.x - npc->x;
                                int delta_y = pc.y - npc->y;

                            
                                char *n_s = ""; 
                                char *e_w = "";

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
                                    if (current_scroll + rows_for_trainers < first_map.entity_num) {
                                        current_scroll++;
                                    }
                                    break;
                            }
                        }

                        clear();
                        display_map_with_pc(&first_map, &this_world, &pc);
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
                    int terrain_cost = calculate_cost(PC, first_map.grid_array[new_y][new_x]);

                    // Check borders.
                    if (new_x == MAP_WIDTH - 1 || new_x == 0 || new_y == 0 || new_y == MAP_HEIGHT - 1) {
                        mvprintw(0, 0, "Going out of bounds.");
                        refresh();
                    }
                    // Check if terrain is not passible
                    else if (terrain_cost == INFINITY) {
                        mvprintw(0, 0, "You can't cross this impassible object.");
                        refresh();
                    }
                    // Check if we encountered a trainer
                    else if (first_map.entity_map[new_y][new_x] != NULL) {
                        struct entity *bumped_npc = first_map.entity_map[new_y][new_x];
                        
                        // If not defeated - battle
                        if (bumped_npc->npc->defeated == 0) {
                            mvprintw(0, 0, "Battle! Press 'Esc' to exit.");
                            refresh();

                            while (getch() != 27) {
                                ;
                            }
                            
                            bumped_npc->npc->defeated = 1;
                            
                            clear();
                            display_map_with_pc(&first_map, &this_world, &pc);
                        } else {
                            mvprintw(0, 0, "You have defeated this trainer!");
                            refresh();
                        }
                    // Valid place to make a move.
                    } else {
                        successful_move = 1;
                        pc.x = new_x;
                        pc.y = new_y;

                        find_distance_map(&pc, RIVAL, first_map.rival_dist_map, &first_map);
                        find_distance_map(&pc, HIKER, first_map.hiker_dist_map, &first_map);
                    }
                }
            }

            // Update time for PC node before enque it back
            int next_step_cost = calculate_cost(PC, first_map.grid_array[pc.y][pc.x]);
            deq_node.current_time += next_step_cost;

            // Insert updated node back in the priority queue.
            mq_insert_node(&moves_queue, deq_node); 
        } else {
            int step_cost = move_npc(deq_node.entity, &first_map, &pc);

            if (step_cost == -1) {
                mvprintw(0, 0, "Trainer got you! Press 'Esc' to exit");
                refresh();
                
                while (getch() != 27) {
                    ;
                }
                
                deq_node.entity->npc->defeated = 1;
                step_cost = WAIT_COST;
                
                clear();
                display_map_with_pc(&first_map, &this_world, &pc);
            }

            deq_node.current_time += step_cost;

            mq_insert_node(&moves_queue, deq_node);
        }
    }

// 6. Return terminal to the default settings
    endwin();

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