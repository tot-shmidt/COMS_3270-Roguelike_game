#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "terrain_generator.h"

struct heap_node {
    int path_cost;  // Distance from PC to the current cell at the given x and y.

    int x;
    int y;
};

struct min_heap {
    int current_size;
    int max_capacity;

    struct heap_node *heap_array;

    // index of (x, y) in the array above.
    int position_lookup[MAP_HEIGHT][MAP_WIDTH];

};

void initialize_min_heap(struct min_heap *min_heap);
void delete_heap(struct min_heap *min_heap);
void insert_node(struct min_heap *min_heap, struct  heap_node new_node);
void extract_min(struct min_heap *min_heap, struct heap_node *root_heap_node);
void decrease_priority(int x, int y, int new_path_cost, struct min_heap *min_heap);

#endif