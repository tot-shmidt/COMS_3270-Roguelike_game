#ifndef POKEMON_H
#define POKEMON_H
#include <string>
#include <vector>

class Player;

class Pokemon {
    public:
    
    std::string identifier;
    int current_level;
    int species_id;
    int gender;
    int shiny;

    // 0=HP, 1=Attack, 2=Defense, 3=Speed, 4=Specia Atack, 5=Special Defense
    int base_stats[6];
    int ivs[6];
    int actual_stats[6];

    int max_hp;
    int current_hp;

    std::vector<std::string> move_set;

    Pokemon(int param_level, int param_species_id, struct Database *database);
};

int select_initial_pokemon(Player *pc, Database *database);
int calculate_damage(int level, int power, int attack, int defense, bool is_stab, float type_modifier);

#endif