#ifndef NANO_33_TIM_H__
#define NANO_33_TIM_H__

#include <Arduino.h>

/*
 * Known Limits and Parameters for the TIMER
 */

#define MAX_CRV 0xFFFFFFFF  //unused - MAX CRV Value
#define CLOCK_MHz 16000000  //Clock Frequency for Compare
#define PRESCALAR_MAX 9 //Maximum value for Clock Prescalar 2^9

//TIMER Specific #defines
#define NUMBER_CC_COMPARES 4

#define TIM_CHAN0_INTR_MASK 0x00010000
#define TIM_CHAN1_INTR_MASK 0x00020000
#define TIM_CHAN2_INTR_MASK 0x00040000
#define TIM_CHAN3_INTR_MASK 0x00080000
#define TIM_CHAN4_INTR_MASK 0x00100000 //May not be used for all timers (TIMER0)
#define TIM_CHAN5_INTR_MASK 0x00200000 //May not be used for all timers (TIMER0)
#define ALL_CHANNELS_INTR_MASK 0x003F0000

//Max Compare Value based on Bitmode
#define BITMODE_32_COMPARE_MAX 0xFFFFFFFF
#define BITMODE_24_COMPARE_MAX 0x00FFFFFF
#define BITMODE_16_COMPARE_MAX 0x0000FFFF
#define BITMODE_08_COMPARE_MAX 0x000000FF

#define BITMODE_16 0
#define BITMODE_8  1
#define BITMODE_24 2
#define BITMODE_32 3

class  Nano33TIMER
{
	public:

    /*
     * Constructor
     */
	Nano33TIMER()
	{
		running = false;
        this->init();
	}

    /*
     * Is the timer Active
     */
    bool isRunning()
    {
        return running;
    }
	
    /*
     * Begin the timer
     */
	bool begin()
	{
        running = true; //Remember we are running
        EnableInterrupt(); //Turn on the interrupts
        NRF_TIMER0->TASKS_START    = 0x01; // Start the timer
        return true;
	}
	
    /*
     * Stop the timer
     */
	bool end()
	{
		NRF_TIMER0->TASKS_STOP 	= 0x01;		// Stop the timer
        DisableInterrupt(); //Turn off the interrupts
		running = false; //Remember we are stopped
        return true;
	}

    /*
     * Starts the counter back at 0, so all events can trigger again
     */
    void restartTimer()
    {
        NRF_TIMER0->TASKS_CLEAR = 0x1;
    }

    /*
     * Clear the triggered event
     */
    bool clearEvent(byte channel)
    {
        if(channel >= NUMBER_CC_COMPARES)
            return false;
        NRF_TIMER0->EVENTS_COMPARE[channel] = 0x00;
        (void)NRF_TIMER0->EVENTS_COMPARE[channel];
        return true;
    }

    /*
     * Turn on the interrupt
     * Will call:
     *      Extern "C" void TIMER0_IRQHandler_v(void)
     */
    void EnableInterrupt()
    {
        NVIC_EnableIRQ(TIMER0_IRQn);
        //NVIC_SetPriority(TIMER0_IRQn,1); //Sets the interrupt priority
    }

    /*
     * Will disable the interupt
     */
    void DisableInterrupt()
    {
        NVIC_DisableIRQ(TIMER0_IRQn);
    }

	bool init();
	bool setTimer(byte bitmode, byte prescalar);
	bool setChannel(byte channel, uint32_t compare);
    bool clearChannel(byte channel);
    void RegisterCallback(void (*func)(void), byte channel);
    void DeregisterCallback(byte channel);
    void TIMER0_ISR(void);
	
private:
    void (*callbackVector[NUMBER_CC_COMPARES])(void);
	bool running;
    byte Bitmode;
    byte Prescalar;
    uint32_t Compare; 
	
};
#endif
