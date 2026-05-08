#ifndef QUEUE_H
#define QUEUE_H

struct queue_item {
    int x_coordinate;
    int y_coordinate;
};

struct queue {
    int head;
    int tail;
    int size;
    struct queue_item queue_array[1700];
};

int queue_init(struct queue *q);
int queue_destroy(struct queue *q);
int queue_enqueue(struct queue *q, int x, int y);
int queue_dequeue(struct queue *q, int *x, int *y);
int queue_has_items(struct queue *q);

#endif