//
// Created by Jeremy Zang on 10/17/18.
//

#ifndef PROJ2_CS1550_QUEUE_H
#define PROJ2_CS1550_QUEUE_H

//#define DEFINE_QUEUE(name, size)

    struct car_queue_type {
        void *buffer[10];
        int head;
        int tail;
        int count;
        int queue_size;
    };

    int is_full(struct car_queue_type *queue) {
        if ((queue->head == queue->tail) && (queue->count == queue->queue_size)) {
            return 1;
        }
        return 0;
    }

    int is_empty(struct car_queue_type *queue) {
        if ((queue->head == queue->tail) && (queue->count == 0)) {
            return 1;
        }
        return 0;
    }

    void enqueue(struct car_queue_type *queue, void *item) {
        queue->buffer[queue->tail] = item;
        queue->tail++;
        queue->count++;
    }

    void *dequeue(struct car_queue_type *queue) {
        if (!is_empty(queue)) {
            void *item = queue->buffer[queue->head];
            queue->head++;
            queue->count--;
            return item;
        }
        return 0;
    }

struct process_queue_type {
    void *buffer[10];
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


//#define DECLARE_QUEUE(queue_name, type_name) struct type_name queue_name;

//DEFINE_QUEUE(car_queue_type, 10)

//DEFINE_QUEUE(process_queue_type, 20)

#endif //PROJ2_CS1550_QUEUE_H