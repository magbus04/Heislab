#pragma once
/**
 * @file
 * @brief Funksjoner som kommuniserer med hardware
 */


#define N_FLOORS 4

typedef enum { 
    DIRN_DOWN   = -1,
    DIRN_STOP   = 0,
    DIRN_UP     = 1
} MotorDirection;


#define N_BUTTONS 3

typedef enum { 
    BUTTON_HALL_UP      = 0,
    BUTTON_HALL_DOWN    = 1,
    BUTTON_CAB          = 2
} ButtonType;

/**
 * @brief Initialiserer heisen. 
 * 
 */
void elevio_init(void);
/**
 * @brief Setter retning på motoren
 */
void elevio_motorDirection(MotorDirection dirn);
/**
 * @brief Setter lys i knappen
 */
void elevio_buttonLamp(int floor, ButtonType button, int value);
/**
 * @brief Setter lys i etasjelyset
 */
void elevio_floorIndicator(int floor);
/**
 * @brief Setter lys i dørlampen
 */
void elevio_doorOpenLamp(int value);
/**
 * @brief Setter lys i stopplampen
 */
void elevio_stopLamp(int value);
/**
 * @brief Leser av knappetrykk
 */
int elevio_callButton(int floor, ButtonType button);
/**
 * @brief Leser av etasje
 */
int elevio_floorSensor(void);
/**
 * @brief Leser av stoppknapp
 */
int elevio_stopButton(void);
/**
 * @brief Leser av obstruksjonsbryter
 */
int elevio_obstruction(void);

