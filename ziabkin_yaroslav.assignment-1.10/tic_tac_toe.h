#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include <stdlib.h>
#include <vector>

class Player;

struct game_state {
    int npc_wins = 0;
    int pc_wins = 0;
    bool game_end = false;
    std::vector<char> cells_vector = std::vector<char>(9, ' '); 
    int turn = rand() % 2; // 0 for PC, 1 for NPC
    char npc_symbol = rand() % 2 == 0 ? 'o': 'x';
    char pc_symbol = npc_symbol == 'o'? 'x': 'o';
    int number_of_turns = 0;
};

void start_tic_tac_toe(Player *pc);

#endif