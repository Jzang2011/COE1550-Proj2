//
// Created by Jeremy Zang on 10/17/18.
//

#ifndef PROJ2_CS1550_QUEUE_H
#define PROJ2_CS1550_QUEUE_H

#define PROCESS_QUEUE_SIZE 3

struct process_queue_type {
    struct task_struct *buffer[PROCESS_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    int queue_size;
};

/**
 * Checks to see if the process queue is full.
 * @param queue - the queue to be checked.
 * @return - 1 if the queue is full and 0 otherwise. 
 */
int is_full_process(struct process_queue_type *queue) {
    if ((queue->head == queue->tail) && (queue->count == queue->queue_size)) {
        return 1;
    }
    return 0;
}

/**
 * Checks to see if the process queue is empty.
 * @param queue - the queue to be checked.
 * @return - 1 for empty and 0 otherwise.
 */
int is_empty_process(struct process_queue_type *queue) {
    if ((queue->head == queue->tail) && (queue->count == 0)) {
        return 1;
    }
    return 0;
}

/**
 * Enqueues a process in a process queue.
 * @param queue - the queue to add to.
 * @param item - the item being added.
 */
void enqueue_process(struct process_queue_type *queue, void *item) {
    queue->buffer[queue->tail % PROCESS_QUEUE_SIZE] = item;
    queue->tail = (queue->tail + 1) % PROCESS_QUEUE_SIZE;
    queue->count++;
}

/**
 * Dequeues from a process queue.
 * @param queue - to dequeue from
 * @return - a void* to the item or zero if queue is empty.
 */
void *dequeue_process(struct process_queue_type *queue) {
    if (!is_empty_process(queue)) {
        void *item = queue->buffer[queue->head % PROCESS_QUEUE_SIZE];
        queue->head = (queue->head + 1) % PROCESS_QUEUE_SIZE;
        queue->count--;
        return item;
    }
    return 0;
}

/**
 * Initializes a process queue structures variables.
 * @param queue to be initialized.
 */
void init_process_queue(struct process_queue_type *queue) {
    queue->tail = 0;
    queue->head = 0;
    queue->count = 0;
    queue->queue_size = PROCESS_QUEUE_SIZE;
}

#endif //PROJ2_CS1550_QUEUE_H