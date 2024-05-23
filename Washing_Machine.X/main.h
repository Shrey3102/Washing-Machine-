/* 
 * File:   main.h
 * Author: Shreyas
 *
 * Created on 27 March, 2024, 12:04 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define RB0                             RB0

#define WASHING_PROGRAM_DISPLAY         0x01
#define WATER_LEVEL                     0x02
#define START_STOP                      0x03
#define START_PROGRAM                   0x04
#define PAUSE                           0x05


#define WASHING_PROGRAM_DISPLAY_RESET   0x10
#define WATER_LEVEL_RESET               0x11
#define START_PROGRAM_RESET             0x12
#define RESET_NOTHING                   0x00

#define FAN                             RC2
#define FAN_DDR                         TRISC2
#define ON                              1
#define OFF                             0       
#define BUZZER                          RC1
#define BUZZER_DDR                      TRISC1

void washing_program_display(unsigned char key);
void power_on_screen(void);
void water_level_display(unsigned char key);
void run_program(unsigned char key);
void set_timer(void);
void door_status_check(void);

#endif	/* MAIN_H */

