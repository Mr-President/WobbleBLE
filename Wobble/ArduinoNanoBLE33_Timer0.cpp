#include "ArduinoNanoBLE33_Timer0.h"
/*
 * Author: Acellama88
 * Description: Wrapper class for enabling TIMER0 on Nano 33 BLE. This only uses the compare functionality, not the counter. 
 * This also only details Timer0.  Most of this code could be easily converted to any other timer, however,
 * this won't be expandible, as is, for multiple timers.  This should be easily upgraded to being usable for
 * multiple timers.  This also assumes you want to use the interrupt routine, and is configured automatically.
 * Date: 4/17/21
 */

/*
 * Init function to set back to defaults after constructor has ran.
 * TIM must be disabled before modifying the value.
 */
bool Nano33TIMER::init()
{
	if(running) //is it running
		return false;
    DisableInterrupt(); //Turn off the interrupt
	NRF_TIMER0->TASKS_CLEAR    = 0x01; //Stop the Timer
    NRF_TIMER0->INTENCLR = ALL_CHANNELS_INTR_MASK; //Remove all interrupt channels
    for (int i = 0; i < NUMBER_CC_COMPARES; i++)
    {
        NRF_TIMER0->CC[i] = 0x00000000;  //Default Value for CC
        callbackVector[i] = NULL;  //Remove all callback vectors
    }
    NRF_TIMER0->MODE = 0x0; //Default Value for Mode - Timer Mode (only supported mode in this implementation)
    NRF_TIMER0->BITMODE = 0x0;  //Default Value for BitMode - 16bit
    NRF_TIMER0->PRESCALER = 0x0; //Default Value for Prescaler - 16MHZ / 2^0 
	return true;
}

/*
 * Configures the Bitmode and Prescalar
 * Verifies arguments are within boundaries
 */
bool Nano33TIMER::setTimer(byte bitmode, byte prescalar)
{
	if(running)
		return false; //Don't configure if the timer is running
    else if (bitmode > BITMODE_32 || prescalar > PRESCALAR_MAX)
    {
        return false;  //Don't try and set anything if it isn't valid
    }
    NRF_TIMER0->BITMODE = bitmode; //Write bitmode to register
    Bitmode = bitmode;
    NRF_TIMER0->PRESCALER = prescalar; //Write prescalar to register
    Prescalar = prescalar;
	return true;
}

/*
 * Configured the Compare Register for the channel 
 * Verifies the compare value will fit based on bitmode and is in an appropriate channel 
 */
bool Nano33TIMER::setChannel(byte channel, uint32_t compare)
{
    if (running || channel >= NUMBER_CC_COMPARES)
		return false; //Make sure it isn't running, and that the Compare Channel is valid
    switch (Bitmode) //Verify the compare value is valid
    {
        case BITMODE_16:
            if (compare > BITMODE_16_COMPARE_MAX)
            {
                return false;
            }
            break;
        case BITMODE_8:
            if (compare > BITMODE_08_COMPARE_MAX)
            {
                return false;
            }
            break;
        case BITMODE_24:
            if (compare > BITMODE_24_COMPARE_MAX)
            {
                return false;
            }
            break;
        default:  // Don't need to check for BITMODE_32 as that can use the entire uint32_t
            break;
    }
    switch (channel)  //Enable the Interrupt for the channel
    {
        case 0:
            NRF_TIMER0->INTENSET = TIM_CHAN0_INTR_MASK;
            break;
        case 1:
            NRF_TIMER0->INTENSET = TIM_CHAN1_INTR_MASK;
            break;
        case 2:
            NRF_TIMER0->INTENSET = TIM_CHAN2_INTR_MASK;
            break;
        case 3:
            NRF_TIMER0->INTENSET = TIM_CHAN3_INTR_MASK;
            break;
        case 4:
            NRF_TIMER0->INTENSET = TIM_CHAN4_INTR_MASK;
            break;
        case 5:
            NRF_TIMER0->INTENSET = TIM_CHAN5_INTR_MASK;
            break;
    }
    NRF_TIMER0->CC[channel] = compare;  //Write compare to register
    Compare = compare;
	return true;
}

/*
 * Clears and disables the interrupt for the Channel comparator
 */
bool Nano33TIMER::clearChannel(byte channel)
{
    if (running || channel >= NUMBER_CC_COMPARES) 
		return false; //Make sure it isn't running, and that the Compare Channel is valid
    switch (channel)  //Disable the interrupt for the Channel
    {
        case 0:
            NRF_TIMER0->INTENCLR = TIM_CHAN0_INTR_MASK;
            break;
        case 1:
            NRF_TIMER0->INTENCLR = TIM_CHAN1_INTR_MASK;
            break;
        case 2:
            NRF_TIMER0->INTENCLR = TIM_CHAN2_INTR_MASK;
            break;
        case 3:
            NRF_TIMER0->INTENCLR = TIM_CHAN3_INTR_MASK;
            break;
        case 4:
            NRF_TIMER0->INTENCLR = TIM_CHAN4_INTR_MASK;
            break;
        case 5:
            NRF_TIMER0->INTENCLR = TIM_CHAN5_INTR_MASK;
            break;
    }
    NRF_TIMER0->CC[channel] = 0;  //Clear the comparator for the channel
    Compare = 0;
	return true;
}

/*
 * Function to call if the timer hits the compare register for that channel
 */
void Nano33TIMER::RegisterCallback(void (*func)(void), byte channel)
{
    if ((func == NULL) || (channel >= NUMBER_CC_COMPARES))
    {
        return;
    }
    callbackVector[channel] = func;
}

/*
 * Remove the callback function for that channel
 */
void Nano33TIMER::DeregisterCallback(byte channel)
{
    if (channel >= NUMBER_CC_COMPARES)
    {
        return;
    }
    callbackVector[channel] = NULL;
}

/*
 * Function called from Interrupt Handler that selects which function to run based on configured callback function.
 */
void Nano33TIMER::TIMER0_ISR(void)
{
    for (int i = 0; i < NUMBER_CC_COMPARES; i++)
    {
        if (NRF_TIMER0->EVENTS_COMPARE[i])
        {
            NRF_TIMER0->EVENTS_COMPARE[i] = 0;
            (void)NRF_TIMER0->EVENTS_COMPARE[i];
            if(callbackVector[i] != NULL)
                (*callbackVector[i])();
        }
    }
}

/*
* Include the following into your code
* Change "TIMER0" to the timer you are using
* Change OBJECT_NAME to the name of your Nano33TIMER
*/

/*
extern "C"
{
    void TIMER0_IRQHandler_v (void)
    {
        OBJECT_NAME.TIMER0_ISR();
    }
}
*/
