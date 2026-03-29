#ifndef MOVES_QUEUE_H
#define MOVES_QUEUE_H

#include "characters_players.h"

struct queue_node {
    int current_time;

    struct entity *entity;
};

struct moves_queue {
    int current_size;
    int max_capacity;

    struct queue_node *queue_array;
};

void initialize_moves_queue(struct moves_queue *queue, int total_entities);
void delete_moves_queue(struct moves_queue *queue);
void mq_insert_node(struct moves_queue *queue, struct queue_node new_node);
void mq_extract_min(struct moves_queue *queue, struct queue_node *min_node);

#endif