//
// Created by Jeremy Zang on 10/16/18.
//

#include <include/linux/spinlock.h> //added for cs1550
#include <include/linux/spinlock_types.h> //added for cs1550
#include <sched.h>

/**
 * Car struct.
 */
typedef struct Car {
    int car_id;
} Car;

/**
 * Semaphore for cs1550
 * Should this be here or in its own header file?
 */
struct cs1550_sem {
    int value;
    spinlock_t sem_lock;
    //add queue here. each instance of a cs1550_sem will have queue. This will be instantiated in trafficsim.c
    // and specifying what the queue instance will be.
    int car; //Replace with a queue later after testing up() and down()
};