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

class pokemon_moves_db {
    public:
        int pokemon_id;
        int version_group_id;
        int move_id;
        int pokemon_move_method_id;
        int level;
        int order;
};

class pokemon_species_db {
public:
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
};

class experience_db {
public:
    int growth_rate_id;
    int level;
    int experience;
};

class type_names_db {
public:
    int type_id;
    int local_language_id;
    std::string name;
};

class pokemon_stats_db {
public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
};

class stats_db {
public:
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;
};

class pokemon_types_db {
public:
    int pokemon_id;
    int type_id;
    int slot;
};

// ~~~ PARSING FUNCTIONS ~~~
std::vector<pokemon_db> parse_pokemon(std::ifstream* csv_file);
std::vector<moves_db> parse_moves(std::ifstream* csv_file);
std::vector<pokemon_moves_db> parse_pokemon_moves(std::ifstream* csv_file);
std::vector<pokemon_species_db> parse_pokemon_species(std::ifstream* csv_file);
std::vector<experience_db> parse_experience(std::ifstream* csv_file);
std::vector<type_names_db> parse_type_names(std::ifstream* csv_file);
std::vector<pokemon_stats_db> parse_pokemon_stats(std::ifstream* csv_file);
std::vector<stats_db> parse_stats(std::ifstream* csv_file);
std::vector<pokemon_types_db> parse_pokemon_types(std::ifstream* csv_file);

// ~~~ PRINTING FUNCTIONS ~~~
void print_pokemon_db(std::vector<pokemon_db>* db_vector);
void print_moves_db(std::vector<moves_db>* db_vector);
void print_pokemon_moves_db(std::vector<pokemon_moves_db>* db_vector);
void print_pokemon_species_db(std::vector<pokemon_species_db>* db_vector);
void print_experience_db(std::vector<experience_db>* db_vector);
void print_type_names_db(std::vector<type_names_db>* db_vector);
void print_pokemon_stats_db(std::vector<pokemon_stats_db>* db_vector);
void print_stats_db(std::vector<stats_db>* db_vector);
void print_pokemon_types_db(std::vector<pokemon_types_db>* db_vector);

#endif