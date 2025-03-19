#pragma once

#include "driver/elevio.h"

#define DOOR_OPEN_TIME 3            // Døråpentid: 3 sekunder
#define SLEEP_NS (20 * 1000 * 1000)   // 20 ms pause i loopen

// Globale variabler for bestillinger og kalibrering
int orders[N_FLOORS][N_BUTTONS] = { { 0 } };
int calibrated = 0;
int lastFloor = 0;
/**
 * @file
 * @brief The main file for the elevator project. 
 */

// Definerer heisens tilstander
typedef enum {
    STATE_CALIBRATING,
    STATE_IDLE,
    STATE_MOVING,
    STATE_DOOR_OPEN,
    STATE_EMERGENCY_STOP
} ElevatorState;

ElevatorState state = STATE_CALIBRATING;
MotorDirection currentDirection = DIRN_STOP;

// Funksjonsprototyper
/**
 * @brief Kalibreringsfunksjon: Få heisen til å komme til en definert etasje ved oppstart
 * 
 */
void calibrateElevator();
/**
 * @brief Oppdaterer bestillingsmatrisen ved å sjekke alle knapper
 * 
 */
void updateOrders(void);
/**
 * @brief Sjekker om det finnes en bestilling i en spesifikk etasje
 * 
 * @param floor Etasjen som skal sjekkes
 * @return int 1 hvis det finnes en bestilling i order-matrisen, 0 ellers
 */
int ordersAtFloor(int floor);
/**
 * @brief Fjerner bestillinger for en etasje og slukker knappelysene
 * 
 * @param floor Etasjen som skal tømmes for bestillinger
 */
void clearOrdersAtFloor(int floor);
/**
 * @brief Sjekker en knapp på en etasje
 * 
 * @param floor Etasjen som skal sjekkes
 * @param b Knappen som skal sjekkes
 * @return int 1 hvis bestillingen er gyldig, 0 ellers
 */
int checkbutton(int floor, int b);
/**
 * @brief Fjerner alle bestillinger og slukker alle knappelys
 * 
 */
void clearAllOrders(void);
/**
 * @brief Sjekker om det finnes bestillinger under en etasje
 * 
 * @param currentFloor Etasjen som skal sjekkes
 * @return int 1 hvis det finnes bestillinger under etasjen, 0 ellers
 */
int ordersBelow(int currentFloor);
/**
 * @brief Sjekker om det finnes bestillinger over en etasje
 * 
 * @param currentFloor Etasjen som skal sjekkes
 * @return int 1 hvis det finnes bestillinger over etasjen, 0 ellers
 */
int ordersAbove(int currentFloor);
/**
 * @brief Velger retning basert på ventende bestillinger og den nåværende etasjen
 * 
 * @param currentFloor Nåværende etasje
 * @return MotorDirection Retning som heisen skal bevege seg
 */
MotorDirection chooseDirection(int currentFloor);
/**
 * @brief Åpner døren i 3 sekunder
 * 
 */
void openDoor(void);
