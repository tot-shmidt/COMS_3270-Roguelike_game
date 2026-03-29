#include "terrain_generator.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void display_map(struct map *current_map) {
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            printf("%c", current_map->grid_array[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    struct map current_map;
    create_map(&current_map);
    display_map(&current_map);
    return 0;
}