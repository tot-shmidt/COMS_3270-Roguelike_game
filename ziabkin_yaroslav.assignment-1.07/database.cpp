#include "database.h"
#include <iostream>
#include <sstream>


// ~~~ PARSING FUNCTIONS ~~~
std::vector<pokemon_db> parse_pokemon(std::ifstream* csv_file) {
    // Vector to store pokemon_db objects
    std::vector<pokemon_db> pokemon_db_objects = std::vector<pokemon_db>();

    if (csv_file == NULL) { return pokemon_db_objects; }

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

        // Push final db_object to the back of the
        pokemon_db_objects.push_back(db_object);
    }

    return pokemon_db_objects;
}

std::vector<moves_db> parse_moves(std::ifstream* csv_file) {
    std::vector<moves_db> ;
}

// ~~~ PRINTING FUNCTIONS ~~~
void print_pokemon_db(std::vector<pokemon_db>* db_vector) {
    for (pokemon_db obj : *db_vector) {
        std::cout << obj.id << ", " << obj.identifier << ", " << obj.species_id << ", " << obj.height << ", " << obj.weight << ", " << obj.base_experience << ", " << obj.order << ", " << obj.is_default << std::endl;
    }
}

void print_moves_db(std::vector<moves_db>* db_vector) {

}