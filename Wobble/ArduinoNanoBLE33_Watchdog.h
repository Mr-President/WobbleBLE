#ifndef NANO_33_WDT_H__
#define NANO_33_WDT_H__

#include <nrfx.h>

/************************
 * Known Limits and Parameters for the WDT
 */

#define MAX_CRV 0xFFFFFFFF  //unused - MAX CRV Value
#define CLOCK_MHz 32768

class Nano33Watchdog
{
	public:

	Nano33Watchdog()
	{
		running = false;
    this->init();
	}
	
	
	bool init();
	
	bool begin()
	{
		NRF_WDT->TASKS_START    = 1;        // Start WDT
		running = true;
	}
	
	bool end()
	{
		NRF_WDT->TASKS_START 	= 0;		// End WDT
		running = false;
	}
	
	bool setTimerSeconds(uint seconds);	
	bool setTimerMilliseconds(uint milliseconds);
	bool setCRV(uint newCRV);
	uint getTimerSeconds();
	uint getTimerMilliseconds();
	uint getCRV();
	void kick();
	
	private:
	uint CRV;
	bool running;
	
};



#endif
