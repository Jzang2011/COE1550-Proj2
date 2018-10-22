//
// Created by Jeremy Zang on 10/16/18.
//

#include <linux/cs1550_queue.h>

/**
 * Semaphore for cs1550
 */
struct cs1550_sem {
    int value;

    //queue of processes.
    struct process_queue_type process_queue;
};

