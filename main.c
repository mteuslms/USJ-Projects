#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // For floor()


// Global constant for simulation time
const int simulation_time = 60; // Simulation runs for 60 seconds

// Car node structure
typedef struct carnode_t {
    int carID;          // Unique identifier for the car
    int loopcount;      // Number of times the car has moved through the queue system
    struct carnode_t *nextptr; // Pointer to the next car in the queue
} carnode_t;

// Street structure
typedef struct street_t {
    char streetname[25];  // Name of the street
    int carQdepth;        // Maximum allowed depth of the car queue
    int crcount;          // Current count of cars in the queue
    carnode_t *Qfront;    // Front of the car queue
    carnode_t *Qrear;     // Rear of the car queue
    int greenlight_time;  // Duration of the green light (in seconds)
    int redlight_time;    // Duration of the red light (in seconds)
    int timer;            // Tracks the current state of the light timer
    int is_green;         // 1 if the light is green, 0 if red
    int cars_per5min; // Number of cars entering this street in a 5-minute observation
    int time_since_last_add;
    int stop_sign;        // 1 if it is a stop sign, 0 otherwise
} street_t;

// Function prototypes
void initialize_street(street_t *street, char *name, int red_time, int green_time, int carQdepth, int green, int cars_per5min, int stop_sign);
void print_street_status(street_t streets[], int num_streets);
void lighttimer(street_t *street);
void add_cars_from_data_cycle(street_t *street, double datacycle, int current_time);
int go(street_t *current_street, street_t streets[]);
double get_datacycle();

int main() {
    srand(time(NULL));

    // Get the multiplier for data cycles
    double datacycle = get_datacycle();
    double time_ratio = 0.2*datacycle;
    // Initialize all streets
    street_t lasalle1, lasalle2, dale3, farmington1, farmington2, brace1, brace2, memorial3, s_main1, s_main2, n_main1, n_main2;

    initialize_street(&lasalle1, "Lasalle 1", 54*time_ratio, 25*time_ratio, 29, 1, 5, 0);
    initialize_street(&lasalle2, "Lasalle 2", 0, 0, 29, 1, 3, 1);
    initialize_street(&dale3, "Dale 3", 0, 0, 17, 1, 3, 1); 
    initialize_street(&farmington1, "Farmington 1", 45*time_ratio, 30*time_ratio, 40, 0, 13, 0);
    initialize_street(&farmington2, "Farmington 2", 25*time_ratio, 54*time_ratio, 42, 0, 31, 0);
    initialize_street(&brace1, "Brace 1", 30*time_ratio, 40*time_ratio, 43, 0, 9, 0);
    initialize_street(&brace2, "Brace 2", 0, 0, 43, 0, 6, 1);
    initialize_street(&memorial3, "Memorial 3", 135*time_ratio, 104*time_ratio, 21, 0, 10, 0);
    initialize_street(&s_main1, "S Main 1", 105*time_ratio, 40*time_ratio, 54, 1, 40, 0);
    initialize_street(&s_main2, "S Main 2", 104*time_ratio, 135*time_ratio, 54, 1, 7, 0);
    initialize_street(&n_main1, "N Main 1", 40*time_ratio, 30*time_ratio, 9, 1, 17, 0);
    initialize_street(&n_main2, "N Main 2", 105*time_ratio, 40*time_ratio, 9, 1, 42, 0);

    street_t streets[] = {lasalle1, lasalle2, dale3, 
                        farmington1, farmington2, brace1, 
                        brace2, memorial3, s_main1, 
                        s_main2, n_main1, n_main2};
    int num_streets = sizeof(streets) / sizeof(streets[0]);

    printf("Starting simulation...\n");

    for (int sim_time = 0; sim_time < simulation_time; sim_time++) {
        
        for (int i = 0; i < num_streets; i++) {
            if (!streets[i].stop_sign) {
                lighttimer(&streets[i]);
            }
            go(&streets[i], streets);
        }
        
        for (int i = 0; i < num_streets; i++) {
            // Increment the timer for each street
            streets[i].time_since_last_add++;

            // Try to add cars based on datacycle
            add_cars_from_data_cycle(&streets[i], datacycle, sim_time);
        }

        print_street_status(streets, num_streets);

        // Sleep for 1 second to simulate real-time behavior
        #ifdef _WIN32
            Sleep(1000);  // Sleep for 1 second (1000 milliseconds) on Windows
        #else
            sleep(1);     // Sleep for 1 second (on Unix/Linux/Mac)
        #endif
    }

    printf("Simulation complete.\n");
}

// Function to initialize a street
void initialize_street(street_t *street, char *name, int red_time, int green_time, int carQdepth, int green, int cars_per5min, int stop_sign) {
    strcpy(street->streetname, name);
    street->redlight_time = red_time;
    street->greenlight_time = green_time;
    street->carQdepth = carQdepth;
    street->crcount = 0;
    street->Qfront = NULL;
    street->Qrear = NULL;
    street->timer = 0;
    street->is_green = green;
    street->cars_per5min = cars_per5min;
    street->time_since_last_add = 0;
    street->stop_sign = stop_sign;
}

// Function to move cars between queues
int go(street_t *current_street, street_t streets[]) {
    if (!current_street->is_green && !current_street->stop_sign) {
        return 0; // Cannot proceed; light is red
    }

    if (current_street->Qfront == NULL) {
        return 0; // No cars to move
    }

    carnode_t *moving_car = current_street->Qfront;
    current_street->Qfront = moving_car->nextptr;
    if (current_street->Qfront == NULL) {
        current_street->Qrear = NULL; // Queue is now empty
    }

    double rand_percent = rand() / (double)RAND_MAX;

    if (strcmp(current_street->streetname, "Lasalle 1") == 0) {
        // 33% to Farmington 1, 33% to Brace 1, 33% leave
        if (rand_percent < 0.3) {
                if(!(streets[3].crcount < streets[3].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[3].Qrear == NULL) {
                    streets[3].Qfront = moving_car;
                } else {
                    streets[3].Qrear->nextptr = moving_car;
                }
                streets[3].Qrear = moving_car;
                streets[3].crcount++;
        } else if (rand_percent < 0.6) {
                if(!(streets[5].crcount < streets[5].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[5].Qrear == NULL) {
                    streets[5].Qfront = moving_car;
                } else {
                    streets[5].Qrear->nextptr = moving_car;
                }
                streets[5].Qrear = moving_car;
                streets[5].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "Lasalle 2") == 0) {
        // 60% to Memorial 3, 40% leave
        if (rand_percent < 0.6) {
                if(!(streets[7].crcount < streets[7].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[7].Qrear == NULL) {
                    streets[7].Qfront = moving_car;
                } else {
                    streets[7].Qrear->nextptr = moving_car;
                }
                streets[7].Qrear = moving_car;
                streets[7].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "Dale 3") == 0) {
        // 60% to Brace 1, 40% leave
        if (rand_percent < 0.6) {
                if(!(streets[5].crcount < streets[5].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[5].Qrear == NULL) {
                    streets[5].Qfront = moving_car;
                } else {
                    streets[5].Qrear->nextptr = moving_car;
                }
                streets[5].Qrear = moving_car;
                streets[5].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "Farmington 1") == 0) {
        // 20% to S Main 2, 40% to N Main 1, 40% leave
        if (rand_percent < 0.2) {
                if(!(streets[9].crcount < streets[9].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[9].Qrear == NULL) {
                    streets[9].Qfront = moving_car;
                } else {
                    streets[9].Qrear->nextptr = moving_car;
                }
                streets[9].Qrear = moving_car;
                streets[9].crcount++;
        } else if (rand_percent < 0.6) {
                if(!(streets[10].crcount < streets[10].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[10].Qrear == NULL) {
                    streets[10].Qfront = moving_car;
                } else {
                    streets[10].Qrear->nextptr = moving_car;
                }
                streets[10].Qrear = moving_car;
                streets[10].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "Farmington 2") == 0) {
        // 30% leave, 30% to Lasalle 2, 30% to Dale 3
        if (rand_percent < 0.3) {
            free(moving_car);
        } else if (rand_percent < 06) {
                if(!(streets[1].crcount < streets[1].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[1].Qrear == NULL) {
                    streets[1].Qfront = moving_car;
                } else {
                    streets[1].Qrear->nextptr = moving_car;
                }
                streets[1].Qrear = moving_car;
                streets[1].crcount++;
        } else if (rand_percent < 1.0) {
                if(!(streets[2].crcount < streets[2].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[2].Qrear == NULL) {
                    streets[2].Qfront = moving_car;
                } else {
                    streets[2].Qrear->nextptr = moving_car;
                }
                streets[2].Qrear = moving_car;
                streets[2].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "Brace 1") == 0) {
        // 40% leave, 60% to N Main 2
        if (rand_percent < 0.4) {
            free(moving_car);
        } else {
            if(!(streets[11].crcount < streets[11].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[11].Qrear == NULL) {
                streets[11].Qfront = moving_car;
            } else {
                streets[11].Qrear->nextptr = moving_car;
            }
            streets[11].Qrear = moving_car;
            streets[11].crcount++;
        }
    } else if (strcmp(current_street->streetname, "Brace 2") == 0) {
        // All cars leave
        free(moving_car);
    } else if (strcmp(current_street->streetname, "Memorial 3") == 0) {
        // 40% leave, 60% to S Main 1
        if (rand_percent < 0.4) {
            free(moving_car);
        } else {
            if(!(streets[8].crcount < streets[8].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[8].Qrear == NULL) {
                streets[8].Qfront = moving_car;
            } else {
                streets[8].Qrear->nextptr = moving_car;
            }
            streets[8].Qrear = moving_car;
            streets[8].crcount++;
        }
    } else if (strcmp(current_street->streetname, "S Main 2") == 0) {
        // All cars leave
        free(moving_car);
    } else if (strcmp(current_street->streetname, "S Main 1") == 0) {
        // 40% to Farmington 2, 40% to N main 1, 20% exit
        if (rand_percent < 0.2) {
            free(moving_car);
        } else if (rand_percent < 0.6) {
                if(!(streets[4].crcount < streets[4].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[4].Qrear == NULL) {
                    streets[4].Qfront = moving_car;
                } else {
                    streets[4].Qrear->nextptr = moving_car;
                }
                streets[4].Qrear = moving_car;
                streets[4].crcount++;
        } else if (rand_percent < 1.0) {
                if(!(streets[10].crcount < streets[10].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[10].Qrear == NULL) {
                    streets[10].Qfront = moving_car;
                } else {
                    streets[10].Qrear->nextptr = moving_car;
                }
                streets[10].Qrear = moving_car;
                streets[10].crcount++;
        } else {
            free(moving_car);
        }
    } else if (strcmp(current_street->streetname, "N Main 1") == 0) {
        // 70% to Brace 2, 30% to exit
        if (rand_percent < 0.3) {
            free(moving_car);
        } else {
            if(!(streets[6].crcount < streets[6].carQdepth)) {
                    return 0;//next street is full
            } else if (streets[6].Qrear == NULL) {
                streets[6].Qfront = moving_car;
            } else {
                streets[6].Qrear->nextptr = moving_car;
            }
            streets[6].Qrear = moving_car;
            streets[6].crcount++;
        }
    } else if (strcmp(current_street->streetname, "N Main 2") == 0) {
        // 40% to Farmington 2, 40% to S main 2, 20% exit
        if (rand_percent < 0.2) {
            free(moving_car);
        } else if (rand_percent < 0.6) {
                if(!(streets[4].crcount < streets[4].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[4].Qrear == NULL) {
                    streets[4].Qfront = moving_car;
                } else {
                    streets[4].Qrear->nextptr = moving_car;
                }
                streets[4].Qrear = moving_car;
                streets[4].crcount++;
        } else if (rand_percent < 1.0) {
                if(!(streets[9].crcount < streets[9].carQdepth)) {
                    return 0;//next street is full
                } else if (streets[9].Qrear == NULL) {
                    streets[9].Qfront = moving_car;
                } else {
                    streets[9].Qrear->nextptr = moving_car;
                }
                streets[9].Qrear = moving_car;
                streets[9].crcount++;
        } else {
            free(moving_car);
        }
    }
    
    current_street->crcount--;
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
void add_cars_from_data_cycle(street_t *street, double datacycle, int sim_time) {
    // Calculate the interval for adding cars (in seconds)
    double cars_to_add_per_minute = street->cars_per5min * datacycle; // Cars per minute
    double interval = 60.0 / cars_to_add_per_minute;                       // Time between car additions

    // Check if it's time to add a car
    if (street->time_since_last_add >= interval) {
        // Add a car to the street's queue
        if (street->crcount < street->carQdepth) {
            carnode_t *new_car = (carnode_t *)malloc(sizeof(carnode_t));
            if (new_car == NULL) {
                printf("Memory allocation failed for new car.\n");
                return;
            }
            new_car->carID = street->crcount + 1;
            new_car->loopcount = 0;
            new_car->nextptr = NULL;

            if (street->Qrear == NULL) {
                street->Qfront = new_car;  // Empty queue
            } else {
                street->Qrear->nextptr = new_car;  // Attach to the end
            }
            street->Qrear = new_car;
            street->crcount++;
        }
        // Reset the timer
        street->time_since_last_add = 0;
    }
}


// Pretty print street function
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

// Function to calculate data cycles and set the simulation to 60 seconds
double get_datacycle() {
    char input_buffer[100];  // Buffer to store user input
    int desired_sim_time;    // User input for the time the simulation should represent

    // Get the desired simulation time from the user
    while (1) {
        printf("Enter the total simulation time to represent (in seconds, e.g., 300): ");
        fgets(input_buffer, sizeof(input_buffer), stdin);

        // Validate input
        if (sscanf(input_buffer, "%d", &desired_sim_time) == 1 && desired_sim_time > 0) {
            break;  // Valid input
        } else {
            printf("Invalid input. Please enter a positive integer.\n");
        }
    }

    // Calculate the number of 5-minute data cycles since data we have is 5 min of observation
    double total_data_cycles = (double)desired_sim_time / 300.0;

    printf("The desired simulation time represents %.2f data cycle(s) (each 5 minutes).\n", total_data_cycles);
    return total_data_cycles;
}