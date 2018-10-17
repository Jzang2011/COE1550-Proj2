//
// Created by Jeremy Zang on 10/16/18.
//

#include <linux/spinlock.h> //added for cs1550
#include <linux/spinlock_types.h> //added for cs1550
#include <linux/cs1550_queue.h>
//#include <sched.h>

/**
 * Car struct.
 */
typedef struct Car {
    int car_id;
    //Add arrival time.?
} Car;

/**
 * Semaphore for cs1550
 */
struct cs1550_sem {
    int value;
    spinlock_t sem_lock;
    struct process_queue_type process_queue;

    //queue of processes.
    //use cs1550_queue.h here.
//    struct task_struct process_queue[5];
//    int head = 0;
//    int tail = 0;
//    int count = 0;

    //This will replace the queue above.
    //DECLARE_QUEUE(process_queue, process_queue_type)
};