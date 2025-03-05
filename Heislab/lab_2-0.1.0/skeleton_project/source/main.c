#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "test.h"

// Kalibreringsfunksjon: Få heisen til å komme til en definert etasje ved oppstart
void calibrateElevator() {
    // Beveg heisen nedover til en gyldig floor
    elevio_motorDirection(DIRN_DOWN);
    int floor;
    while (1) {
        floor = elevio_floorSensor();
        printf("Current floor: %d\n", floor); // Debug print statement

        if (floor != -1) {
            // Kalibrering ferdig
            calibrated = 1;
            elevio_motorDirection(DIRN_STOP);
            elevio_floorIndicator(floor);
            lastFloor = floor;
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
            int currentFloor = elevio_floorSensor();
            if (currentFloor == f && btnStatus) {
                openDoor();
                goto skip_order;
            }
            if (btnStatus) {
                orders[f][b] = 1;
                elevio_buttonLamp(f, b, 1);
            }
            skip_order:
            continue;
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
int checkButton(int floor, int b) {
        return orders[floor][b];
    }

// Fjerner alle ventende bestillinger
void clearAllOrders(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        clearOrdersAtFloor(f);
    }
}

int ordersBelow(int currentFloor) {
for (int b = 0; b <= 2; b+=2) {
    for (int f = currentFloor - 1; f >= 0; f--) {
        if (checkButton(f,b)) {
            return 1;
        }
    }
}
return 0;
}
int ordersAbove(int currentFloor) {
    for (int b = 1; b <= 2; b++) {
        for (int f = currentFloor + 1; f < N_FLOORS; f++) {
            if (checkButton(f,b)) {
                return 1;
            }
        }
    }
    return 0;
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

    if (currentDirection == DIRN_UP && orderAbove) {
        return DIRN_UP;
    }

    if (currentDirection == DIRN_DOWN && orderBelow) {
        return DIRN_DOWN;
    }

    if (orderAbove) {
        return DIRN_UP;
    }

    if (orderBelow) {
        return DIRN_DOWN;
    }

    return DIRN_STOP;
    }

// Åpner døren i 3 sekunder. Hvis obstruksjon eller stoppknappen aktiveres, fornyes timeren
void openDoor(void) {
    elevio_doorOpenLamp(1);
    time_t start = time(NULL);
    while (time(NULL) - start < DOOR_OPEN_TIME) {
        if (elevio_stopButton() == 0) {
            elevio_stopLamp(0);
        }
        if (elevio_obstruction() || elevio_stopButton()) {
            start = time(NULL);
        }
        updateOrders();
        nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
    }
    elevio_doorOpenLamp(0);
}

int main(void) {
    // Starter heissystemet
    elevio_init();
    state = STATE_IDLE;
    currentDirection = DIRN_STOP;
    calibrated = 0;

    printf("=== Elevator system test ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    while (1) {
        // Sikkerhet: Håndter stoppknappen
        if (elevio_stopButton()) {
            state = STATE_EMERGENCY_STOP;
            elevio_stopLamp(1);
            elevio_motorDirection(DIRN_STOP);
            clearAllOrders();
    
            // Dersom heisen er i en etasje, åpnes døren
            int currentFloor = elevio_floorSensor();
            if (currentFloor != -1) {
                openDoor();
            }
            currentFloor = lastFloor;
            // Vent til stoppknappen slippes
            while (elevio_stopButton()) {
                nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
            }
            elevio_stopLamp(0);
            elevio_doorOpenLamp(0);
            state = STATE_IDLE;
            
        }
        
        // Kalibreringsfase: Ignorer bestillinger før heisen er i en definert tilstand
        if (calibrated == 0) {
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
            lastFloor = currentFloor;
        }
        
        // Tilstandsmaskin for heisens oppførsel
        switch (state) {
            case STATE_IDLE: {
                // Dersom det finnes ventende bestillinger, velg retning og begynn å bevege deg
                MotorDirection nextDir = chooseDirection(lastFloor);
                if (nextDir != DIRN_STOP) {
                    currentDirection = nextDir;
                    elevio_motorDirection(currentDirection);
                    state = STATE_MOVING;
                }
                break;
            }
            case STATE_MOVING: {
                if (currentFloor != -1) {
                    elevio_floorIndicator(currentFloor);
                    lastFloor = currentFloor;
                
                // Ved ankomst til etasje med en bestilling, stopp og åpne døren
                if (currentFloor != -1 && ordersAtFloor(currentFloor)){
                    if ((currentDirection == DIRN_UP && (orders[currentFloor][BUTTON_HALL_UP] || orders[currentFloor][BUTTON_CAB] || (orders[3][BUTTON_HALL_DOWN] && currentFloor == 3))) ||
                        (currentDirection == DIRN_DOWN && (orders[currentFloor][BUTTON_HALL_DOWN] || orders[currentFloor][BUTTON_CAB] || (orders[0][BUTTON_HALL_UP] && currentFloor == 0)))) {
                        elevio_motorDirection(DIRN_STOP);
                        state = STATE_DOOR_OPEN;
                        clearOrdersAtFloor(currentFloor);
                        printf("used complex if\n");
                    }
                    else {
                        printf("used else\n");
                        if (ordersAbove(currentFloor) == 1 && currentDirection == DIRN_UP){
                            printf("keeps going up\n");
                            goto skip_stop;
                        }
                        if (ordersBelow(currentFloor) == 1 && currentDirection == DIRN_DOWN){
                            printf("keeps going down\n");
                            goto skip_stop;
                        }
                        elevio_motorDirection(DIRN_STOP);
                        state = STATE_DOOR_OPEN;
                        clearOrdersAtFloor(currentFloor);
                        printf("didnt break\n");
                    }                  
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
        skip_stop:
        nanosleep(&(struct timespec){0, SLEEP_NS}, NULL);
    }
    
    return 0;
}
