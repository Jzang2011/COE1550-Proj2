//
// Created by Jeremy Zang on 10/16/18.
//

#include <linux/spinlock.h> //added for cs1550
#include <linux/spinlock_types.h> //added for cs1550
#include <linux/cs1550_queue.h>

/**
 * Car struct.
 */
typedef struct Car {
    int car_id;
    struct tm* timeinfo;
    //Add arrival time.?
} Car;

/**
 * Semaphore for cs1550
 */
struct cs1550_sem {
    int value;
    spinlock_t sem_lock;

    //queue of processes.
    struct process_queue_type process_queue;
};