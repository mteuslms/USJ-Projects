#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For clock()

// matteus 
typedef struct carnode_t {
    int carID;  // Unique identifier for the car
    int loopcount;  // Number of times the car has moved through the queue system
    struct carnode_t *nextptr; // Pointer to the next car in the queue
} carnode_t;

// matteus
typedef struct street_t {
    char streetname[25]; // Name of the street
    int carQdepth; // Maximum allowed depth of the car queue
    int crcount; // Current count of cars in the queue
    carnode_t *Qfront; // Front of the car queue
    carnode_t *Qrear; // Rear of the car queue
    int greenlight_time;  // Duration of the green light (in seconds)
    int redlight_time; // Duration of the red light (in seconds)
    int timer;  // Tracks the current state of the light timer
    int is_green; // 1 if the light is green, 0 if red
} street_t;

// Function to move cars between queues
int go(street_t *current_street, street_t *next_street) {
    if (!current_street->is_green) {
        return 0; // Cannot proceed; light is red
    }

    if (current_street->Qfront == NULL) {
        return 0; // Cannot proceed; no cars to move
    }

    if (next_street && next_street->crcount >= next_street->carQdepth) {
        return 0; // Cannot proceed; next queue is full
    }

    // Moving the car from current street
    carnode_t *moving_car = current_street->Qfront;
    current_street->Qfront = moving_car->nextptr;
    if (current_street->Qfront == NULL) {
        current_street->Qrear = NULL; // Queue is now empty
    }
    current_street->crcount--;

    // If no next street, the car exits
    if (!next_street) {
        free(moving_car);
        return 1;
    }

    // Adding the car to the next street's queue
    moving_car->nextptr = NULL;
    if (next_street->Qrear == NULL) {
        next_street->Qfront = moving_car;
    } else {
        next_street->Qrear->nextptr = moving_car;
    }
    next_street->Qrear = moving_car;
    next_street->crcount++;

    return 1;
}

// Function to control traffic light timing
void lighttimer(street_t *street) {
    street->timer++;
    if (street->is_green && street->timer >= street->greenlight_time) {
        street->is_green = 0; // Switch to red light
        street->timer = 0;
    } else if (!street->is_green && street->timer >= street->redlight_time) {
        street->is_green = 1; // Switch to green light
        street->timer = 0;
    }
}

// Function to add cars to a queue
void add_cars_to_queue(street_t *street, int num_cars) {
    for (int i = 0; i < num_cars; i++) {
        carnode_t *new_car = (carnode_t *)malloc(sizeof(carnode_t));
        new_car->carID = street->crcount + 1;
        new_car->loopcount = 0;
        new_car->nextptr = NULL;
        if (street->Qrear == NULL) {
            street->Qfront = new_car;
        } else {
            street->Qrear->nextptr = new_car;
        }
        street->Qrear = new_car;
        street->crcount++;
    }
}

// Initialize a street
void initialize_street(street_t *street, char *name, int red_time, int green_time, int carQdepth, int green) {
    strcpy(street->streetname, name);
    street->redlight_time = red_time;
    street->greenlight_time = green_time;
    street->carQdepth = carQdepth;
    street->crcount = 0;
    street->Qfront = NULL;
    street->Qrear = NULL;
    street->timer = 0;
    street->is_green = green;
}

// Sergio 12/7/24 pretty print street function
void print_street_status(street_t streets[], int num_streets) {

    // clears previous output
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear");  // Unix/Linux/Mac
    #endif

    printf("Street Queue Status:\n");

    for (int i = 0; i < num_streets; i++) {
        double fullness_percentage = ((double)streets[i].crcount / streets[i].carQdepth) * 100;

        // Print the street details
        printf("%-15s | Queue Depth: %-3d | Cars in Queue: %-3d | Fullness: %.2f%%\n",
               streets[i].streetname, streets[i].carQdepth, streets[i].crcount, fullness_percentage);

        // Create a visual queue representation
        int num_dashes = (int)((double)streets[i].crcount / streets[i].carQdepth * 20);  // 20 dashes for visual scale
        printf("Queue: [");
        for (int j = 0; j < 20; j++) {
            if (j < num_dashes) {
                printf("-");  // Representing cars in the queue
            } else {
                printf(" ");  // Empty spaces for the remaining capacity
            }
        }
        printf("]\n");

        // blank line between streets
        printf("\n");
    }
}

// Random number generator
int rollDice(int numRange) {
    return rand() % numRange;
}

// nelson
void get_simulation_settings(int *num_cycles, int *cycles_per_second) {
    printf("Enter the number of cycles to run: ");
    scanf("%d", num_cycles);

    printf("Enter the number of cycles per second: ");
    scanf("%d", cycles_per_second);
}

int main() {
    srand(time(NULL));

    // Get simulation settings from the user
    int num_cycles, cycles_per_second;
    get_simulation_settings(&num_cycles, &cycles_per_second);

    // Initializing streets
    street_t streets[12];
    char *names[] = {"Lasalle 2", "Lasalle 1", "Farmington 2", "Dale 1", "Brace 1", "Brace 2",
                     "N Main 1", "N Main 2", "Farmington 1", "S Main 1", "S Main 2", "Memorial 3"};
    int red_times[] = {3, 54, 25, 3, 30, 3, 40, 105, 45, 105, 104, 135}; //set times for red light
    int green_times[] = {3, 25, 54, 3, 40, 3, 30, 40, 30, 40, 135, 104}; //set times for green light
    int depths[] = {29, 29, 40, 17, 43, 43, 9, 9, 40, 54, 54, 21}; //set quene depths
    int greens[] = {1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0}; //set starting light state (1 = green, 0 = red))

    for (int i = 0; i < 12; i++) {
        initialize_street(&streets[i], names[i], red_times[i], green_times[i], depths[i], greens[i]);
    }

    // sergio
    double cycle_time = 1.0 / cycles_per_second;
    clock_t start_time = clock();
    clock_t current_time = start_time;

    printf("Starting simulation for %d cycles...\n", num_cycles);

    for (int cycle = 0; cycle < num_cycles; cycle++) {
        printf("Cycle %d:\n", cycle + 1);

        // Random actions and queue updates
        for (int i = 0; i < 12; i++) {
            add_cars_to_queue(&streets[i], rollDice(3)); // Random cars added
            lighttimer(&streets[i]);
        }

        // Print street status
        print_street_status(streets, 12); // Visual representation of all streets' queues

        // Simulate traffic movement
        for (int i = 0; i < 12; i++) {
            go(&streets[i], rollDice(2) ? &streets[(i + 1) % 12] : NULL);
        }

        printf("\n");

        // Wait for the next cycle
        current_time = clock();
        while (((double)(clock() - current_time) / CLOCKS_PER_SEC) < cycle_time) {
        }
    }

    printf("Simulation complete.\n");
    return 0;
}
