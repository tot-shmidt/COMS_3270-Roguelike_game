#ifndef POKEMON_H
#define POKEMON_H
#include <string>
#include <vector>

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

    std::vector<std::string> move_set;

    Pokemon(int param_level, int param_species_id, struct Database *database);
};

#endif