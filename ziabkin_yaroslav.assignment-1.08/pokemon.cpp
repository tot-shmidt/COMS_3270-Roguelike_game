#include "pokemon.h"
#include "database.h"
#include <cmath>
// #include <iterator>

Pokemon::Pokemon(int p_level, int p_species_id, struct Database *database) {
    this->current_level = p_level;
    this->species_id = p_species_id;

    this->gender = rand() % 2;
    this->shiny = (rand() % 8192 == 0);

    // get identifier
    int pokemon_size = database->pokemon.size();
    for (int i = 0; i < pokemon_size; i++) {
        pokemon_db &record = database->pokemon.at(i);

        if (record.species_id == species_id) {
            identifier = record.identifier;
            break;
        }
    }

    // get base stats
    int pokemon_stats_size = database->pokemon_stats.size();
    int assigned_stats = 0;

    for (int i = 0; i < pokemon_stats_size; i = i + 5) {
        pokemon_stats_db &record = database->pokemon_stats.at(i);

        if (record.pokemon_id == species_id) {
            base_stats[record.stat_id - 1] = record.base_stat;

            assigned_stats++;

            if (assigned_stats == 6) {
                break;
            }
        }
    }

    // generate ivs 
    for (int i = 0; i < std::size(ivs); i++) {
        ivs[i] = rand() % 16;
    }

    // calculate actual_stats.
    int hp = (((base_stats[0] + ivs[0]) * 2) * current_level) / 100 + current_level + 10; // does it have to be this->current_level ?
    actual_stats[0] = hp;

    for (int i = 1; i < std::size(ivs); i++) {
        int stat = (((base_stats[i] + ivs[i]) * 2) * current_level) / 100 + 5;
        actual_stats[i] = stat;
    }

    // find correct moves:
    
    // Select potential moves 
    std::vector<int> potential_moves_id;
    for (pokemon_moves_db &move : database->pokemon_moves) {
        if (move.pokemon_id == species_id && move.pokemon_move_method_id == 1 && move.level <= current_level) {
            potential_moves_id.push_back(move.move_id);
        }
    }

    // Choose up to two moves from potential moves
    std::vector<int> chosen_moves_id;

    if (potential_moves_id.size() == 0) {
        move_set.push_back("struggle");
    } else if (potential_moves_id.size() == 1) {
        chosen_moves_id.push_back(potential_moves_id[0]);
    } else {
        int first = rand() % potential_moves_id.size();
        int second;

        do {
            second = rand() % potential_moves_id.size();
        } while (first == second);

        chosen_moves_id.push_back(potential_moves_id[first]);
        chosen_moves_id.push_back(potential_moves_id[second]);
    }

    // Find identifier of each id we found
    for (int chosen_id : chosen_moves_id) {
        for (moves_db &move : database->moves) {
            if (move.id == chosen_id) {
                move_set.push_back(move.identifier);
                break;
            }
        }
    }
}