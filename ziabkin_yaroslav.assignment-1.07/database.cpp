#include "database.h"
#include <iostream>
#include <sstream>
#include <limits.h>


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