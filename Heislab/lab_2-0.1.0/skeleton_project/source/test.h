#pragma once

#include "driver/elevio.h"

#define DOOR_OPEN_TIME 3            // Døråpentid: 3 sekunder
#define SLEEP_NS (20 * 1000 * 1000)   // 20 ms pause i loopen

// Globale variabler for bestillinger og kalibrering
int orders[N_FLOORS][N_BUTTONS] = { { 0 } };
int calibrated = 0;
int lastFloor = 0;

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
void calibrateElevator();
void updateOrders(void);
int ordersAtFloor(int floor);
void clearOrdersAtFloor(int floor);
void clearAllOrders(void);
MotorDirection chooseDirection(int currentFloor);
void openDoor(void);
int testheis(void);