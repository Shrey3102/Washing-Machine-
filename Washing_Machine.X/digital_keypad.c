#include <xc.h>
#include "digital_keypad.h"

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1, long_press;
    static unsigned char key, prev_key = 0x00;
    
    if (mode == LEVEL_DETECTION)
    {
        return KEYPAD_PORT & INPUT_LINES;
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            long_press = 0;
            prev_key = KEYPAD_PORT & INPUT_LINES;
        }
        //comaparing the current key and prev_key
        else if(!once && (prev_key == (KEYPAD_PORT & INPUT_LINES)) && long_press < 20)
        {
            long_press++;
        }
        else if(long_press == 20)
        {
            long_press++;
            //return long press key value
            return prev_key | 0x80;
        }
        else if (((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED) && !once)
        {
            once = 1;
            if(long_press < 20) // if key is not pressed for long time
            {
                return prev_key;
            }
        }
    }
    
    return ALL_RELEASED;
}
