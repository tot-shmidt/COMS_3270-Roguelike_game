#include "tic_tac_toe.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#define PC_TURN 0
#define NPC_TURN 1
#define SCR_WIDTH 80
#define SCR_HEIGHT 24 // WAS 25 and it was ok but 25 feels weird number.

void print_frame() {
    for (int i = 0; i < SCR_WIDTH; i++) {
        mvaddch(0, i, ' ');
        mvaddch(23, i, ' ');
    }

    for (int i = 0; i < SCR_HEIGHT; i++) {
        mvaddch(i, 0, ' ');
        mvaddch(i, 79, ' ');
    }
}
// 80 - 20 = 60 / 2 = 30
void print_header() {
    attron(COLOR_PAIR(12));
    mvprintw(0, (SCR_WIDTH - strlen(" Test your luck and intelligence! ")) / 2 ," Test your luck and intelligence! ");
    attroff(COLOR_PAIR(12));
}

void paint_background() {
    for (int i=0; i < SCR_HEIGHT; i++) {
        for (int j=0; j < SCR_WIDTH; j++) {
            mvaddch(i, j, ' ' | COLOR_PAIR(11));
        }
    }
}

// Prints cross which is 17 lines wide and 11 lines hight.
void print_board() {
    int horizontal_offset = (SCR_WIDTH - 17 ) / 2;
    int vertical_offset = (SCR_HEIGHT - 11) / 2;

    // Draw horizontal lines
    for (int i = horizontal_offset; i < horizontal_offset + 17; i++) {
        mvaddch(vertical_offset + 3, i, ACS_HLINE | COLOR_PAIR(11));
    }

    for (int i = horizontal_offset; i < horizontal_offset + 17; i++) {
        mvaddch(vertical_offset + 7, i, ACS_HLINE | COLOR_PAIR(11));
    }

    // Draw vertical lines
    for (int i = vertical_offset; i < vertical_offset + 11; i++) {
        mvaddch(i, horizontal_offset + 5, ACS_VLINE | COLOR_PAIR(11));
    }

    for (int i = vertical_offset; i < vertical_offset + 11; i++) {
        mvaddch(i, horizontal_offset + 11, ACS_VLINE | COLOR_PAIR(11));
    }

    // Insert crosses
    mvaddch(vertical_offset + 3, horizontal_offset + 5, ACS_PLUS | COLOR_PAIR(11));
    mvaddch(vertical_offset + 7, horizontal_offset + 5, ACS_PLUS | COLOR_PAIR(11));

    mvaddch(vertical_offset + 3, horizontal_offset + 11, ACS_PLUS | COLOR_PAIR(11));
    mvaddch(vertical_offset + 7, horizontal_offset + 11, ACS_PLUS | COLOR_PAIR(11));

    // for testing
    /*
    mvaddch(vertical_offset, horizontal_offset, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset, horizontal_offset + 6, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset, horizontal_offset + 12, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 4, horizontal_offset, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 4, horizontal_offset + 6, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 4, horizontal_offset + 12, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 8, horizontal_offset, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 8, horizontal_offset + 6, '*' | COLOR_PAIR(11));
    mvaddch(vertical_offset + 8, horizontal_offset + 12, '*' | COLOR_PAIR(11));
    */
}

void print_symbol(char symbol, int cell, struct game_state *game_state, bool colorful) {
    int horizontal_offset = (SCR_WIDTH - 17 ) / 2;
    int vertical_offset = (SCR_HEIGHT - 11) / 2;

    [[maybe_unused]] int x,y;

    switch (cell) {
        case 7: x = horizontal_offset; y = vertical_offset; break;
        case 8: x = horizontal_offset + 6; y = vertical_offset; break;
        case 9: x = horizontal_offset + 12; y = vertical_offset;break;
        case 4: x = horizontal_offset; y = vertical_offset + 4; break;
        case 5: x = horizontal_offset + 6; y = vertical_offset + 4; break;
        case 6: x = horizontal_offset + 12; y = vertical_offset + 4; break;
        case 1: x = horizontal_offset; y = vertical_offset + 8; break;
        case 2: x = horizontal_offset + 6; y = vertical_offset + 8; break;
        case 3: x = horizontal_offset + 12; y = vertical_offset + 8; break;
    }

    // Determine collor we need to use to print. 0 for Player, 1 for Computer
    int color;
    if (game_state->turn == 0 && colorful) {
        color = 13;
    } else if (game_state->turn == 1 && colorful) {
        color = 14;
    } else {
        color = 11;
    }

    // Print symbol with correct color
    if (symbol == 'o') {
    // /---\. 
    // |   |
    // \---/
        mvaddch(y, x, '/' | COLOR_PAIR(color));
        mvaddch(y, x+1, ACS_S1 | COLOR_PAIR(color));
        mvaddch(y, x+2, ACS_S1 | COLOR_PAIR(color));
        mvaddch(y, x+3, ACS_S1 | COLOR_PAIR(color));
        mvaddch(y, x+4, '\\' | COLOR_PAIR(color));
        mvaddch(y+1, x+4, '|' | COLOR_PAIR(color));
        mvaddch(y+2, x+4, '/' | COLOR_PAIR(color));
        mvaddch(y+2, x+1, ACS_S9 | COLOR_PAIR(color));
        mvaddch(y+2, x+2, ACS_S9 | COLOR_PAIR(color));
        mvaddch(y+2, x+3, ACS_S9 | COLOR_PAIR(color));
        mvaddch(y+2, x, '\\' | COLOR_PAIR(color));
        mvaddch(y+1, x, '|' | COLOR_PAIR(color));

    } else if (symbol == 'x') {
    // \   /
    //  ---
    // /   \.
        mvaddch(y, x, '\\' | COLOR_PAIR(color));
        mvaddch(y, x+4, '/' | COLOR_PAIR(color));
        mvaddch(y+1, x+1, ACS_HLINE | COLOR_PAIR(color));
        mvaddch(y+1, x+2, ACS_HLINE | COLOR_PAIR(color));
        mvaddch(y+1, x+3, ACS_HLINE | COLOR_PAIR(color));
        mvaddch(y+2, x+4, '\\' | COLOR_PAIR(color));
        mvaddch(y+2, x, '/' | COLOR_PAIR(color));
    } else {
        ;
    }

    refresh();
}

// Consolidate all the function calls to draw the screen of the game
void set_up_screen(struct game_state * game_state) {
    // To not erase symbols after the game ends
    if (game_state->game_end != true) {
        paint_background();
    }

    print_frame();
    print_header();
    print_board();

    // Write here logic for stats TO-DO
    attron(COLOR_PAIR(11));
    mvprintw(2, 3, "Your score:  %d", game_state->pc_wins);
    mvprintw(3, 3, "Rival score: %d", game_state->npc_wins);
    attroff(COLOR_PAIR(11));
}

// Reset game state for a new round
void prepare_new_round(struct game_state * game_state) {
    for (int i = 0; i < (int) game_state->cells_vector.size(); i++) {
        game_state->cells_vector.at(i) = ' ';
    }

    game_state->number_of_turns = 0;

    // Erase all the symbols from the game board(redraw everything);
    set_up_screen(game_state);
    refresh();
}

// Change turn
void change_turn(struct game_state * game_state) {
    if (game_state->turn == PC_TURN) {
        game_state->turn = NPC_TURN;
    } else if (game_state->turn == NPC_TURN) {
        game_state->turn = PC_TURN;
    }
}

// Check win condition
bool check_win(struct game_state & game_state, char symbol) {
    if ((game_state.cells_vector[0] == symbol && game_state.cells_vector[1] == symbol && game_state.cells_vector[2] == symbol) ||
        (game_state.cells_vector[3] == symbol && game_state.cells_vector[4] == symbol && game_state.cells_vector[5] == symbol) ||
        (game_state.cells_vector[6] == symbol && game_state.cells_vector[7] == symbol && game_state.cells_vector[8] == symbol) ||
        (game_state.cells_vector[0] == symbol && game_state.cells_vector[3] == symbol && game_state.cells_vector[6] == symbol) ||
        (game_state.cells_vector[1] == symbol && game_state.cells_vector[4] == symbol && game_state.cells_vector[7] == symbol) ||
        (game_state.cells_vector[2] == symbol && game_state.cells_vector[5] == symbol && game_state.cells_vector[8] == symbol) ||
        (game_state.cells_vector[0] == symbol && game_state.cells_vector[4] == symbol && game_state.cells_vector[8] == symbol) ||
        (game_state.cells_vector[2] == symbol && game_state.cells_vector[4] == symbol && game_state.cells_vector[6] == symbol)) {
        return true;
    } else {
        return false;
    }
}

// Print in color winning sequence
void paint_win_sequence(char symbol, struct game_state *game_state) {
    std::vector<char> * arr = &(game_state->cells_vector);
    // Find winning cells
    const int win_lines[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
    
    for (int i = 0; i < 8; i++) {
        if (arr->at(win_lines[i][0]) == symbol && arr->at(win_lines[i][1]) == symbol && arr->at(win_lines[i][2]) == symbol) {
            // Change color based on who won. Red for computer, green for pc.
            print_symbol(symbol, win_lines[i][0] + 1, game_state, true);
            print_symbol(symbol, win_lines[i][1] + 1, game_state, true);
            print_symbol(symbol, win_lines[i][2] + 1, game_state, true);

            break;
        }
    }
}


void start_tic_tac_toe(Player *pc){
    // variables for the game
    struct game_state game_state;

    // Add background, frame, header, cross in the middle
    clear();
    set_up_screen(&game_state);
    refresh();

    

    // main game loop
    while (!(game_state.game_end)) {
        if (game_state.turn == PC_TURN) {
            // get valid input
            bool valid_input = false;
            char symbol = game_state.pc_symbol;

            flushinp(); // Clears buffer that could be filled during usleep()
            int input;
            while (!valid_input) {
                input = getch() - '0';
                if ((input == 1 || input == 2 || input == 3 ||
                    input == 4 || input == 5 || input == 6 ||
                    input == 7 || input == 8 || input == 9) && game_state.cells_vector.at(input - 1) == ' ') {
                        valid_input = true;
                }
            }

            // Have to place figure in the cell.
            game_state.cells_vector.at(input - 1) = symbol;
            print_symbol(symbol, input, &game_state, false);
            game_state.number_of_turns++;

            // check for win condition
            if  (check_win(game_state, symbol)) {
                    game_state.pc_wins++;
                    usleep(1000000);

                    // paint winning sequence 
                    paint_win_sequence(game_state.pc_symbol, &game_state);
                    refresh();
                    usleep(1500000);
                    
                    if (game_state.pc_wins == 2) {
                        game_state.game_end = true;
                    }

                    prepare_new_round(&game_state);
            // check for tie
            } else if (game_state.number_of_turns == 9) {
                usleep(1500000);
                prepare_new_round(&game_state);
            }

            // change turn
            change_turn(&game_state);

        } else if (game_state.turn == NPC_TURN) {
            usleep(1000000);

            // find place for a move
            bool valid_input = false;
            int input;
            while (!valid_input) {
                input = rand() % 9 + 1;
                if (game_state.cells_vector.at(input - 1) == ' ') {
                    valid_input = true;
                }
            }

            // Have to place figure in the cell.
            char symbol = game_state.npc_symbol;
            game_state.cells_vector.at(input - 1) = symbol;
            print_symbol(symbol, input, &game_state, false);
            game_state.number_of_turns++;

            // check for win condition or tie
            if (check_win(game_state, symbol)) {
                game_state.npc_wins++;
                usleep(1000000);
                
                // paint winning sequence
                paint_win_sequence(game_state.npc_symbol, &game_state);
                refresh();
                usleep(1500000);

                if (game_state.npc_wins == 2) {
                    game_state.game_end = true;
                }

                prepare_new_round(&game_state);
            } else if (game_state.number_of_turns == 9) {
                usleep(1500000);
                prepare_new_round(&game_state);
            }

            // change_turn
            change_turn(&game_state);
        }
    }

    attron(COLOR_PAIR(11));

    // print you win or you loose
    if (game_state.game_end == true) {
        if (game_state.pc_wins == 2) {
            mvprintw(4, (SCR_WIDTH - strlen("You won!")) / 2, "You win!");
        } else if (game_state.npc_wins == 2) {
            mvprintw(4, (SCR_WIDTH - strlen("You lost!")) / 2, "You lost!");
        }
    }

    mvprintw(SCR_HEIGHT - 3, (SCR_WIDTH - 1 - strlen("Press any key to exit: ") - 2) , "Press any key to exit: ");
    refresh();
    
    attroff(COLOR_PAIR(11));
    getch();

    clear();
}