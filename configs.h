#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#define abs(X) ((X < 0) ? -1 * X : X)

/**
* PINS CONFIGURATION
*/
#define MOT_A_1_PIN 4
#define MOT_A_PWM_PIN 5

#define MOT_B_1_PIN 7
#define MOT_B_PWM_PIN 6

#define SRV_PIN 9
#define SRV_UP_ANGLE 35
#define SRV_DW_ANGLE 0

#define BT_TX_PIN 3
#define BT_RX_PIN 2

#define STEPS_CM   200
#define STEPS_DEG 100
#define ROBOT_SPEED_DELAY 10

#endif /* __CONFIGS_H__ */

