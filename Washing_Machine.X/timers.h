/* 
 * File:   timers.h
 * Author: CFT
 *
 * Created on 28 April, 2020, 6:42 PM
 */

#ifndef TIMERS_H
#define	TIMERS_H

#define PEIE            PEIE
#define GIE             GIE

#define T2CKPS0         T2CKPS0
#define T2CKPS1         T2CKPS1
#define PR2             PR2
#define TMR2IE          TMR2IE
#define TMR2ON          TMR2ON
#define LED1            RD0
#define TMR2IF          TMR2IF

void init_timer0(void);
void init_timer2(void);

#endif	/* TIMERS_H */

