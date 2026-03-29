#include "terrain_generator.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void display_map(struct map *current_map, struct world *this_world) {
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            printf("%c", current_map->grid_array[i][j]);
        }
        printf("\n");
    }

    printf("Current coordinates: (%d, %d)\n", this_world->current_map_x - 200, this_world->current_map_y - 200);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // 1. Create world and first map.
    struct world this_world;
    struct map first_map;
    initialize_world(&this_world, &first_map);
    create_map(&first_map, &this_world);

    display_map(&first_map, &this_world);
    

    // 2. Game loop
    int game_continues = 1;

    do {
        // a. Take input from the user.
        printf("Enter command: ");

        char command;
        scanf(" %c", &command);

        // b. Validate input, calculate new place.
        int next_map_y = this_world.current_map_y;
        int next_map_x = this_world.current_map_x;
        int fly_y, fly_x;

        switch (command) {
            case 'n':
                next_map_y = this_world.current_map_y - 1; break;
            case 'e':
                next_map_x = this_world.current_map_x + 1; break;
            case 's':
                next_map_y = this_world.current_map_y + 1; break;
            case 'w':
                next_map_x = this_world.current_map_x - 1; break;
            case 'f':
                scanf(" %d %d", &fly_x, &fly_y);
                next_map_x = 200 + fly_x;
                next_map_y = 200 + fly_y;
                break;
            case 'q':
                game_continues = 0;
                continue;
            default:
                printf("Uncorrect input!\n");
                continue;
        }

        if (next_map_x < 0 || next_map_y < 0 || next_map_x >= WORLD_WIDTH || next_map_y >= WORLD_HEIGHT) {
            printf("Moving out off bounds of the world!\n");
        } else {
            this_world.current_map_y = next_map_y;
            this_world.current_map_x = next_map_x;

            // If map does not exist.
            if (this_world.world_maps[next_map_y][next_map_x] == NULL) {
                this_world.world_maps[next_map_y][next_map_x] = malloc(sizeof(struct map));
                create_map(this_world.world_maps[next_map_y][next_map_x], &this_world);
            }

            display_map(this_world.world_maps[next_map_y][next_map_x], &this_world);
        }
    } while (game_continues);

    // 3. Exit game
    return 0;
}