#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Number of philosophers
#define N 5

// Enum to represent philosopher states
enum { THINKING, HUNGRY, EATING } state[N];

// Mutex for critical sections
pthread_mutex_t mutex;

// Condition variables for each philosopher
pthread_cond_t self[N];

// Function declarations
void pickup(int i);
void putdown(int i);
void test(int i);
void* philosopher(void* num);

// Initializes the philosophers to THINKING and sets up mutex and condition variables
void init() {
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        pthread_cond_init(&self[i], NULL);
    }
}

// Philosopher attempts to pick up chopsticks
// NOTE: starvation could occur here because of the waiting condition
void pickup(int i) {
    pthread_mutex_lock(&mutex); // Enter critical section
    state[i] = HUNGRY; // Mark philosopher as HUNGRY
    test(i); // Try to acquire both chopsticks
    if (state[i] != EATING) // If unable to eat, wait for a signal (starvation)
        pthread_cond_wait(&self[i], &mutex);
    pthread_mutex_unlock(&mutex); // Leave critical section
}

// Philosopher puts down chopsticks
void putdown(int i) {
    pthread_mutex_lock(&mutex); // Enter critical section
    state[i] = THINKING; // Mark philosopher as THINKING
    test((i + N - 1) % N); // Test if left neighbor can eat
    test((i + 1) % N); // Test if right neighbor can eat
    pthread_mutex_unlock(&mutex); // Leave critical section
}

// Test if a philosopher can start eating
void test(int i) {
    if (state[(i + N - 1) % N] != EATING && // Left neighbor not eating
        state[i] == HUNGRY && // Current philosopher is HUNGRY
        state[(i + 1) % N] != EATING) { // Right neighbor not eating
        
        state[i] = EATING; // Set state to EATING
        pthread_cond_signal(&self[i]); // Signal the philosopher to start eating
    }
}

// Function run by each philosopher thread
void* philosopher(void* num) {
    int i = *(int*)num;

    while (1) {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(1); // Thinking for a while

        pickup(i); // Try to pick up chopsticks
        printf("Philosopher %d is eating\n", i + 1);
        sleep(1); // Eating for a while

        putdown(i); // Put down chopsticks
    }
}

int main() {
    pthread_t tid[N];
    int philosophers[N];

    init(); // Initialize mutex and condition variables

    for (int i = 0; i < N; i++) {
        philosophers[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &philosophers[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL); // Wait for all threads to finish (in this case, they run indefinitely)
    }

    return 0;
}

/*
    must implement increasing priority while waiting (a counter for each philosopher)
    or a similar condition that keeps track of philosophers who ate recently or ones that havent
    so that those processes can run eventually without starving
    ie. a counter that goes up to 5 and check it on the pickup, if it's reached 5 then let that
    philosopher eat
*/