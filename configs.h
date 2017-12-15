#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#define abs(X) ((X < 0) ? -1 * X : X)

/**
* PINS CONFIGURATION
*/
#define MOT_A_1_PIN 4
#define MOT_A_2_PIN 5
#define MOT_A_PWM_PIN 6

#define MOT_B_1_PIN 7
#define MOT_B_2_PIN 8
#define MOT_B_PWM_PIN 9

#define ENCODE_A_PIN 2
#define ENCODE_B_PIN 3

#define LCD_SDA_PIN 18
#define LCD_SCL_PIN 19

#define BT_TX_PIN 0
#define BT_RX_PIN 1

/* KEYMAP */
#define KEY_PIN_0 10
#define KEY_PIN_1 11
#define KEY_PIN_2 12
#define KEY_PIN_3 14
#define KEY_PIN_4 15
#define KEY_PIN_5 16
#define KEY_PIN_6 17

/**
* GENERAL CONSTANTS
*/
// LCD maps
#define NB_SYMBOL_MV 4
#define NB_STATUS 2
#define NB_MODES 2

/** 
* Symbols are mapped as follows on font u8g_font_9x15_67_75:
* Up arrow: 0x67
* Down arrow: 0x69
* Left arrow: 0x66
* Right arrow: 0x68
* Stop: 0xA0
* Run: 0xB6
*/
#define SYMBOL_FRONT 0
#define SYMBOL_BACK 1
#define SYMBOL_LEFT 2
#define SYMBOL_RIGHT 3

char moveSymbols[NB_SYMBOL_MV + NB_STATUS + 1] = { 0x67, 0x69, 0x66, 0x68, 0xA0, 0xB6, '\0' };

/**
* Symbols are mapped as follows on font u8g_font_9x15_78_79:
* Mode 1: 0x8A
* Mode 2: 0x8B
*/
#define SYMBOL_MODE_1 0
#define SYMBOL_MODE_2 1

char modeSymbols[NB_MODES + 1] = { 0x8A, 0x8B, '\0' };

// Keymaps
#define ROWS 4
#define COLS 3

char hexaKeys[ROWS][COLS] = {
    { '1',                      moveSymbols[SYMBOL_FRONT], '3' },
    { moveSymbols[SYMBOL_LEFT], '5',                        moveSymbols[SYMBOL_RIGHT] },
    { '7',                      moveSymbols[SYMBOL_BACK],   '9' },
    { '*',                      '+',                       '#' }
};

typedef struct  __attribute__( ( packed ) )
{
    int ticksPerRobot;
    int ticksPerRotation;
    int baseSpeed;
    int proportinalK;
}S_CONFIGS;

#endif /* __CONFIGS_H__ */

