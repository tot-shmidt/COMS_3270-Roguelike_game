#include "moves_queue.h"
#include "terrain_generator.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

void initialize_moves_queue(struct moves_queue *queue, int total_entities) {
    queue->current_size = 0;
    queue->max_capacity = total_entities;
    queue->queue_array = (struct queue_node*) malloc((sizeof (struct queue_node)) * total_entities);
}

void delete_moves_queue(struct moves_queue *queue) {
    free(queue->queue_array);
}

void mq_heapify_up(struct moves_queue *queue, int this_index) {
    while (this_index > 0) {
        // Get current node by its this_index in the queue_array
        struct queue_node this_node = queue->queue_array[this_index];

        // Get parent node in the same way, find this_index beforehand
        int parent_node_index = (this_index - 1) / 2;
        struct queue_node parent_node = queue->queue_array[parent_node_index];

        // Swap parent and child node if child`s current_time is smaller  
        if (this_node.current_time < parent_node.current_time) {
            queue->queue_array[this_index] = parent_node;
            queue->queue_array[parent_node_index] = this_node;

            this_index = parent_node_index;
        } else {
            break;
        }
    }
}

void mq_insert_node(struct moves_queue *queue, struct queue_node new_node) {
    // Check if we are not out of capacity
    if (queue->current_size >= queue->max_capacity) {
        return;
    }

    // Place new node into the heap array
    queue->queue_array[queue->current_size] = new_node;

    // Increase number of elements in the heap
    queue->current_size++;
    
    // Restore queue property. Send initial index, not increased by 1.
    mq_heapify_up(queue, queue->current_size - 1);
}

void mq_heapify_down(struct moves_queue *queue, int index) {
    while (index <= queue->current_size - 1) {
        // Indexes of both childs in the queue_array and pointers to them
        int right_child_index = index * 2 + 2;
        int left_child_index = index * 2 + 1;

        struct queue_node *right_child_pointer;
        struct queue_node *left_child_pointer;

        // Check if both child exist and figure out who has smaller current_time
        int right_cost, left_cost;

        // Check left child
        if (left_child_index < queue->current_size) {
            left_child_pointer = queue->queue_array + left_child_index;
            left_cost = left_child_pointer->current_time;
        } else {
            left_cost = INT_MAX;
            left_child_pointer = NULL;
        }

        // Check right child
        if (right_child_index < queue->current_size) {
            right_child_pointer = queue->queue_array + right_child_index;
            right_cost = right_child_pointer->current_time;
        } else {
            right_cost = INT_MAX;
            right_child_pointer = NULL;
        }

        // If both are NULL, we are at a leaf node
        if (left_child_pointer == NULL && right_child_pointer == NULL) {
            break;
        }

        // Determine who is smaller, right or left child
        struct queue_node *smaller_child;
        int smaller_index;


        if (left_cost < right_cost) {
            smaller_child = left_child_pointer;
            smaller_index = left_child_index;
        } else {
            smaller_child = right_child_pointer;
            smaller_index = right_child_index;
        }

        // Swap nodes until not at the bottom
        if ((queue->queue_array + index)->current_time > smaller_child->current_time) {
            //If we are here, then parent's current_time was higher than the child's. Parent has to bubble down.
            struct queue_node current_node = queue->queue_array[index]; // This holds current node as temporary storage

            // Swap the nodes
            queue->queue_array[index] = *smaller_child;
            queue->queue_array[smaller_index] = current_node;

            // Update index
            index = smaller_index;
        } else {
            break;
        }
    }  
}

void mq_extract_min(struct moves_queue *queue, struct queue_node *min_node) {
    // Save root at the pointer's address
    // Below is de-sugared version of: *root_queue_node = min_heap->heap_array[0]; 
    memcpy(min_node, queue->queue_array + 0, sizeof (struct queue_node));

    // Move the last node in the heap array to the root. Sugar and de-sugar:
    // struct queue_node last_node = queue->queue_array[queue->current_size - 1];
    // queue->queue_array[0] = last_node;
    memcpy(queue->queue_array + 0, queue->queue_array + queue->current_size - 1, sizeof (struct queue_node));

    // Decrease size. Last node becomes zombie to be overwritten later by some other node.
    queue->current_size--;

    // Check if we did not extract the very last node. There is nothing to do if heap is empty.
    if (queue->current_size > 0) {
        
        mq_heapify_down(queue, 0);
    }
}