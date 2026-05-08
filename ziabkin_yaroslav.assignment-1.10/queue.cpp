#include "queue.h"
#include <stdio.h>

int queue_init(struct queue *q) {
    q->size = 0;
    q->head = 0;
    q->tail = 0;

    return 0;
}

int queue_enqueue(struct queue *q, int x, int y) {
    q->queue_array[q->tail].x_coordinate = x;
    q->queue_array[q->tail].y_coordinate = y;

    q->tail++;
    q->size++;

    return 0;
}

int queue_dequeue(struct queue *q, int *x, int *y) {
    if (q->size == 0) {
        return -1;
    }

    if (x != NULL && y != NULL) {
        *x = q->queue_array[q->head].x_coordinate;
        *y = q->queue_array[q->head].y_coordinate;
    }

    q->size--;
    q->head++;

    return 0;
}

int queue_has_items(struct queue *q) {
    if (q->size == 0) {
        return 0;
    } else {
        return 1;
    }
}

int queue_destroy(struct queue *q) {
    q->size = 0;
    q->head = 0;
    q->tail = 0;
    
    return 0;
}

