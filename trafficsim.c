#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <asm/unistd.h> //this should use the one in linux-2.6.23.1
#include <linux/cs1550_sem.h>

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

#define CAR_QUEUE_SIZE 10

typedef enum {
    NORTH,
    SOUTH
} Direction;

struct car_queue {
    void *buffer[10];
    int head;
    int tail;
    int count;
    int size;
};

int is_full(struct car_queue *queue) {
    if ((queue->head == queue->tail) && (queue->count == queue->size)) {
        return 1;
    }
    return 0;
}

int is_empty(struct car_queue *queue) {
    if ((queue->head == queue->tail) && (queue->count == 0)) {
        return 1;
    }
    return 0;
}

void enqueue(struct car_queue *queue, void *item) {
    queue->buffer[queue->tail] = item;
    queue->tail++;
    queue->count++;
}

void *dequeue(struct car_queue *queue) {
    if (!is_empty(queue)) {
        void *item = queue->buffer[queue->head];
        queue->head++;
        queue->count--;
        return item;
    }
    return 0;
}

void set_queue_size(struct car_queue * queue) {
    queue->size = CAR_QUEUE_SIZE;
}

/**
 * Car struct.
 */
typedef struct Car {
    int car_id;
    struct tm* timeinfo; //Time of the car.
    Direction dir; //The direction the car is traveling.
} Car;

typedef struct {
   struct cs1550_sem* sem_north_bound;
   struct cs1550_sem* sem_south_bound;
   struct cs1550_sem* sem_flag_person;

} my_sems;

my_sems sems;

int car_id_count = 0;
Direction current_direction = NORTH;

struct car_queue* north_bound;
struct car_queue* south_bound;

/**
* Used to call our modified syscall in the linux kernel to down our semaphore.
* @param sem - the semaphore being down'ed
*/
void down(struct cs1550_sem *sem) {
   syscall(__NR_cs1550_down, sem);
//    printf("syscall down 57 called\n");
}

/**
* Used to call our modified syscall in the linux kernel to up our semaphore.
* @param sem - the semaphore being up'ed
*/
void up(struct cs1550_sem *sem) {
   syscall(__NR_cs1550_up, sem);
//    printf("syscall up called 66\n");
}

/**
* Calculates the size of the memory needed to be mapped.
* @return N - the number of bytes.
*/
int calculate_mem_size() {
    printf("Calculating Mem size \n");
    int N = 0;
    N = N + sizeof(Car) * CAR_QUEUE_SIZE; //Size of one queue
    N = N + sizeof(Car) * CAR_QUEUE_SIZE; //Size of another queue
    N = N + sizeof(struct cs1550_sem) * 3; //we have 3 semaphores (one for each process 1 consumer 2 producer)
   return N;
}

void init_ptrs(void* ptr_to_mem) {
    printf("initializing pointers \n");
    int sizeOfSem = sizeof(struct cs1550_sem);

    sems.sem_flag_person = ptr_to_mem;
    sems.sem_north_bound = sems.sem_flag_person + sizeOfSem;
    sems.sem_south_bound = sems.sem_north_bound + sizeOfSem;
    north_bound = (struct car_queue*)(sems.sem_south_bound + sizeOfSem);
    south_bound = north_bound + (sizeof(Car) * CAR_QUEUE_SIZE);
}

/**
* Used to initialize the simulaiton
*  - Initializes 2 producers (Northbound  and Southbound)
*  - Initializes 1 consumer (flagperson)
*/
void init_sim() {
    printf("initializing simulation.\n");
   //Initialize memory space
   int N = calculate_mem_size();
   void* ptr = mmap(NULL, N, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
   init_ptrs(ptr); //initializes the global pointers.
    //Initialize values in sems.
    printf("Initializing semaphore values.\n");
    sems.sem_south_bound->value = 0;
    sems.sem_north_bound->value = 1;
    sems.sem_flag_person->value = 0;
    printf("setting queue sizes.\n");
    //Setting car queue sizes to QUEUE_SIZE.
    set_queue_size(north_bound);
    set_queue_size(south_bound);

    //initialize the process_queue sizes.
    sems.sem_north_bound->process_queue.queue_size = 4;
    sems.sem_south_bound->process_queue.queue_size = 4;
    sems.sem_flag_person->process_queue.queue_size = 4;
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

//Is this the best way to get the time from a car?
/**
 * Gets the local time based on the timeinfo of the car.
 * @param c - The car to get the time of.
 */
void get_car_time(Car *c) {
   time_t rawtime;
   time(&rawtime);
   c->timeinfo = localtime(&rawtime);
}

/**
 * Uses get_car_time() to print what time the car arrived in the que and what direction
 * @param c - the car added to the queue.
 */
void print_car_arrived(Car* c) {
    get_car_time(c);
    char direction;
    if (c->dir == NORTH) {
        direction = 'N';
    } else {
        direction = 'S';
    }
    printf("Car %d coming from the %c direction arrive in the queue at time %d:%d:%d.\n"
            , c->car_id, direction, c->timeinfo->tm_hour, c->timeinfo->tm_min, c->timeinfo->tm_sec);
}

/**
 * Prints the time the car honked their horn.
 * @param c - The car that honks their horn.
 */
void print_car_honk(Car* c) {
    get_car_time(c);
    char direction;
    if (c->dir == NORTH) {
        direction = 'N';
    } else {
        direction = 'S';
    }
    printf("Car %d coming from the %c direction blew their horn at time %d:%d:%d.\n"
            , c->car_id, direction, c->timeinfo->tm_hour, c->timeinfo->tm_min, c->timeinfo->tm_sec);
}

void print_car_left(Car* c) {
    get_car_time(c);
    char direction;
    if (c->dir == NORTH) {
        direction = 'N';
    } else {
        direction = 'S';
    }
    printf("Car %d coming from the %c direction left the construction zone at time %d:%d:%d.\n"
            , c->car_id, direction, c->timeinfo->tm_hour, c->timeinfo->tm_min, c->timeinfo->tm_sec);
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

void let_car_pass(struct car_queue* queue) {
    struct car_queue current_car_queue = *queue;
    while (!is_empty(&current_car_queue)) {
        Car* dequed_car = dequeue(&current_car_queue);
        let_car_through(); //sleeps for 2 seconds
        print_car_left(dequed_car);
        //TODO: gotta check to see if other queue is full. If it is we have to switch sides and consume from the other side
        if (current_direction == NORTH) {
            if (is_full(south_bound)) {
                //If the opposite side is full we have to change current_direction to let cars from other side go.
                current_direction = SOUTH;
            }
        } else { //current_direction == South
            if (is_full(north_bound)) {
                //If the opposite side is full we have to change current_direction to let cars from other side go.
                current_direction = NORTH;
            }
        }

        //Change the current_car_queue to dequeue from the current_direction.
        if (current_direction == NORTH) {
            current_car_queue = *north_bound;
        } else {
            current_car_queue = *south_bound;
        }

    }
    delay_20_sec();
    down(sems.sem_flag_person); //
}

/**
 * Adds cars to a given queue. If the queue is empty and flag_person is asleep the car will wake up the flag_person
 * by honking their horn.
 * @param queue
 */
int car_arrives(struct car_queue* queue, Direction direction) {
    //produces cars each car created has an 80% chance of a car being behind it.
    do {
        //Create a car
        Car car;
        car.car_id = car_id_count;
        car.dir = direction;
        car_id_count++; //increment car count so next car created has a new id.
        if (is_empty(queue)) { //check to see if queue is empty. Which means car about to be added will be the first car.
            //first car is arriving
            //Check and make sure flag_person is sleeping before waking him up.
            if (sems.sem_flag_person->value < 0) {
                print_car_honk(&car); //Car honks to wake up flag person.
                current_direction = direction;//Set current direction to the honking direction
                //Wake up flag_person.
                up(sems.sem_flag_person); //Does this actually wake up consumer?
                down(queue); //down the current queue to stop producing.
                printf("Upped flag_person to wake him up after car honks\n");
            }
        }
        if(is_full(queue)){
            //When the que becomes full set the current_direction to this direction
            // so that the flag_person can start consuming from the full queue.
            current_direction = direction;
            up(sems.sem_flag_person); //wake up flag_person
            down(queue); //down the current queue to stop producing
            printf("Upped flag_person to wake him up after changing direction. About to return 0 from car_arrives()\n");
            return 0;
        } else {
            enqueue(queue, &car);
        }
        print_car_arrived(&car);
    }
    while(chance_80());
    return 1;
}

int main(int argc, char **argv) {
   srand(time(NULL));
   char key = getkey();

   //Start by initializing the simulation.
    init_sim();

   //Gets the current process ID. This dictates what code to run (producer or consumer)
   int current_process = fork();

   if (current_process == 0) { //child process
       printf("About to execute northbound producer code\n");
      //Northbound producer
       while(key != 'q') {
           down(sems.sem_north_bound);
           printf("Downed north_bound sem on line 322\n");
           car_arrives(north_bound, NORTH);  //car arrives in northbound queue.
           printf("N - car_arrives() called line 325\n");
           up(sems.sem_north_bound);
           printf("Upped north_bound sem on line 327\n");
           key = getkey();
       }
   } else if (current_process > 0) { //parent process.
       int pid = fork(); //fork again from parent process to create a third process.
       if(pid == 0) { //child2 process.
           //Southbound producer
           printf("About to execute southbound producer code\n");
           while(key != 'q') {
               // generate cars
               down(sems.sem_south_bound);
               printf("Downed south_bound sem on line 338\n");
               car_arrives(south_bound, SOUTH);//car arrives in southbound queue.
               printf("S - car_arrives() called line 340\n");
               up(sems.sem_south_bound);
               printf("Upped south_bound sem on line 342\n");
               key = getkey();
           }

       } else if (pid > 0) { //parent process.
           //Flag Person
           // allow cars to travel (consume)
           printf("About to execute flag_person consumer code\n");
           while(key != 'q'){
               down(sems.sem_flag_person);
               printf("Downed flag_person sem on line 352\n");
               //Remove car from either  northbound or southbound based on current_direction.
                if (current_direction == NORTH) {
                    let_car_pass(north_bound);
                    printf("let_car_pass(north_bound)  on line 356\n");
                } else {
                    let_car_pass(south_bound);
                    printf("let_car_pass(south_bound)  on line 359\n");
                }
               up(sems.sem_flag_person);
               printf("Upped flag_person sem on line 362\n");
               key = getkey();
           }

       }
   }


    return 0;
}