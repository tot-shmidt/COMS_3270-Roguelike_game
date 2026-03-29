#ifndef CHARACTERS_PLAYERS_H
#define CHARACTERS_PLAYERS_H

#include "terrain_generator.h"

// This is a forward declaration to terrain_generator.h, so I don't #include "terrain.. .h"
struct map;

enum entity_type {
    PC = '@',
    HIKER = 'h',
    RIVAL = 'r',
    SWIMMER = 's',
    OTHER = '?'
};

/* This represents some type of a game unit, like player, hiker, rival and so on */
struct entity {
    // Current unit's position
    int x;
    int y;

    enum entity_type type;

    struct pc_data *pc;
    struct npc_data *npc;
};

struct pc_data {
    int temp;
};

struct npc_data {
    int temp;
};

int place_pc_on_road(struct entity *pc, struct map *map);
int initialize_entity(struct entity *entity, enum entity_type type);
int calculate_cost(enum entity_type e_type, enum terrain_types t_type);
void find_distance_map(struct entity *pc, enum entity_type npc_type, int distance[MAP_HEIGHT][MAP_WIDTH], struct map *map);
void print_dist_map(enum entity_type npc_type, int distance_map[MAP_HEIGHT][MAP_WIDTH]);

#endif