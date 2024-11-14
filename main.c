/**
 * Operating System Concepts (10th edition) Book pages: 
 * 293-294 - dining philosopher problem explained
 * 296 - the basis of the code implemented from this page
 *      this is an impartial solution because of the possiblity of starvation
 * 298 - provides counter type atomic_t, and functions atomic_set,atomic_add,atomic_sub, etc.
 * 299 - POSIX synchronization, non-kernel methods to synchronization
 * 299-300 - POSIX Mutex
 * 300-302 - POSIX Named/Unnamed Semaphores
 * 302-303 - POSIX Condition Variables: pthread_cond_init(&cond_var, &mutex);
 *      condition variable that triggers a wait on the mutex
 * 
 * https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/
 * 
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <semaphore.h>

// Number of philosophers
#define N 5

// Enum to represent philosopher states
enum { THINKING, HUNGRY, EATING } state[N];

// Mutex for critical sections
pthread_mutex_t mutex;

// Semaphore to solve starvation
sem_t sem;
sem_t S[N];

// Condition variables for each philosopher
pthread_cond_t self[N];

// Function declarations
void pickup(int i);
void putdown(int i);
void test(int i);
void *philosopher(void* num);

/**
 * pthread_mutex_init Initializes the philosophers to 
 *      THINKING and sets up mutex and condition variables
 * */ 
void init() {
    // pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 1); // Semaphore is non-shared and initialized to value 1
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        pthread_cond_init(&self[i], NULL);
    }
}

/**
 * pickup - Philospher attempts to pick up chopsticks
 * @param (int) i - index of the philosopher (the philosopher's state)
 * NOTE: starvation could occur here because of the waiting condition
*/
void pickup(int i) {
    // pthread_mutex_lock(&mutex); // Enter critical section
    sem_wait(&sem);
    state[i] = HUNGRY; // Mark philosopher as HUNGRY
    test(i); // Try to acquire both chopsticks
    sem_post(&sem);
    sem_wait(&sem); // if unable to eat, wait for a signal
    /* if (state[i] != EATING) // If unable to eat, wait for a signal
        pthread_cond_wait(&self[i], &mutex); */
    pthread_mutex_unlock(&mutex); // Leave critical section
}

/**
 * putdown - Philosopher puts down chopsticks
 * @param (int) i - index of the philosopher (the philosopher's state)
*/
void putdown(int i) {
    pthread_mutex_lock(&mutex); // Enter critical section
    state[i] = THINKING; // Mark philosopher as THINKING
    test((i + N - 1) % N); // Test if left neighbor can eat
    test((i + 1) % N); // Test if right neighbor can eat
    pthread_mutex_unlock(&mutex); // Leave critical section
}

/**
 * test - test if the conditions meet to start eating:
 *      if the philosopher is hungry AND
 *      if the philosopher's neighbors are not currently eating
 * @param (int) i - index of the philosopher (the philosopher's state)
*/
void test(int i) {
    if (state[(i + N - 1) % N] != EATING && // Left neighbor not eating
        state[i] == HUNGRY && // Current philosopher is hungry
        state[(i + 1) % N] != EATING) { // Right neighbor not eating
        
        state[i] = EATING; // Set state to EATING
        pthread_cond_signal(&self[i]); // Signal the philosopher to start eating
    }
}

/**
 * philosopher - Function run by each philosopher thread
 * @param (void*) - numbers that represent philosopher
*/
void *philosopher(void* num) {
    int i = *(int*)num;

    while (1) {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(1); // Thinking for 1 second

        pickup(i); // Try to pick up chopsticks
        printf("Philosopher %d is eating\n", i + 1);
        sleep(1); // Eating for 1 second

        putdown(i); // Put down chopsticks
    }
}

int main() {
    pthread_t tid[N];
    int philosophers[N];

    // initialize mutex and condition variables
    init(); 

    // create a thread for each philosopher
    for (int i = 0; i < N; i++) {
        philosophers[i] = i;
        // run the philosopher function on each thread
        pthread_create(&tid[i], NULL, philosopher, &philosophers[i]); 
    }

    for (int i = 0; i < N; i++) {
        // Wait for all threads to finish (in this case, they run indefinitely)
        pthread_join(tid[i], NULL); 
    }

    return 0;
}