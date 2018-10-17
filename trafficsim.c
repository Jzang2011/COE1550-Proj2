#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h> //this should use the one in linux-2.6.23.1
#include <include/linux/spinlock_types.h>
#include <include/linux/spinlock.h>
#include <kernel/cs1550_sem.h>

#include "linux-2.6.23.1/include/asm/unistd.h"
#include "linux-2.6.23.1/kernel/cs1550_sem.h"
#include "linux-2.6.23.1/include/linux/spinlock_types.h"

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

//PID's for processes.
int* flag_person_pid = NULL;
int* north_bound_pid = NULL;
int* south_bound_pid = NULL;

//TODO: make array for the queue inside the cs1550_sem struct.
//Car south_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};
//Car north_bound_queue[10] = {0,0,0,0,0,0,0,0,0,0};
int* south_bound_queue;
int* north_bound_queue;

int* north_bound_first;
int* south_bount_first;
int* north_bound_last;
int* south_bount_last;
int* north_bound_size;
int* south_bound_size;

struct cs1550_sem sem;
int car_count = 0;

void add_to_queue(Car* queue) {

}

void remove_from_queue(Car* queue) {

}

int is_queue_empty(Car* queue) {

}

/**
 * Used to call our modified syscall in the linux kernel to down our semaphore.
 * @param sem - the semaphore being down'ed
 */
void down(struct cs1550_sem *sem) {
    syscall(__NR_cs1550_down, sem);
}

/**
 * Used to call our modified syscall in the linux kernel to up our semaphore.
 * @param sem - the semaphore being up'ed
 */
void up(struct cs1550_sem *sem) {
    syscall(__NR_cs1550_up, sem);
}

/**
 * Calculates the size of the memory needed to be mapped.
 * @return N - the number of bytes.
 */
int calculate_mem_size() {
    int N = 0;
    N = N + (sizeof(int) * 5); //add the size of 5 ints (3 for process ids and 2 for size of queues)
    N = N + (sizeof(int) * 24); // 10 for each queue (20 total) + 2 for the head and tail pointers (4 total)

    return N;
}

void init_ptrs(void* ptr_to_mem) {
    int sizeOfCar = sizeof(int);
    south_bound_size = ptr_to_mem;
    north_bound_size = south_bound_size + 1;
    flag_person_pid = north_bound_size + 1;
    north_bound_pid = flag_person_pid + 1;
    south_bound_pid = north_bound_pid + 1;
    north_bound_first = south_bound_pid + sizeOfCar;
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
    init_ptrs(ptr); //initializes the global pointers.
    DEFINE_SPINLOCK(sem.sem_lock); //initializes the semaphores lock with a spin lock.
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

/**
 * Gets key from user.
 * Note: Reused from project 1.
 * @return - The key pressed by the user.
 */
char getkey() {
    char key = 0;
    int fD = 0; //fD is the file descriptor. 0 for keyboard input.
    int nfds = 1; //number of file descriptors. 1 since we are only worried about keyboard input.
    fd_set fs; //declare a fd_set named fs

    FD_ZERO(&fs);
    FD_SET(fD, &fs);

    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;

    //int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    int numberOfKeysReady = select(nfds, &fs, NULL, NULL, &time); //Look up man pages for select(2)

    //if numberOfKeysReady == 1 read from the file descriptor
    // use read sys call to get the key pressed
    if (numberOfKeysReady > 0)
    {
        //ssize_t read(int fd, void *buf, size_t count);
        read(fD, &key, 1);
    }
    return key; //value of key is the decimal value of the character. See ascii table.
}

int main(int argc, char **argv) {
    srand(time(NULL));
    char key = getkey();

    //Start by initializing the simulation.
     init_sim();

    //Gets the current process ID. This dictates what code to run (producer or consumer)
    int current_process = fork();

    if (current_process == 0) { //child process
       //Northbound producer
        *north_bound_pid = getpid();
        while(key != 'q') {
            // generate cars
            down(&sem);
            //TODO: add to northbound_queue here.
            int car = car_count;
            sem.car = car;
            car_count++; //increment car count so next car created has a new id.
            printf("Car %d coming from the N direction, arrived in the queue at time %d:%d:%d", car_count, get_time()->tm_hour, get_time()->tm_min, get_time()->tm_sec);
            up(&sem);

            key = getkey();
        }
    } else if (current_process > 0) { //parent process.
        int pid = fork(); //fork again from parent process to create a third process.
        if(pid == 0) { //child2 process.
            //Southbound producer
            *south_bound_pid = getpid();
            while(key != 'q') {
                // generate cars
                down(&sem);
                //TODO: add to southbound_queue here.
                int car = car_count;
                sem.car = car;
                car_count++;
                printf("Car %d coming from the S direction, arrived in the queue at time %d:%d:%d", car_count, get_time()->tm_hour, get_time()->tm_min, get_time()->tm_sec);
                up(&sem);

                key = getkey();
            }

        } else if (pid > 0) { //parent process.
            //Flag Person
            *flag_person_pid = getpid();
            // allow cars to travel (consume)
            while(key != 'q'){
                down(&sem);
                //Remove car from either  northbound or southbound based on logic defined in project description.

                up(&sem);

                key = getkey();
            }

        }
    }


    return 0;
}