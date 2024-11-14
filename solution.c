#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define LEFT (i + 4) % N
#define RIGHT (i + 1) % N

// Enum to represent philosopher states
enum { THINKING, HUNGRY, EATING } state[N];

// Semaphores for signaling philosophers
sem_t sem;
sem_t S[N];

// Function declarations
void pickup(int i);
void putdown(int i);
void test(int i);
void *philosopher(void* num);

/**
 * init - Initializes the philosophers' states to THINKING,
 * 
 * sets up the semaphores for synchronization.
 */
void init() {
    sem_init(&sem, 0, 1); // Initialize semaphore to 1 for critical sections
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        sem_init(&S[i], 0, 0); // Initialize each philosopher's semaphore to 0 (blocked initially)
    }
}

/**
 * pickup - Philosopher attempts to pick up chopsticks.
 * @param i - Index of the philosopher.
 */
void pickup(int i) {
    sem_wait(&sem);  // Enter critical section

    state[i] = HUNGRY; // Mark philosopher as HUNGRY
    printf("Philosopher %d is Hungry\n", i + 1);

    test(i); // Attempt to pick up both chopsticks
    sem_post(&sem); // Leave critical section

    // Wait if unable to eat
    sem_wait(&S[i]); // Wait on philosopher's semaphore until signaled to eat
}

/**
 * putdown - Philosopher puts down chopsticks.
 * @param i - Index of the philosopher.
 */
void putdown(int i) {
    sem_wait(&sem);  // Enter critical section

    state[i] = THINKING; // Mark philosopher as THINKING
    printf("Philosopher %d is putting down chopsticks\n", i + 1);
    printf("Philosopher %d is thinking\n", i + 1);

    // Test if the neighbors can start eating
    test(LEFT); // Check if left neighbor can eat
    test(RIGHT);     // Check if right neighbor can eat

    sem_post(&sem); // Leave critical section
}

/**
 * test - Checks if a philosopher can start eating.
 * @param i - Index of the philosopher.
 */
void test(int i) {
    if (state[i] == HUNGRY &&
        state[LEFT] != EATING &&
        state[RIGHT] != EATING) {
        
        // Set philosopher state to EATING
        state[i] = EATING;
        printf("Philosopher %d takes chopsticks %d and %d\n", i + 1, (i + N - 1) % N + 1, i + 1);
        printf("Philosopher %d is Eating\n", i + 1);

        // Signal philosopher to start eating
        sem_post(&S[i]);
    }
}

/**
 * philosopher - Function run by each philosopher thread.
 * @param num - Index of the philosopher.
 */
void *philosopher(void* num) {
    int i = *(int*)num;

    while (1) {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(1); // Think for 1 second

        pickup(i); // Try to pick up chopsticks
        sleep(1); // Eat for 1 second

        putdown(i); // Put down chopsticks
    }
}

int main() {
    pthread_t tid[N];
    int philosophers[N];

    // Initialize semaphores and states
    init();

    // Create threads for each philosopher
    for (int i = 0; i < N; i++) {
        philosophers[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &philosophers[i]);
    }

    // Wait for all threads to finish (in this case, they run indefinitely)
    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
