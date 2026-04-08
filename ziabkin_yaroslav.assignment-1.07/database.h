#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <fstream>

// ~~~ CLASSES ~~~

class pokemon_db {
    // id,identifier,species_id,height,weight,base_experience,order,is_default
    public:
        int id;
        std::string identifier;
        int species_id;
        int height;
        int weight;
        int base_experience;
        int order;
        int is_default;
};

class moves_db {
    // id,identifier,generation_id,type_id,power,pp,accuracy,priority,target_id,
    // damage_class_id,effect_id,effect_chance,contest_type_id,contest_effect_id,super_contest_effect_id
    public:
        int id;
        std::string identifier;
        int generation_id;
        int type_id;
        int power;
        int pp;
        int accuracy;
        int priority;
        int target_id;
        int damage_class_id;
        int effect_id;
        int effect_chance;
        int contest_type_id;
        int contest_effect_id;
        int super_contest_effect_id;
};

// ~~~ PARSING FUNCTIONS ~~~
std::vector<pokemon_db> parse_pokemon(std::ifstream* csv_file);
std::vector<moves_db> parse_moves(std::ifstream* csv_file);

// ~~~ PRINTING FUNCTIONS ~~~
void print_pokemon_db(std::vector<pokemon_db>* db_vector);
void print_moves_db(std::vector<moves_db>* db_vector);

#endif