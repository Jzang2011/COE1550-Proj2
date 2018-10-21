//
// Created by Jeremy Zang on 10/17/18.
//

#ifndef PROJ2_CS1550_QUEUE_H
#define PROJ2_CS1550_QUEUE_H

struct process_queue_type {
    void *buffer[4];
    int head;
    int tail;
    int count;
    int queue_size;
};

int is_full_process(struct process_queue_type *queue) {
    if ((queue->head == queue->tail) && (queue->count == queue->queue_size)) {
        return 1;
    }
    return 0;
}

int is_empty_process(struct process_queue_type *queue) {
    if ((queue->head == queue->tail) && (queue->count == 0)) {
        return 1;
    }
    return 0;
}

void enqueue_process(struct process_queue_type *queue, void *item) {
    queue->buffer[queue->tail] = item;
    queue->tail++;
    queue->count++;
}

void *dequeue_process(struct process_queue_type *queue) {
    if (!is_empty_process(queue)) {
        void *item = queue->buffer[queue->head];
        queue->head++;
        queue->count--;
        return item;
    }
    return 0;
}

#endif //PROJ2_CS1550_QUEUE_H