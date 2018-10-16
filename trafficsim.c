#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * 1. Treat the road as two queues, and have a producer for each direction putting cars into the
 *    queues at the appropriate times.
 * 2. Have a consumer (flagperson) that allows cars from one direction to pass through the work area
 *    as described above.
 * 3. To get an 80% chance of something, you can generate a random number modulo 10, and see if
 *    its value is less than 8. It’s like flipping an unfair coin.
 * 4. Use the syscall nanosleep() or sleep() to pause your processes
 * 5.
 */

/**
 * Car struct.
 */
typedef struct Car {
    int car_id;
} Car;

int flag_person;
Car south_bound_producer[10] = {0,0,0,0,0,0,0,0,0,0};
Car north_bound_producer[10] = {0,0,0,0,0,0,0,0,0,0};

Car* north_bound_first;
Car* south_bount_first;



/**
 * Should this go in kernel?
 */
typedef struct cs1550_sem {
    int value;
    //Some queue goes here? 10/15/18
} cs1550_sem;

/**
 * Used to call our modified syscall in the linux kernel to down our semaphore.
 * @param sem - the semaphore being down'ed
 */
void down(cs1550_sem *sem) {
    //syscall(__NR_cs1550_down, sem);
}

/**
 * Used to call our modified syscall in the linux kernel to up our semaphore.
 * @param sem - the semaphore being up'ed
 */
void up(cs1550_sem *sem) {
    //syscall(__NR_cs1550_up, sem);
}


/**
 * Used to initialize the simulaiton
 *  - Initializes 2 producers (Northbound  and Southbound)
 *  - Initializes 1 consumer (flagperson)
 */
void init_sim() {
    //Initialize memory space

    //Initialize processes.
    flag_person = fork();
    south_bound_producer = fork();
    north_bound_producer = fork();
}

/**
 * Generates the chance there is a car following another.
 * @return Returns 1 for positive (true) chancel
 * Returns 0 for negative (false).
 */
int chance_80(){
    int r = rand() % 10;
    if (r < 8) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char **argv) {
//    srand(time(NULL));
//    printf("chance_80 returned %d\n", chance_80());
//    printf("chance_80 returned %d\n", chance_80());
//    printf("chance_80 returned %d\n", chance_80());
//    printf("chance_80 returned %d\n", chance_80());
//    printf("chance_80 returned %d\n", chance_80());

    //Start by initializing the simulation.
    init_sim();

    return 0;
}