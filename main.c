monitor DiningPhilosophers {
    enum { THINKING, HUNGRY, EATING } state[5]; // Define possible states for each philosopher: THINKING, HUNGRY, and EATING.
    condition self[5];                          // Each philosopher has a condition variable for synchronizing access to resources (chopsticks).

    // Function to pick up chopsticks (called when a philosopher wants to eat)
    void pickup(int i) {
        state[i] = HUNGRY;          // Set the philosopher's state to HUNGRY, indicating they want to eat.
        test(i);                    // Check if the philosopher can eat (both neighbors are not eating).
        
        if (state[i] != EATING)     // If the philosopher couldn't start eating immediately,
            self[i].wait();         // they wait until signaled that they can proceed (when chopsticks are available).
    }

    // Function to put down chopsticks (called when a philosopher finishes eating)
    void putdown(int i) {
        state[i] = THINKING;        // Set the philosopher's state to THINKING, indicating they are done eating.
        
        // Test if the left neighbor can eat
        test((i + 4) % 5);          // This checks the philosopher on the left, adjusting index with modulo for circular array.
        
        // Test if the right neighbor can eat
        test((i + 1) % 5);          // This checks the philosopher on the right.
    }

    // Function to check if a philosopher can start eating
    void test(int i) {
        // Conditions to allow philosopher i to eat:
        // - The left neighbor is not eating
        // - Philosopher i is hungry
        // - The right neighbor is not eating
        if ((state[(i + 4) % 5] != EATING) && 
            (state[i] == HUNGRY) && 
            (state[(i + 1) % 5] != EATING)) {
                
            state[i] = EATING;      // Set the philosopher's state to EATING (they can start eating).
            self[i].signal();       // Signal the philosopher, allowing them to proceed if they were waiting.
        }
    }

    // Initialization code for setting up philosophers' initial states
    initialization code() {
        for (int i = 0; i < 5; i++) // Loop through each philosopher
            state[i] = THINKING;    // Set each philosopher's initial state to THINKING.
    }
}
