#include "database.h"
#include <iostream>
#include <sstream>
#include <limits.h>
#include <string.h>


int read_db_in_memory(struct Database *database) {
    // PATH 1: /share/cs327/
    const char *base1 = "/share/cs327/pokedex/pokedex/data/csv/";

    // PATH 2: $HOME/.poke327/ , use getenv() to resolve the value of the HOME
    char *home_path = getenv("HOME");
    if (home_path == NULL) {
        printf("HOME variable was not found.\n");
        return 1;
    }

    const char *base2_without_home = "/.poke327/pokedex/pokedex/data/csv/";

    int base_length2 = strlen(home_path) + strlen(base2_without_home) + 1;
    char *base2 = (char *) malloc(base_length2);

    // String print formatted is like printf() but instead it prints directly into my string variable. Automatically add \0 at the end.
    // snprintf(destination, max_size, "format string", variables...)
    snprintf(base2, base_length2,"%s%s", home_path, base2_without_home);

    // PATH 3: mine local path to the DB on my comuter
    const char *base3 = "/home/tot_shmidt/Iowa_State/Spring_2026/COMS_3270/pokedex/pokedex/data/csv/";

    // ##########################
    // #   FILE 1: pokemon.csv  #
    // ##########################
    int total_length1 = strlen(base1) + strlen("pokemon.csv");
    int total_length2 = strlen(base2) + strlen("pokemon.csv");
    int total_length3 = strlen(base3) + strlen("pokemon.csv");

    char *path1 = (char *) malloc(total_length1 + strlen("pokemon.csv") + 1);
    char *path2 = (char *) malloc(total_length2 + strlen("pokemon.csv") + 1);
    char *path3 = (char *) malloc(total_length3 + strlen("pokemon.csv") + 1);

    // String print formatted is like printf() but instead it prints directly into my string variable. Automatically add \0 at the end.
    // snprintf(destination, max_size, "format string", variables...)
    snprintf(path1, total_length1 + 1, "%s%s", base1, "pokemon.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "pokemon.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "pokemon.csv");

    // 3. OPEN 3 locations, one by one.
    std::ifstream csv_file;

    // Open three paths one by one.
    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "pokemon.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->pokemon = parse_pokemon(&csv_file);

    // Prepare csv_file file stream to open the next file.
    csv_file.close();
    csv_file.clear();

    free(path1);
    free(path2);
    free(path3);


    // ##########################
    // #   FILE 2: moves.csv  #
    // ##########################
    total_length1 = strlen(base1) + strlen("moves.csv");
    total_length2 = strlen(base2) + strlen("moves.csv");
    total_length3 = strlen(base3) + strlen("moves.csv");

    path1 = (char *) malloc(total_length1 + strlen("moves.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("moves.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("moves.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "moves.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "moves.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "moves.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no moves.csv at three locations." << std::endl;
        return 1;
    }

    // I have opened file. Parse it and store in RAM.
    database->moves = parse_moves(&csv_file);

    // Prepare csv_file file stream to open the next file.
    csv_file.close();
    csv_file.clear();

    free(path1);
    free(path2);
    free(path3);

    // ################################
    // #   FILE 3: pokemon_moves.csv  #
    // ################################
    total_length1 = strlen(base1) + strlen("pokemon_moves.csv");
    total_length2 = strlen(base2) + strlen("pokemon_moves.csv");
    total_length3 = strlen(base3) + strlen("pokemon_moves.csv");

    path1 = (char *) malloc(total_length1 + strlen("pokemon_moves.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("pokemon_moves.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("pokemon_moves.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "pokemon_moves.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "pokemon_moves.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "pokemon_moves.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no pokemon_moves.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->pokemon_moves = parse_pokemon_moves(&csv_file);

    csv_file.close();
    csv_file.clear();
    
    free(path1);
    free(path2);
    free(path3);

    // ##################################
    // #   FILE 4: pokemon_species.csv  #
    // ##################################
    total_length1 = strlen(base1) + strlen("pokemon_species.csv");
    total_length2 = strlen(base2) + strlen("pokemon_species.csv");
    total_length3 = strlen(base3) + strlen("pokemon_species.csv");

    path1 = (char *) malloc(total_length1 + strlen("pokemon_species.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("pokemon_species.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("pokemon_species.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "pokemon_species.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "pokemon_species.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "pokemon_species.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no pokemon_species.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->pokemon_species = parse_pokemon_species(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);

    // #############################
    // #   FILE 5: experience.csv  #
    // #############################
    total_length1 = strlen(base1) + strlen("experience.csv");
    total_length2 = strlen(base2) + strlen("experience.csv");
    total_length3 = strlen(base3) + strlen("experience.csv");

    path1 = (char *) malloc(total_length1 + strlen("experience.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("experience.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("experience.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "experience.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "experience.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "experience.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no experience.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->experience = parse_experience(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);

    
    // #############################
    // #   FILE 6: type_names.csv  #
    // #############################
    total_length1 = strlen(base1) + strlen("type_names.csv");
    total_length2 = strlen(base2) + strlen("type_names.csv");
    total_length3 = strlen(base3) + strlen("type_names.csv");

    path1 = (char *) malloc(total_length1 + strlen("type_names.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("type_names.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("type_names.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "type_names.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "type_names.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "type_names.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no type_names.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->type_names = parse_type_names(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);


    // ################################
    // #   FILE 7: pokemon_stats.csv  #
    // ################################
    total_length1 = strlen(base1) + strlen("pokemon_stats.csv");
    total_length2 = strlen(base2) + strlen("pokemon_stats.csv");
    total_length3 = strlen(base3) + strlen("pokemon_stats.csv");

    path1 = (char *) malloc(total_length1 + strlen("pokemon_stats.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("pokemon_stats.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("pokemon_stats.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "pokemon_stats.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "pokemon_stats.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "pokemon_stats.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no pokemon_stats.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->pokemon_stats = parse_pokemon_stats(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);


    // ##########################
    // #   FILE 8:  stats.csv   #
    // ##########################
    total_length1 = strlen(base1) + strlen("stats.csv");
    total_length2 = strlen(base2) + strlen("stats.csv");
    total_length3 = strlen(base3) + strlen("stats.csv");

    path1 = (char *) malloc(total_length1 + strlen("stats.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("stats.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("stats.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "stats.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "stats.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "stats.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no stats.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->stats = parse_stats(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);

    // ##################################
    // #   FILE 9:  pokemon_types.csv   #
    // ##################################
    total_length1 = strlen(base1) + strlen("pokemon_types.csv");
    total_length2 = strlen(base2) + strlen("pokemon_types.csv");
    total_length3 = strlen(base3) + strlen("pokemon_types.csv");

    path1 = (char *) malloc(total_length1 + strlen("pokemon_types.csv") + 1);
    path2 = (char *) malloc(total_length2 + strlen("pokemon_types.csv") + 1);
    path3 = (char *) malloc(total_length3 + strlen("pokemon_types.csv") + 1);

    snprintf(path1, total_length1 + 1, "%s%s", base1, "pokemon_types.csv");
    snprintf(path2, total_length2 + 1, "%s%s", base2, "pokemon_types.csv");
    snprintf(path3, total_length2 + 1, "%s%s", base3, "pokemon_types.csv");

    // Try first one:
    csv_file.open(path1);

    // If not there, try the second one.
    if (!csv_file.is_open()) {
        csv_file.open(path2);
    }

    // If not there, try the third one.
    if (!csv_file.is_open()) {
        csv_file.open(path3);
    }

    // If non was successful, exit the program
    if (!csv_file.is_open()) {
        std::cout << "no pokemon_types.csv at three locations." << std::endl;
        return 1;
    }

    // 4. I have opened file. Parse it and store in RAM.
    database->pokemon_types = parse_pokemon_types(&csv_file);

    csv_file.close();
    csv_file.clear();

    free(path1); free(path2); free(path3);
    free(base2);

    return 0;
}


// ~~~ PARSING FUNCTIONS ~~~
int atoi_check(std::string *str) {
    if ((*str).empty()) {
        return INT_MAX;
    } else {
        return std::stoi(*str);
    }
}

std::vector<pokemon_db> parse_pokemon(std::ifstream* csv_file) {
    // Vector to store pokemon_db objects
    std::vector<pokemon_db> pokemon_db_objects = std::vector<pokemon_db>();

    if (csv_file == NULL) {return pokemon_db_objects;}

    // String to read 1 record from .csv file
    std::string record = std::string();
    
    std::getline(*csv_file, record);    // Read first row and slkip it.

    while (std::getline(*csv_file, record)) {
        // Initialize an object in which we will fill in the data from .csv file
        pokemon_db db_object = pokemon_db();

        // Create stringstream object to split records at commas
        std::stringstream string_stream = std::stringstream(record);

        std::string entry = std::string(); // this will hold each field of the record, one by one

        // ####################
        // #      1.id        #
        // ####################
        // stringstream acts exactly like a file stream (ifstream), but instead of reading from a file on hard drive, it reads from a string in RAM.
        // Because it acts like a file stream, I can use the exact same std::getline function, but with a third argument: the delimiter.
        std::getline(string_stream, entry, ',');

        // Convert character 1 of id to integer 1, as our record is just a long string.
        db_object.id = std::stoi(entry);

        // ####################
        // #   2.identifier   #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.identifier = entry;

        // ####################
        // #   3.species_id   #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.species_id = std::stoi(entry);

        // ####################
        // #     4.height     #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.height = std::stoi(entry);

        // ####################
        // #     5.weight     #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.weight = std::stoi(entry);

        // #####################
        // # 6.base_experience #
        // #####################
        std::getline(string_stream,entry, ',');
        db_object.base_experience = std::stoi(entry);

        // ####################
        // #     7.order     #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.order = std::stoi(entry);

        // ####################
        // #  8.is_default    #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.is_default = std::stoi(entry);

        // Push final db_object to the back of the vector
        pokemon_db_objects.push_back(db_object);
    }

    return pokemon_db_objects;
}

std::vector<moves_db> parse_moves(std::ifstream* csv_file) {
    std::vector<moves_db> moves_db_objects = std::vector<moves_db>();

    if (csv_file == NULL) {return moves_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        moves_db db_object = moves_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        // ####################
        // #      1.id        #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.id = std::stoi(entry);

        // ####################
        // #   2.identifier   #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.identifier = entry;

        // ####################
        // # 3.generation_id  #
        // ####################
        std::getline(string_stream, entry, ',');
        if (entry.empty()) {
            db_object.generation_id = INT_MAX;
        } else {
            db_object.generation_id = std::stoi(entry);
        }

        // ####################
        // #     4.type_id    #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.type_id = atoi_check(&entry);

        // ####################
        // #     5.power      #
        // ####################
        std::getline(string_stream, entry, ',');
        if (entry.empty()) {
            db_object.power = INT_MAX;
        } else {
            db_object.power = std::stoi(entry);
        }

        // #####################
        // #       6.pp        #
        // #####################
        std::getline(string_stream,entry, ',');
        db_object.pp = atoi_check(&entry);

        // ####################
        // #     7.accuracy   #
        // ####################
        std::getline(string_stream, entry, ',');
        if (entry.empty()) {
            db_object.accuracy = INT_MAX;
        } else {
            db_object.accuracy = std::stoi(entry);
        }

        // ####################
        // #    8.priority    #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.priority = atoi_check(&entry);

        // #####################
        // #   9.target_id     #
        // #####################
        std::getline(string_stream,entry, ',');
        db_object.target_id = atoi_check(&entry);

        // #######################
        // #  10.damage_class_id #
        // #######################
        std::getline(string_stream, entry, ',');
        db_object.damage_class_id = atoi_check(&entry);

        // ####################
        // #    11.effect_id  #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.effect_id = atoi_check(&entry);

        // #####################
        // #  12.effect_chance #
        // #####################
        std::getline(string_stream,entry, ',');
        if (entry.empty()) {
            db_object.effect_chance = INT_MAX;
        } else {
            db_object.effect_chance = std::stoi(entry);
        }

        // #######################
        // #  13.contest_type_id #
        // #######################
        std::getline(string_stream, entry, ',');
        db_object.contest_type_id = atoi_check(&entry);

        // ##########################
        // #  14.contest_effect_id  #
        // ##########################
        std::getline(string_stream, entry, ',');
        db_object.contest_effect_id = atoi_check(&entry);

        // ################################
        // #  15.super_contest_effect_id  #
        // ################################
        std::getline(string_stream, entry, ',');
        db_object.super_contest_effect_id = atoi_check(&entry);

        // Push final db_object to the back of the vector
        moves_db_objects.push_back(db_object);
    }

    return moves_db_objects;
}

std::vector<pokemon_moves_db> parse_pokemon_moves(std::ifstream* csv_file) {
    std::vector<pokemon_moves_db> pokemon_moves_db_objects = std::vector<pokemon_moves_db>();

    if (csv_file == NULL) {return pokemon_moves_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        pokemon_moves_db db_object = pokemon_moves_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        // ####################
        // #  1.pokemon_id    #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.pokemon_id = atoi_check(&entry);

        // #######################
        // #  2.version_group_id #
        // #######################
        std::getline(string_stream, entry, ',');
        db_object.version_group_id = atoi_check(&entry);

        // ####################
        // #     3.move_id    #
        // ####################
        std::getline(string_stream, entry, ',');
        db_object.move_id = atoi_check(&entry);

        // #############################
        // #  4.pokemon_move_method_id #
        // #############################
        std::getline(string_stream, entry, ',');
        db_object.pokemon_move_method_id = atoi_check(&entry);

        // ######################
        // #       5.level      #
        // ######################
        std::getline(string_stream, entry, ',');
        db_object.level = atoi_check(&entry);

        // ######################
        // #       6.order      #
        // ######################
        std::getline(string_stream, entry, ',');
        db_object.order = atoi_check(&entry);

        pokemon_moves_db_objects.push_back(db_object);
    }

    return pokemon_moves_db_objects;
}

std::vector<pokemon_species_db> parse_pokemon_species(std::ifstream* csv_file) {
    std::vector<pokemon_species_db> pokemon_species_db_objects;

    if (csv_file == NULL) return pokemon_species_db_objects;

    std::string record;
    std::string entry;
    
    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        pokemon_species_db db_object;
        std::stringstream string_stream(record);

        std::getline(string_stream, entry, ','); db_object.id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.identifier = entry;
        std::getline(string_stream, entry, ','); db_object.generation_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.evolves_from_species_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.evolution_chain_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.color_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.shape_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.habitat_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.gender_rate = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.capture_rate = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.base_happiness = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.is_baby = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.hatch_counter = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.has_gender_differences = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.growth_rate_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.forms_switchable = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.is_legendary = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.is_mythical = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.order = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.conquest_order = atoi_check(&entry);

        pokemon_species_db_objects.push_back(db_object);
    }
    return pokemon_species_db_objects;
}

std::vector<experience_db> parse_experience(std::ifstream* csv_file) {
    std::vector<experience_db> experience_db_objects = std::vector<experience_db>();

    if (csv_file == NULL) {return experience_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        experience_db db_object = experience_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        std::getline(string_stream, entry, ','); db_object.growth_rate_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.level = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.experience = atoi_check(&entry);

        experience_db_objects.push_back(db_object);
    }

    return experience_db_objects;
}

std::vector<type_names_db> parse_type_names(std::ifstream* csv_file) {
    std::vector<type_names_db> type_names_db_objects = std::vector<type_names_db>();

    if (csv_file == NULL) {return type_names_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        type_names_db db_object = type_names_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        std::getline(string_stream, entry, ','); db_object.type_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.local_language_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.name = entry;

        type_names_db_objects.push_back(db_object);
    }

    return type_names_db_objects;
}

std::vector<pokemon_stats_db> parse_pokemon_stats(std::ifstream* csv_file) {
    std::vector<pokemon_stats_db> pokemon_stats_db_objects = std::vector<pokemon_stats_db>();

    if (csv_file == NULL) {return pokemon_stats_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        pokemon_stats_db db_object = pokemon_stats_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        std::getline(string_stream, entry, ','); db_object.pokemon_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.stat_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.base_stat = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.effort = atoi_check(&entry);

        pokemon_stats_db_objects.push_back(db_object);
    }

    return pokemon_stats_db_objects;
}

std::vector<stats_db> parse_stats(std::ifstream* csv_file) {
    std::vector<stats_db> stats_db_objects = std::vector<stats_db>();

    if (csv_file == NULL) {return stats_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        stats_db db_object = stats_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        std::getline(string_stream, entry, ','); db_object.id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.damage_class_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.identifier = entry;
        std::getline(string_stream, entry, ','); db_object.is_battle_only = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.game_index = atoi_check(&entry);

        stats_db_objects.push_back(db_object);
    }

    return stats_db_objects;
}

std::vector<pokemon_types_db> parse_pokemon_types(std::ifstream* csv_file) {
    std::vector<pokemon_types_db> pokemon_types_db_objects = std::vector<pokemon_types_db>();

    if (csv_file == NULL) {return pokemon_types_db_objects;}

    std::string record = std::string();

    std::getline(*csv_file, record);

    while (std::getline(*csv_file, record)) {
        pokemon_types_db db_object = pokemon_types_db();

        std::stringstream string_stream = std::stringstream(record);
        std::string entry = std::string();

        std::getline(string_stream, entry, ','); db_object.pokemon_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.type_id = atoi_check(&entry);
        std::getline(string_stream, entry, ','); db_object.slot = atoi_check(&entry);

        pokemon_types_db_objects.push_back(db_object);
    }

    return pokemon_types_db_objects;
}

// ~~~ PRINTING FUNCTIONS ~~~
void print_pokemon_db(std::vector<pokemon_db>* db_vector) {
    for (pokemon_db obj : *db_vector) {
        std::cout << obj.id << ", " << obj.identifier << ", " << obj.species_id << ", " << obj.height << ", " << obj.weight << ", " << obj.base_experience << ", " << obj.order << ", " << obj.is_default << std::endl;
    }
}

void print_moves_db(std::vector<moves_db>* db_vector) {
    for (moves_db obj : *db_vector) {
        std::cout << obj.id << "," << obj.identifier << ",";
        std::cout << (obj.generation_id == INT_MAX ? "" : std::to_string(obj.generation_id)) << ",";
        std::cout << (obj.type_id == INT_MAX ? "" : std::to_string(obj.type_id)) << ",";
        std::cout << (obj.power == INT_MAX ? "" : std::to_string(obj.power)) << ",";
        std::cout << (obj.pp == INT_MAX ? "" : std::to_string(obj.pp)) << ",";
        std::cout << (obj.accuracy == INT_MAX ? "" : std::to_string(obj.accuracy)) << ",";
        std::cout << (obj.priority == INT_MAX ? "" : std::to_string(obj.priority)) << ",";
        std::cout << (obj.target_id == INT_MAX ? "" : std::to_string(obj.target_id)) << ",";
        std::cout << (obj.damage_class_id == INT_MAX ? "" : std::to_string(obj.damage_class_id)) << ",";
        std::cout << (obj.effect_id == INT_MAX ? "" : std::to_string(obj.effect_id)) << ",";
        std::cout << (obj.effect_chance == INT_MAX ? "" : std::to_string(obj.effect_chance)) << ",";
        std::cout << (obj.contest_type_id == INT_MAX ? "" : std::to_string(obj.contest_type_id)) << ",";
        std::cout << (obj.contest_effect_id == INT_MAX ? "" : std::to_string(obj.contest_effect_id)) << ",";
        std::cout << (obj.super_contest_effect_id == INT_MAX ? "" : std::to_string(obj.super_contest_effect_id)) << std::endl;
    }
}

void print_pokemon_moves_db(std::vector<pokemon_moves_db>* db_vector) {
    for (pokemon_moves_db obj : *db_vector) {
        std::cout << obj.pokemon_id << ", " << obj.version_group_id << ", " << obj.move_id << ", " << obj.pokemon_move_method_id << ", " << obj.level << ", " << (obj.order == INT_MAX ? "" : std::to_string(obj.order))  << std::endl;
    }
}

void print_pokemon_species_db(std::vector<pokemon_species_db>* db_vector) {
    for (const pokemon_species_db& obj : *db_vector) {
        std::cout << (obj.id == INT_MAX ? "" : std::to_string(obj.id)) << ","
            << obj.identifier << ","
            << (obj.generation_id == INT_MAX ? "" : std::to_string(obj.generation_id)) << ","
            << (obj.evolves_from_species_id == INT_MAX ? "" : std::to_string(obj.evolves_from_species_id)) << ","
            << (obj.evolution_chain_id == INT_MAX ? "" : std::to_string(obj.evolution_chain_id)) << ","
            << (obj.color_id == INT_MAX ? "" : std::to_string(obj.color_id)) << ","
            << (obj.shape_id == INT_MAX ? "" : std::to_string(obj.shape_id)) << ","
            << (obj.habitat_id == INT_MAX ? "" : std::to_string(obj.habitat_id)) << ","
            << (obj.gender_rate == INT_MAX ? "" : std::to_string(obj.gender_rate)) << ","
            << (obj.capture_rate == INT_MAX ? "" : std::to_string(obj.capture_rate)) << ","
            << (obj.base_happiness == INT_MAX ? "" : std::to_string(obj.base_happiness)) << ","
            << (obj.is_baby == INT_MAX ? "" : std::to_string(obj.is_baby)) << ","
            << (obj.hatch_counter == INT_MAX ? "" : std::to_string(obj.hatch_counter)) << ","
            << (obj.has_gender_differences == INT_MAX ? "" : std::to_string(obj.has_gender_differences)) << ","
            << (obj.growth_rate_id == INT_MAX ? "" : std::to_string(obj.growth_rate_id)) << ","
            << (obj.forms_switchable == INT_MAX ? "" : std::to_string(obj.forms_switchable)) << ","
            << (obj.is_legendary == INT_MAX ? "" : std::to_string(obj.is_legendary)) << ","
            << (obj.is_mythical == INT_MAX ? "" : std::to_string(obj.is_mythical)) << ","
            << (obj.order == INT_MAX ? "" : std::to_string(obj.order)) << ","
            << (obj.conquest_order == INT_MAX ? "" : std::to_string(obj.conquest_order)) << std::endl;
    }
}

void print_experience_db(std::vector<experience_db>* db_vector) {
    for (const experience_db& obj : *db_vector) {
        std::cout << (obj.growth_rate_id == INT_MAX ? "" : std::to_string(obj.growth_rate_id)) << ","
            << (obj.level == INT_MAX ? "" : std::to_string(obj.level)) << ","
            << (obj.experience == INT_MAX ? "" : std::to_string(obj.experience)) << std::endl;
    }
}

void print_type_names_db(std::vector<type_names_db>* db_vector) {
    for (const type_names_db& obj : *db_vector) {
        std::cout << (obj.type_id == INT_MAX ? "" : std::to_string(obj.type_id)) << ","
            << (obj.local_language_id == INT_MAX ? "" : std::to_string(obj.local_language_id)) << ","
            << obj.name << std::endl;
    }
}

void print_pokemon_stats_db(std::vector<pokemon_stats_db>* db_vector) {
    for (const pokemon_stats_db& obj : *db_vector) {
        std::cout << (obj.pokemon_id == INT_MAX ? "" : std::to_string(obj.pokemon_id)) << ","
            << (obj.stat_id == INT_MAX ? "" : std::to_string(obj.stat_id)) << ","
            << (obj.base_stat == INT_MAX ? "" : std::to_string(obj.base_stat)) << ","
            << (obj.effort == INT_MAX ? "" : std::to_string(obj.effort)) << std::endl;
    }
}

void print_stats_db(std::vector<stats_db>* db_vector) {
    for (const stats_db& obj : *db_vector) {
        std::cout << (obj.id == INT_MAX ? "" : std::to_string(obj.id)) << ","
            << (obj.damage_class_id == INT_MAX ? "" : std::to_string(obj.damage_class_id)) << ","
            << obj.identifier << ","
            << (obj.is_battle_only == INT_MAX ? "" : std::to_string(obj.is_battle_only)) << ","
            << (obj.game_index == INT_MAX ? "" : std::to_string(obj.game_index)) << std::endl;
    }
}

void print_pokemon_types_db(std::vector<pokemon_types_db>* db_vector) {
    for (const pokemon_types_db& obj : *db_vector) {
        std::cout << (obj.pokemon_id == INT_MAX ? "" : std::to_string(obj.pokemon_id)) << ","
            << (obj.type_id == INT_MAX ? "" : std::to_string(obj.type_id)) << ","
            << (obj.slot == INT_MAX ? "" : std::to_string(obj.slot)) << std::endl;
    }   
}