#include "pokemon.h"
#include "database.h"
#include <cmath>
#include "characters_players.h"
#include <ncurses.h>
#include <unistd.h>
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

    for (int i = 0; i < pokemon_stats_size; i++) {
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
            // Eliminate dublicate moves
            bool duplicate = false;

            for (int i = 0; i < potential_moves_id.size(); i++) {
                if (move.move_id == potential_moves_id[i]) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate == false) {
                potential_moves_id.push_back(move.move_id);
            } 
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

// Select pokemons at the start of the game
int select_initial_pokemon(Player *pc, Database *database) {
    mvprintw(0, 0, "Who are your initial pokemons? Press 1, 2 or 3 to select:");

    
    
    // Create 3 pokemons
    int new_level = 1;

    int random_row1 = rand() % database->pokemon.size();
    int random_row2 = rand() % database->pokemon.size();
    int random_row3 = rand() % database->pokemon.size();

    int id1_from_row = database->pokemon[random_row1].species_id;
    int id2_from_row = database->pokemon[random_row2].species_id;
    int id3_from_row = database->pokemon[random_row3].species_id;


    Pokemon poke1(new_level, id1_from_row, database);
    // poke1.identifier = "DEBUG";
    Pokemon poke2(new_level, id2_from_row, database);
    Pokemon poke3(new_level, id3_from_row, database);

    // Tablichka s nomerom
    mvprintw(3, 2, "Pokemon 1:");
    mvprintw(3, 25, "Pokemon 2:");
    mvprintw(3, 48, "Pokemon 3:");
    
    // Row with names
    mvprintw(5, 2, "Name: %s", poke1.identifier.c_str());
    mvprintw(5, 25, "Name: %s", poke2.identifier.c_str());
    mvprintw(5, 48, "Name: %s", poke3.identifier.c_str());

    // Loading bar variables
    int i = 0;
    char a[5] = {'\\','|','/','-'};

    int pokemon_selected = 0;

    nodelay(stdscr, TRUE);
    char input;

    while (!pokemon_selected) {
        usleep(125000);
        mvaddch(1, i / 4, a[i%4]);
        refresh();

        while ((input = getch()) != ERR) {
            if (input == '1') {
                pc->available_pokemons.push_back(poke1);
                pokemon_selected = 1;
            } else if (input == '2') {
                pc->available_pokemons.push_back(poke2);
                pokemon_selected = 1;
            } else if (input == '3') {
                pc->available_pokemons.push_back(poke3);
                pokemon_selected = 1;
            }
        }

        i++;

        if (i / 4 >= MAP_WIDTH) {
            i = 0;
            move(1, 0);
            clrtoeol();
        }
    }

    nodelay(stdscr, FALSE);
    clear();

    return 0;
}