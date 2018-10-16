#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> //this should use the one in linux-2.6.23.1

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

//PID's for processes.
int flag_person_pid;
int north_bound_pid;
int south_bound_pid;

Car south_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};
Car north_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};

Car* north_bound_first;
Car* south_bount_first;
Car* north_bound_last;
Car* south_bount_last;
int north_bound_size;
int south_bound_size;


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
    syscall(__NR_cs1550_down, sem);
}

/**
 * Used to call our modified syscall in the linux kernel to up our semaphore.
 * @param sem - the semaphore being up'ed
 */
void up(cs1550_sem *sem) {
    syscall(__NR_cs1550_up, sem);
}


/**
 * Used to initialize the simulaiton
 *  - Initializes 2 producers (Northbound  and Southbound)
 *  - Initializes 1 consumer (flagperson)
 */
void init_sim() {
    //Initialize memory space

    //Initialize processes.
    flag_person_pid = fork();
    south_bound_pid = fork();
    north_bound_pid = fork();
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

void delay_20_sec() {
    //once no car comes, there is a 20 second delay before any new car will come.
    //sleep for 20 seconds to simulate this.
    sleep(20);
}

void let_car_through() {
    //Each car takes 2 seconds to go through the construction area.
    //Sleep for 2 seconds to simulate this.
    sleep(2);
}

void wake_up_flagperson() {
    //car honks horn and wakes up flag person

}

/**
 * Gets the current system time.
 *  use timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec to get hours, minutes and seconds.
 * @return A struct tm* which represents the current local system time.
 */
struct tm* get_time() {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
//    printf ("Current local time and date: %d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return timeinfo;
}


int main(int argc, char **argv) {
    srand(time(NULL));

    //Start by initializing the simulation.
     init_sim();

    //Gets the current process ID. This dictates what code to run (producer or consumer)
    int current_process = getpid();

    if (current_process == flag_person_pid) {
        //Consumer code
        // allow cars to travel (consume)
        // call newly created up()
        // Do more stuff
        // call newly created down()
    } else if (current_process == south_bound_pid) {
        //Producer code
        //generate cars
        // call newly created up()
        // Do more stuff
        // add to southbound_queue
        // call newly created down()
    } else if (current_process == north_bound_pid) {
        //Producer code
        //generate cars
        // call newly created up()
        // Do more stuff
        // add to north_bound_queue
        // call newly created down()
    }


    return 0;
}