#ifndef CHARACTERS_PLAYERS_H
#define CHARACTERS_PLAYERS_H

#include "terrain_generator.h"
#include <vector>
#include "pokemon.h"

#define MY_INFINITY 1000000
#define MAP_HEIGHT 21
#define MAP_WIDTH 80
#define WAIT_COST 12


enum entity_type {
    PC = '@',
    HIKER = 'h',
    RIVAL = 'r',
    PACER = 'p',
    SWIMMER = 'U',
    WANDERER = 'w',
    SENTRIES = 's',
    EXPLORER = 'e'
};

// Base class for all entities.
class Character {
    public:
        // Current unit's position
        int x;
        int y;

        enum entity_type type;
        std::vector<Pokemon> available_pokemons;
};

class Player : public Character {
    public:
        int temp;
};

class NPC : public Character {
    public:
        enum directions direction;
        int defeated;
};


int place_pc_on_road(Player *pc, map *current_map);
int initialize_pc(Player *player);
int initialize_npc(NPC *npc, enum entity_type type);
int calculate_cost(enum entity_type e_type, enum terrain_types t_type);
void find_distance_map(Player *pc, enum entity_type npc_type, int distance[MAP_HEIGHT][MAP_WIDTH], map *map);
void print_dist_map(enum entity_type npc_type, int distance_map[MAP_HEIGHT][MAP_WIDTH]);
int move_npc(NPC *entity, map *map, Player *pc);

#endif