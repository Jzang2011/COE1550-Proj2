#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
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
int* flag_person_pid = NULL;
int* north_bound_pid = NULL;
int* south_bound_pid = NULL;

//Car south_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};
//Car north_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};
Car* south_bound_queue;
Car* north_bound_queue;


Car* north_bound_first;
Car* south_bount_first;
Car* north_bound_last;
Car* south_bount_last;
int* north_bound_size;
int* south_bound_size;
struct cs1550_sem* sem;


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
 * Calculates the size of the memory needed to be mapped.
 * @return N - the number of bytes.
 */
int calculate_mem_size() {
    int N = 0;
    N = N + (sizeof(int) * 5); //add the size of 5 ints (3 for process ids and 2 for size of queues)
    N = N + (sizeof(Car) * 24); // 10 for each queue (20 total) + 2 for the head and tail pointers (4 total)
    N = N + sizeof(cs1550_sem);
    return N;
}

void init_ptrs(void* ptr_to_mem) {
    int sizeOfCar = sizeof(Car);
    sem = ptr_to_mem;
    south_bound_size = ptr_to_mem + sizeof(cs1550_sem);
    north_bound_size = south_bound_size + 1;
    flag_person_pid = north_bound_size + 1;
    north_bound_pid = flag_person_pid + 1;
    south_bound_pid = north_bound_pid + 1;
    north_bound_first = (Car *) (south_bound_pid + sizeOfCar);
    north_bound_last = north_bound_first + sizeOfCar;
    south_bount_first = north_bound_last + sizeOfCar;
    south_bount_last = south_bount_first + sizeOfCar;
    south_bound_queue = south_bount_last + sizeOfCar;
    north_bound_queue = south_bound_queue + (sizeOfCar * 10);
}

/**
 * Used to initialize the simulaiton
 *  - Initializes 2 producers (Northbound  and Southbound)
 *  - Initializes 1 consumer (flagperson)
 */
void init_sim() {
    //Initialize memory space
    int N = calculate_mem_size();
    void* ptr = mmap(NULL, N, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
    init_ptrs(ptr);
    //Initialize processes.
    *flag_person_pid = fork();
    *south_bound_pid = fork();
    *north_bound_pid = fork();
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
    //Remove car from either Northbound or Southbound queues.
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
        //Southbound Producer code
        //generate cars
        // call newly created up()
        // Do more stuff
        // add to southbound_queue
        // call newly created down()
    } else if (current_process == north_bound_pid) {
        //Northbound Producer code
        //generate cars
        // call newly created up()
        // Do more stuff
        // add to north_bound_queue
        // call newly created down()
    }


    return 0;
}