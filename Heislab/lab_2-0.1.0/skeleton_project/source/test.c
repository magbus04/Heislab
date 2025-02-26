/*
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "test.h"

// Kalibreringsfunksjon: Få heisen til å komme til en definert etasje ved oppstart
void calibrateElevator(void) {
    // Beveg heisen nedover til en gyldig floor
    elevio_motorDirection(DIRN_DOWN);
    int floor;
    while (1) {
        floor = elevio_floorSensor();
        if (floor != -1 && floor >= 0 && floor < N_FLOORS) {
            // Kalibrering ferdig
            calibrated = 1;
            elevio_motorDirection(DIRN_STOP);
            elevio_floorIndicator(floor);
            break;
        }
        nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
    }
}

// Oppdaterer bestillingsmatrisen ved å sjekke alle knapper
void updateOrders(void) {
    // Ignorer bestillinger dersom heisen ikke er kalibrert
    if (!calibrated)
        return;
    
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            int btnStatus = elevio_callButton(f, b);
            if (btnStatus) {
                orders[f][b] = 1;
                elevio_buttonLamp(f, b, 1);
            }
        }
    }
}

// Sjekker om det finnes en bestilling i en spesifikk etasje
int ordersAtFloor(int floor) {
    for (int b = 0; b < N_BUTTONS; b++) {
        if (orders[floor][b])
            return 1;
    }
    return 0;
}

// Fjerner bestillinger for en etasje og slukker knappelysene
void clearOrdersAtFloor(int floor) {
    for (int b = 0; b < N_BUTTONS; b++) {
        orders[floor][b] = 0;
        elevio_buttonLamp(floor, b, 0);
    }
}

// Fjerner alle ventende bestillinger
void clearAllOrders(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        clearOrdersAtFloor(f);
    }
}

// Velger retning basert på ventende bestillinger og den nåværende etasjen
// Kun bestillinger i den aktuelle retningen tas med
MotorDirection chooseDirection(int currentFloor) {
    int orderAbove = 0;
    int orderBelow = 0;
    
    // Sjekk etter bestillinger over floor
    for (int f = currentFloor + 1; f < N_FLOORS; f++) {
        if (ordersAtFloor(f)) {
            orderAbove = 1;
            break;
        }
    }
    // Sjekk etter bestillinger under floor
    for (int f = 0; f < currentFloor; f++) {
        if (ordersAtFloor(f)) {
            orderBelow = 1;
            break;
        }
    }
    if (orderAbove)
        return DIRN_UP;
    if (orderBelow)
        return DIRN_DOWN;
    return DIRN_STOP;
}

// Åpner døren i 3 sekunder. Hvis obstruksjon eller stoppknappen aktiveres, fornyes timeren
void openDoor(void) {
    elevio_doorOpenLamp(1);
    time_t start = time(NULL);
    while (time(NULL) - start < DOOR_OPEN_TIME) {
        if (elevio_obstruction() || elevio_stopButton()) {
            start = time(NULL);
        }
        nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
    }
    elevio_doorOpenLamp(0);
}

int testheis(void) {
    // Starter heissystemet
    elevio_init();

    printf("=== Elevator system test ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    while (1) {
        // Sikkerhet: Håndter stoppknappen
        if (elevio_stopButton()) {
            state = STATE_EMERGENCY_STOP;
            elevio_motorDirection(DIRN_STOP);
            clearAllOrders();
            // Dersom heisen er i en etasje, åpnes døren
            int currentFloor = elevio_floorSensor();
            if (currentFloor != -1)
                elevio_doorOpenLamp(1);
            // Vent til stoppknappen slippes
            while (elevio_stopButton()) {
                nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
            }
            elevio_stopLamp(0);
            state = STATE_IDLE;
        }
        
        // Kalibreringsfase: Ignorer bestillinger før heisen er i en definert tilstand
        if (!calibrated) {
            state = STATE_CALIBRATING;
            calibrateElevator();
            state = STATE_IDLE;
            continue;
        }
        
        // Oppdater bestillinger (hvis stoppknappen ikke er aktivert)
        updateOrders();
        
        // Hent nåværende etasje og oppdater etasjelyset
        int currentFloor = elevio_floorSensor();
        if (currentFloor != -1) {
            elevio_floorIndicator(currentFloor);
        }
        
        // Tilstandsmaskin for heisens oppførsel
        switch (state) {
            case STATE_IDLE: {
                // Dersom det finnes ventende bestillinger, velg retning og begynn å bevege deg
                MotorDirection nextDir = chooseDirection(currentFloor);
                if (nextDir != DIRN_STOP) {
                    currentDirection = nextDir;
                    elevio_motorDirection(currentDirection);
                    state = STATE_MOVING;
                }
                break;
            }
            case STATE_MOVING: {
                // Ved ankomst til etasje med en bestilling, stopp og åpne døren
                if (currentFloor != -1 && ordersAtFloor(currentFloor)) {
                    elevio_motorDirection(DIRN_STOP);
                    state = STATE_DOOR_OPEN;
                    clearOrdersAtFloor(currentFloor);
                } else {
                    // Juster retningen dersom nye bestillinger dukker opp
                    MotorDirection nextDir = chooseDirection(currentFloor);
                    if (nextDir != currentDirection && nextDir != DIRN_STOP) {
                        currentDirection = nextDir;
                        elevio_motorDirection(currentDirection);
                    } else if (nextDir == DIRN_STOP) {
                        elevio_motorDirection(DIRN_STOP);
                        state = STATE_IDLE;
                    }
                }
                break;
            }
            case STATE_DOOR_OPEN: {
                openDoor();
                state = STATE_IDLE;
                break;
            }
            case STATE_EMERGENCY_STOP:
                // Denne tilstanden håndteres øverst i loopen
                break;
            case STATE_CALIBRATING:
                // Skal ikke inntreffe her siden kalibreringen allerede håndteres
                break;
        }
        
        nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
    }
    
    return 0;
}
*/