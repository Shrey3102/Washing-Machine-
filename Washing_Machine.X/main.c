/*
 * File:   main.c
 * Author: Shreyas
 *
 * Created on 27 March, 2024, 12:04 PM
 */


#include <xc.h>
#include "digital_keypad.h"
#include "clcd.h"
#include "main.h"
#include "timers.h"

#pragma config WDTE = OFF //*watch dog timer

void init_config(void) 
{
    /*initialisation of Digital Keypad*/
    init_digital_keypad();
    
    /*initaliastion of the clcd*/
    init_clcd();
    
    
    /*config RC2 to FAN as output pin*/
    FAN_DDR = 0;
    
    /* config RC1 to BUZZER as output pin*/
    BUZZER_DDR = 0;
    BUZZER = OFF;
    
    init_timer2();
    PEIE = 1;
    GIE = 1;
}

/*initialising operation mode of the washing program*/
unsigned char operation_mode = WASHING_PROGRAM_DISPLAY ;
unsigned char reset_mode = 0;
unsigned char program_no = 0, water_level_index = 0;
unsigned char *washing_prog[] = {"Daily", "Heavy", "Delicates", "Whites", "Stainwash", "Eco Cottons", "Woolens", "Bedsheets","Rinse+Dry", "Dry Only", "Wash Only", "Aqua Store" };/*array of pointers/ strings */
unsigned char *water_level_options[] = {"Auto", "Low", "Medium", "High", "Max"};
unsigned char min, sec;
unsigned int rinse_time, wash_time, spin_time;


void main(void) {
    init_config();
    unsigned char key = read_digital_keypad(STATE);
    
    
    clcd_print("Press Key 5 To", LINE1(1));
    clcd_print("Power ON", LINE2(4));
    clcd_print("Washing Machine", LINE3(1));
    
    /*keep waiting till the switch 5 is pressed*/
    while(read_digital_keypad(STATE) != SW5)
    {
        for(unsigned int wait = 3000; wait--; ); 
    }
    
    power_on_screen();
    
    while(1)
    {
        key = read_digital_keypad(STATE); /*SW5, SW6,....ALL_RELEASED*/
        
        if(key == LSW4 && operation_mode == WASHING_PROGRAM_DISPLAY)
        { /*if SW4 is pressed for long ti=me if the screen is washing mode*/
            operation_mode = WATER_LEVEL; 
            reset_mode = WATER_LEVEL_RESET;
        }
        
        //if DW4 is pressed for long time, operation mode is water level screen
        else if(key == LSW4 && operation_mode == WATER_LEVEL)
        {
            set_timer();
            //change operation mode to start and stop 
            operation_mode = START_STOP;
            clcd_clear();
            clcd_print("Press Switch", LINE1(1));
            clcd_print("SW5: START", LINE2(1));
            clcd_print("SW6: STOP", LINE3(1));             
        }
        
        //operation mode is paused and SW5 is pressed then start the machine
        if((key == SW5) && operation_mode == PAUSE)
        {
            TMR2ON = 1;
            FAN = ON;
            operation_mode = START_PROGRAM;
        }
        switch(operation_mode)
        {
            case WASHING_PROGRAM_DISPLAY:
                washing_program_display(key);
                break;
            case WATER_LEVEL:
                water_level_display(key);
                break;
            case START_STOP:
                if(key == SW5)
                {
                    operation_mode = START_PROGRAM;
                    reset_mode = START_PROGRAM_RESET;
                    continue;
                }
                else if(key == SW6)  
                {
                    operation_mode = WASHING_PROGRAM_DISPLAY ;
                    reset_mode = WASHING_PROGRAM_DISPLAY_RESET;
                    continue;
                }
                break;
            case START_PROGRAM:
                run_program(key);
                break;
        }
        reset_mode = RESET_NOTHING;
        
    }
    
    
}


void power_on_screen(void)
{
    /*printing the blocks*/
    for(unsigned char i =0; i<16; i++)
    {
        clcd_putch(BLOCK,LINE1(i));
    }
    
    clcd_print("Powering ON", LINE2(2));
    clcd_print("Washing Machine", LINE3(1));
    
     for(unsigned char i =0; i<16; i++)
    {
        clcd_putch(BLOCK,LINE4(i));
    }
    __delay_ms(1000);
    
    clcd_clear();
}

void washing_program_display(unsigned char key)
 {
     if(reset_mode == WASHING_PROGRAM_DISPLAY_RESET)
     {
         /*only for the first time when function is called*/
         clcd_clear();
         program_no = 0;
     }
     
     /*to check if SW4 is pressed increament the index of the program*/
     if (key == SW4)
     {
         clcd_clear();
         program_no++;
         if(program_no == 12)
         {
            program_no = 0;
         }
     }
     clcd_print("washing Program", LINE1(0));
     clcd_putch('*', LINE2(0));
     
     /*12 program options*/
     if(program_no <= 9)
     {
        clcd_print(washing_prog[program_no]    , LINE2(2));
        clcd_print(washing_prog[program_no + 1], LINE3(2));
        clcd_print(washing_prog[program_no + 2], LINE4(2));
     }
     else if(program_no == 10)
     {
        clcd_print(washing_prog[program_no]    , LINE2(2));
        clcd_print(washing_prog[program_no + 1], LINE3(2));
        clcd_print(washing_prog[0]             , LINE4(2));
     }
      else if(program_no == 11)
     {
        clcd_print(washing_prog[program_no]    , LINE2(2));
        clcd_print(washing_prog[0]             , LINE3(2));
        clcd_print(washing_prog[1]             , LINE4(2));
     }
 }

void water_level_display(unsigned char key)
{
    if(reset_mode == WATER_LEVEL_RESET)
    {
        water_level_index = 0;
        clcd_clear();
        
    }
    
    if(key == SW4)
    {
        water_level_index++;
        
        if(water_level_index ==5)
        {
            water_level_index = 0;
        }
        clcd_clear();
        
        //increament the water level options
    }
    
    // print the options on sw press
    clcd_print("Water Level:", LINE1(0));
    clcd_putch('*', LINE2(0));
    
    //Auto Low Medium
    if(water_level_index <=2)
    {
        clcd_print(water_level_options[water_level_index], LINE2(2));
        clcd_print(water_level_options[water_level_index + 1], LINE3(2));
        clcd_print(water_level_options[water_level_index + 2], LINE4(2));
    }
    
    else if(water_level_index == 3)
    {
        clcd_print(water_level_options[water_level_index], LINE2(2));
        clcd_print(water_level_options[water_level_index + 1], LINE3(2));
        clcd_print(water_level_options[1], LINE4(2));
    }
    
    else if(water_level_index == 4)
    {
        clcd_print(water_level_options[water_level_index], LINE2(2));
        clcd_print(water_level_options[0], LINE3(2));
        clcd_print(water_level_options[1], LINE4(2));
    }
}

void set_timer(void)
{
    switch(program_no)
    {
        //daily
        case 0:
            switch(water_level_index)
            {
                //low
                case 1:
                    sec = 33;
                    min = 0;
                    break;
                case 0:
                case 2:
                    sec = 44;
                    min = 0;
                    break;
                    //high
                case 3:
                case 4:
                    sec = 45;
                    min = 0;
                    break;
            }
            break;
            //heavy
        case 1:
            switch(water_level_index)
            {
                //low
                case 1:
                    sec = 43;
                    min = 0;
                    break;
                    //auto medium
                case 0:
                case 2:
                    sec = 50;
                    min = 0;
                    break;
                    //high max
                case 3:
                case 4:
                    sec = 57;
                    min = 0;
                    break;
            } 
            break;
        case 2:
            switch(water_level_index)
            {
                //low medium auto
                case 1:
                case 0:
                case 2:
                    sec = 26;
                    min = 0;
                    break;
                    //high
                case 3:
                case 4:
                    sec = 31;
                    min = 0;
                    break;
            }
        case 3://whites
        {
            sec = 16;
            min = 1;
            break;
        }
        case 4:
        {//stain wash
            sec = 36;
            min = 1;
            break;
        }
        case 5:
        {//eco cottons
            sec = 36;
            min = 0;
            break;
        }
        case 6:
        {// wollens
            sec = 29;
            min = 0;
            break;
        }
        case 7:
        switch(water_level_index)
            {
                //low medium auto
                case 1:
                    sec = 46;
                    min = 0;
                    break;
                case 0:
                case 2:
                    sec = 53;
                    min = 0;
                    break;
                    //high
                case 3:
                case 4:
                    sec = 0;
                    min = 1;
                    break;
            }
        break;
    
        case 8: //rinse and dry
            switch(water_level_index)
            {
                //low medium auto
                case 1:
                    sec = 18;
                    min = 0;
                    break;
                case 0:
                case 2:
                case 3:
                case 4:
                    sec = 20;
                    min = 0;
                    break;
            }
            break;
        case 9:   // dru
            sec = 6;
            min = 0;
            break;            
        case 10: //
        case 11:
            switch(water_level_index)
            {
                //low medium auto
                case 1:
                    sec = 16;
                    min = 0;
                    break;
                case 0:
                case 2:
                    sec = 21;
                    min = 0;
                    break;
                case 3:
                case 4:
                    sec = 26;
                    min = 0;
                    break;
            }
    }
}

void run_program(unsigned char key)
{
    door_status_check();
    static int total_time, time;
    if(reset_mode == START_PROGRAM_RESET)
    {
        set_timer();
        clcd_clear();
        clcd_print("Prog: ", LINE1(0));
        clcd_print(washing_prog[program_no], LINE1(6));
        clcd_print("Time: ", LINE2(0));
                // displaying the time in seconds and minutes
        clcd_putch((min/10) + '0', LINE2(6));
        clcd_putch((min%10) + '0', LINE2(7));
        clcd_putch(':', LINE2(8));
        clcd_putch((sec/10) + '0', LINE2(9));
        clcd_putch((sec/10) + '0', LINE2(10));
        clcd_print("(MM:SS)" , LINE3(5));
        
        __delay_ms(2000); //2 sec
        
        clcd_clear();
        clcd_print("Function - ", LINE1(0));
        clcd_print("TIME: ", LINE2(0));
        clcd_print("5-START  6-PAUSE", LINE4(0));
        
        time = total_time = (min * 60) + sec;
        wash_time = (int)total_time * 0.46; //we type casted from float to int
        rinse_time = (int)total_time * 0.12;
        spin_time = total_time - wash_time - rinse_time;
        
        /*to turn ON the FAN*/
        FAN = ON;
        /*turn on the timer*/
        TMR2ON = 1; 
    }
    
    if(key == SW6)
    {
        FAN = OFF;
        TMR2ON = 0;
        operation_mode = PAUSE;
        
    }
    total_time = (min * 60) + sec;
    if(program_no <= 7)
    {
        if(total_time >= (time - wash_time))
        {
            clcd_print("Wash", LINE1(11));
            
        } 
        else if(total_time >= (time - wash_time - spin_time))
        {
            clcd_print("Rinse", LINE1(11));
            
        }
        else
        {
            clcd_print("Spin ", LINE1(11));
        }
    }
    else if(program_no == 8) //rinse +dry // 40 rinse// 60wash
    {
        if(total_time >= (time = (0.40 * time)))  
        {
            clcd_print("Rinse", LINE1(11));
            clcd_clear();
        }
        else 
        {
            clcd_print("Spin ", LINE1(11));
        }      
    }
    
    else if(program_no == 9)
    {
        clcd_print("Spin ", LINE1(11));
    }
    else
    {
        clcd_print("Wash", LINE1(11));

    }
    
    clcd_putch((min/10) + '0', LINE2(6));
    clcd_putch((min%10) + '0', LINE2(7));
    clcd_putch(':', LINE2(8));
    clcd_putch((sec/10) + '0', LINE2(9));
    clcd_putch((sec%10) + '0', LINE2(10));
    
    
    if((sec == 0) && (min == 0))
    {
        FAN = OFF;
        TMR2ON = OFF;
        
        for (unsigned int wait = 50000; wait--;);
        BUZZER = ON;
        
        clcd_clear();
        clcd_print("Prog. Completed", LINE1(0));
        clcd_print("Remove Clothes", LINE2(0));
        __delay_ms(3000);
        BUZZER = OFF;
        
        operation_mode = WASHING_PROGRAM_DISPLAY ;
        reset_mode = WASHING_PROGRAM_DISPLAY_RESET;
        clcd_clear();
    }
}

void door_status_check(void)
{
    if(RB0 == 0) //if door is open
    {
        //stop the machine and turn on the buzzer
        FAN = OFF;
        TMR2ON = OFF;
        BUZZER = ON;
        clcd_clear();
        clcd_print("Door : open", LINE1(0));
        clcd_print("Please Close", LINE2(0));
        while(RB0 == 0)
        {
            ;
        }
        clcd_clear();
        clcd_print("Function - ", LINE1(0));
        clcd_print("TIME: ", LINE2(0));
        clcd_print("5-START  6-PAUSE", LINE4(0));
        FAN = ON;
        BUZZER = OFF;
        TMR2ON = ON;
    }
}