#include "min_heap.h"
#include "terrain_generator.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

void initialize_min_heap(struct min_heap *min_heap) {
    min_heap->current_size = 0;
    min_heap->max_capacity = MAP_HEIGHT * MAP_WIDTH;

    min_heap->heap_array = malloc((sizeof (struct heap_node)) * MAP_HEIGHT * MAP_WIDTH);

    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            min_heap->position_lookup[i][j] = -1;
        }
    }
}

void delete_heap(struct min_heap *min_heap) {
    free(min_heap->heap_array);
}

void heapify_up(struct min_heap *min_heap, int this_index) {
    while (this_index > 0) {
        // Get current node by its this_index in the heap_array
        struct heap_node this_node = min_heap->heap_array[this_index];

        // Get parent node in the same way, find this_index beforehand
        int parent_node_index = (this_index - 1) / 2;
        struct heap_node parent_node = min_heap->heap_array[parent_node_index];

        // Swap parent and child node if child`s path_cost is smaller  
        if (this_node.path_cost < parent_node.path_cost) {
            min_heap->heap_array[this_index] = parent_node;
            min_heap->heap_array[parent_node_index] = this_node;

            // Update lookup table.
            min_heap->position_lookup[this_node.y][this_node.x] = parent_node_index;
            min_heap->position_lookup[parent_node.y][parent_node.x] = this_index;

            this_index = parent_node_index;
        } else {
            break;
        }
    }
}

void insert_node(struct min_heap *min_heap, struct heap_node new_node) {
    // Check if we are not out of capacity
    if (min_heap->current_size >= min_heap->max_capacity) {
        return;
    }

    // Place new node into the heap array
    min_heap->heap_array[min_heap->current_size] = new_node;

    // Initialize its index in our index lookup table
    min_heap->position_lookup[new_node.y][new_node.x] = min_heap->current_size;

    // Increase number of elements in the heap
    min_heap->current_size++;
    
    // Restore min_heap property. Send initial index, not increased by 1.
    heapify_up(min_heap, min_heap->current_size - 1);
}

void heapify_down(struct min_heap *min_heap, int index) {
    while (index <= min_heap->current_size - 1) {
        // Indexes of both childs in the heap_array and pointers to them
        int right_child_index = index * 2 + 2;
        int left_child_index = index * 2 + 1;

        struct heap_node *right_child_pointer;
        struct heap_node *left_child_pointer;

        // Check if both child exist and figure out who has smaller path_cost
        int right_cost, left_cost;

        // Check left child
        if (left_child_index < min_heap->current_size) {
            left_child_pointer = min_heap->heap_array + left_child_index;
            left_cost = left_child_pointer->path_cost;
        } else {
            left_cost = INT_MAX;
            left_child_pointer = NULL;
        }

        // Check right child
        if (right_child_index < min_heap->current_size) {
            right_child_pointer = min_heap->heap_array + right_child_index;
            right_cost = right_child_pointer->path_cost;
        } else {
            right_cost = INT_MAX;
            right_child_pointer = NULL;
        }

        // If both are NULL, we are at a leaf node
        if (left_child_pointer == NULL && right_child_pointer == NULL) {
            break;
        }

        // Determine who is smaller, right or left child
        struct heap_node *smaller_child;
        int smaller_index;


        if (left_cost < right_cost) {
            smaller_child = left_child_pointer;
            smaller_index = left_child_index;
        } else {
            smaller_child = right_child_pointer;
            smaller_index = right_child_index;
        }

        // Swap nodes until not at the bottom
        if ((min_heap->heap_array + index)->path_cost > smaller_child->path_cost) {
            //If we are here, then parent's path_cost was higher than the child's. Parent has to bubble down.
            struct heap_node current_node = min_heap->heap_array[index]; // This holds current node as temporary storage

            // Swap the nodes
            min_heap->heap_array[index] = *smaller_child;
            min_heap->heap_array[smaller_index] = current_node;

            // Update lookup table
            min_heap->position_lookup[current_node.y][current_node.x] = smaller_index;
            min_heap->position_lookup[smaller_child->y][smaller_child->x] = index;

            // Update index
            index = smaller_index;
        } else {
            break;
        }
    }  
}

void extract_min(struct min_heap *min_heap, struct heap_node *root_heap_node) {
    // Save root at the pointer's address
    // Below is de-sugared version of: *root_heap_node = min_heap->heap_array[0]; 
    memcpy(root_heap_node, min_heap->heap_array + 0, sizeof (struct heap_node));

    // Update lookup table for the root we are removing.
    min_heap->position_lookup[root_heap_node->y][root_heap_node->x] = -1;

    // Move the last node in the heap array to the root. Sugar and de-sugar:
    // struct heap_node last_node = min_heap->heap_array[min_heap->current_size - 1];
    // min_heap->heap_array[0] = last_node;
    memcpy(min_heap->heap_array + 0, min_heap->heap_array + min_heap->current_size - 1, sizeof (struct heap_node));

    // Decrease size. Last node becomes zombie to be overwritten later by some other node.
    min_heap->current_size--;

    // Check if we did not extract the very last node. There is nothing to do if heap is empty.
    if (min_heap->current_size > 0) {
        min_heap->position_lookup[(min_heap->heap_array + 0)->y][(min_heap->heap_array + 0)->x] = 0;
        
        heapify_down(min_heap, 0);
    }
}

void decrease_priority(int x, int y, int new_path_cost, struct min_heap *min_heap) {
    int node_array_index = min_heap->position_lookup[y][x];

    if (node_array_index == -1) {
        return;
    }

    // Sugar version of below: min_heap->heap_array[node_array_index].path_cost = new_path_cost;
    (min_heap->heap_array + node_array_index)->path_cost= new_path_cost;

    heapify_up(min_heap, node_array_index);
}