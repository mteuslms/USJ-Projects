//Mateus 12/5/2024  

#include <stdio.h> 

#include <stdlib.h> 

#include <string.h> 

#include <time.h> // For clock()
 

// Define the car node structure 

typedef struct carnode_t { 

    int carID;  // Unique identifier for the car 

    int loopcount;  // Number of times the car has moved through the queue system 

    struct carnode_t *nextptr;// Pointer to the next car in the queue 

} carnode_t; 

 

// Define the street structure 

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

 

//Mateus 12/5/2024  

 

// Function to move cars between queues 

int go(street_t *current_street, street_t *next_street) { 

    if (!current_street->is_green) { 

        printf("Light is red. Cars cannot move.\n"); 

        return 0; // Cannot proceed; light is red 

    } 

 

    if (current_street->Qfront == NULL) { 

        printf("No cars in the current street queue.\n"); 

        return 0; // Cannot proceed; no cars to move 

    } 
    //Andrey 12/6/2024

    if (next_street == NULL) {
        // Moving the car from current street to exit
        carnode_t *moving_car = current_street->Qfront;
        current_street->Qfront = moving_car->nextptr; 

        if (current_street->Qfront == NULL) { 

            current_street->Qrear = NULL; // Queue is now empty 

        } 

        current_street->crcount--; 
 

        printf("Car %d moved from %s to Exit.\n", moving_car->carID, current_street->streetname); 

        return 1; // Successfully moved car 
    
    }
    
    //Mateus 12/5/2024 
    
    if (next_street->crcount >= next_street->carQdepth) { 

        printf("Next street queue is full. Cannot move cars.\n"); 

        return 0; // Cannot proceed; next queue is full 

    } 

 

    // Moving the car from current street to next street 

    carnode_t *moving_car = current_street->Qfront; 

    current_street->Qfront = moving_car->nextptr; 

    if (current_street->Qfront == NULL) { 

        current_street->Qrear = NULL; // Queue is now empty 

    } 

    current_street->crcount--; 

 

    // Adding the car to the next street's queue 

    moving_car->nextptr = NULL; 

    if (next_street->Qrear == NULL) { 

        next_street->Qfront = moving_car; 

    } else { 

        next_street->Qrear->nextptr = moving_car; 

    } 

    next_street->Qrear = moving_car; 

    next_street->crcount++; 

 

    printf("Car %d moved from %s to %s.\n", moving_car->carID, current_street->streetname, next_street->streetname); 

    return 1; // Successfully moved car 

} 

 

//Mateus 12/5/2024  

// Function to control traffic light timing 

void lighttimer(street_t *street) { 

    street->timer++; 

 

    if (street->is_green && street->timer >= street->greenlight_time) { 

        // Switch to red light 

        street->is_green = 0; 

        street->timer = 0; 

        printf("%s light turned red.\n", street->streetname); 

    } else if (!street->is_green && street->timer >= street->redlight_time) { 

        // Switch to green light 

        street->is_green = 1; 

        street->timer = 0; 

        printf("%s light turned green.\n", street->streetname); 

    } 

} 

//Mateus 12/5/2024 

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

    street->Qrear = new_car; street->crcount++;  

    } 

} 

void initialize_street(street_t *street, char *name, int red_time, int green_time, int carQdepth, int green) { 
    strcpy(street->streetname, name);  

    street->redlight_time = red_time;  

    street->greenlight_time = green_time;  

    street->carQdepth = carQdepth;  

    street->crcount = 0;  

    street->Qfront = NULL;  

    street->Qrear = NULL; 

    street->timer = 0;  

    street->is_green = green; // Start with green light if 1
}
//Andrey 12/6/2024
int rollDice(int numRange){
    return rand() % numRange;
}
//Mateus 12/5/2024 

int main(){ 
        
    // Andrey 12/6/2024
    
    //init randomizer
    srand(time(NULL));
    
    //Initializing streets 

    street_t lasalle2, lasalle1, farmington2, dale1, brace1, brace2, n_main1, n_main2, farmington1, s_main1, s_main2, memorial3; 

    // initialize clockwise
    initialize_street(&lasalle2, "Lasalle 2", 3, 3, 29, 1);
    
    initialize_street(&lasalle1, "Lasalle 1", 54, 25, 29, 1); 
    
    initialize_street(&farmington2, "Farmington 2", 25, 54, 40, 0);
    
    initialize_street(&dale1, "Dale 1", 3, 3, 17, 1);
    
    initialize_street(&brace1, "Brace 1", 30, 40, 43, 0);
    
    initialize_street(&brace2, "Brace 2", 3, 3, 43, 0);
    
    initialize_street(&n_main1, "N Main 1", 40, 30, 9, 1);
    
    initialize_street(&n_main2, "N Main 2", 105, 40, 9, 1);
    
    initialize_street(&farmington1, "Farmington 1", 45, 30, 40, 0); 
    
    initialize_street(&s_main1, "S Main 1", 105, 40, 54, 1);
    
    initialize_street(&s_main2, "S Main 2", 104, 135, 54, 1);
    
    initialize_street(&memorial3, "Memorial 3", 135, 104, 21, 0);

    
    //add_cars_to_queue(&lasalle1, 3); // Laselle 1 gets 3 cars  

    //add_cars_to_queue(&farmington1, 13); // Farmington 1 gets 13 cars  

    //add_cars_to_queue(&brace1, 6); // Brace 1 gets 6 cars  

    //add_cars_to_queue(&n_main2, 42); // N Main 2 gets 42 cars
    
    // Define simulation parameters
    int total_time = 10;       // Simulation runs for 10 seconds
    int cycles_per_second = 1; // 1 cycles per second
    double cycle_time = 1.0 / cycles_per_second; // Time per cycle in seconds

    // Start the timer
    clock_t start_time = clock();
    clock_t current_time = start_time;

    int cycle_number = 0; // Tracks the current cycle number

    printf("Starting game simulation for %d seconds at %d cycles per second...\n",
           total_time, cycles_per_second);

    while (((double)(current_time - start_time) / CLOCKS_PER_SEC) < total_time) {
        
        // Run a game cycle
        printf("Running game cycle #%d\n", cycle_number);
        int result;
        //50/50 chance
        result = rollDice(2);
        if (result == 0) {
            go(&lasalle2, NULL);
        } else {
            go(&lasalle2, &memorial3);
        }
        //33% chance
        result = rollDice(3);
        if (result == 0) {
            go(&lasalle1, NULL);
        } else if(result == 1) {
            go(&lasalle1, &dale1);
        } else {
            go(&lasalle1, &farmington1);
        }
        //33% chance
        result = rollDice(3);
        if (result == 0) {
            go(&farmington2, NULL);
        } else if(result == 1) {
            go(&farmington2, &dale1);
        } else {
            go(&farmington2, &lasalle2);
        }
        //50/50 chance
        result = rollDice(2);
        if (result == 0) {
            go(&dale1, NULL);
        } else {
            go(&dale1, &brace1);
        }
        //50/50 chance
        result = rollDice(2);
        if (result == 0) {
            go(&brace1, NULL);
        } else {
            go(&brace1, &n_main2);
        }
        //100%
        go(&brace2, NULL);
        //50/50 chance
        result = rollDice(2);
        if (result == 0) {
            go(&n_main1, NULL);
        } else {
            go(&n_main1, &brace2);
        }
        //33% chance
        result = rollDice(3);
        if (result == 0) {
            go(&n_main2, NULL);
        } else if(result == 1) {
            go(&n_main2, &farmington2);
        } else {
            go(&n_main2, &s_main2);
        }
        //33% chance
        result = rollDice(3);
        if (result == 0) {
            go(&farmington1, NULL);
        } else if(result == 1) {
            go(&farmington1, &n_main1);
        } else {
            go(&farmington1, &s_main2);
        }
        //33% chance
        result = rollDice(3);
        if (result == 0) {
            go(&s_main1, NULL);
        } else if(result == 1) {
            go(&s_main1, &n_main1);
        } else {
            go(&s_main1, &farmington2);
        }
        //100%
        go(&s_main2, NULL);
        //50/50 chance
        result = rollDice(2);
        if (result == 0) {
            go(&memorial3, NULL);
        } else {
            go(&memorial3, &s_main1);
        }
        
        add_cars_to_queue(&lasalle2, 2);
        add_cars_to_queue(&dale1, 2);
        add_cars_to_queue(&farmington2, 2);
        add_cars_to_queue(&brace2, 2);
        add_cars_to_queue(&n_main1, 2);
        add_cars_to_queue(&n_main2, 2);
        add_cars_to_queue(&farmington1, 2);
        add_cars_to_queue(&s_main1, 2);
        
        lighttimer(&lasalle2);
        lighttimer(&lasalle1);
        lighttimer(&farmington2);
        lighttimer(&dale1);
        lighttimer(&brace1);
        lighttimer(&brace2);
        lighttimer(&n_main1);
        lighttimer(&n_main2);
        lighttimer(&farmington1);
        lighttimer(&s_main1);
        lighttimer(&s_main2);
        lighttimer(&memorial3);
        
        cycle_number++;

        // Wait for the next cycle
        current_time = clock();
        while (((double)(clock() - current_time) / CLOCKS_PER_SEC) < cycle_time) {
            // Busy-wait until the next cycle (can be replaced with sleep for better CPU usage)
        }

        // Update the current time
        current_time = clock();
        
        //clear screen 
        system("clear");
    }

    printf("Simulation complete. Total cycles run: %d\n", cycle_number);

    return 0;
}
