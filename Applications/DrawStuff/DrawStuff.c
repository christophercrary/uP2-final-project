
#include "DrawStuff.h"
#include "MuMessage.h"
    /************************************************************************************
 * Name: aperiodic_mumessage_draw_image
 * Purpose:
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_mumessage_draw(void)
{
    uint8_t status;     // used to identify which P4 pin triggered the ISR (pins 4 or 5)

    status = P4IV;      // P4IV = (2*(n+1)) where n is the pin number of the
                        // lowest bit with a pending interrupt.
                        // this access will only clear flag n.

//    /* the following check is probably in the wrong place (no debounce has happened yet) */
//    // check if triggering switch is high after debounce time
//    if ( ( P4->IN & ( 1 << (status/2) - 1 ) ) == ( 1 << (status/2) - 1 ) )
//    {
//        return;     // ISR improperly called
//    }
//
//    // otherwise, interrupt was properly called

    // if LCD TP triggered interrupt (port 4, pin 0)
    if (status == 2)        //  2 = 2*(0+2)
    {
        // disable pin 0 interrupt (will be rearmed after 0.5 seconds) (IMPLEMENT!)
        GPIO_disableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

        // create thread to check LCD screen press within Compose Message
        G8RTOS_add_thread(thread_mumessage_image_draw_check_TP, 100, "TP - home screen");
    }
}


/************************************************************************************
* Name: thread_muphone_home_screen_check_TP
* Purpose: Thread to check if touch made to LCD TouchPanel interacted with LCD TP
*          as to draw on screen.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_image_draw_check_TP(void)
{

    while(1)
    {


    // otherwise, touch was properly made to TouchPanel LCD
        Point touch = TP_ReadXY();
        // first debounce LCD
        G8RTOS_thread_sleep(30);

        // check if triggering switch is high after debounce time
        if ( P4->IN & PIN_TOUCHPANEL)
        {
           // G8RTOS_kill_current_thread();     // thread improperly called
        }

        LCD_DrawRectangle(touch.x - 1, touch.x + 1, touch.y - 1, touch.y + 1,
                          LCD_BLACK);
        //LCD_SetPoint(touch.x, touch.y, LCD_BLACK);      // test pixel draw

        // re-enable LCD Touch Panel interrupt and clear interrupt flag
        GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);
        GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN0);

    }
    // done handling TP touches, kill self
    G8RTOS_kill_current_thread();

}


