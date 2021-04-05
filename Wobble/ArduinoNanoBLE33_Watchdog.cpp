#include "ArduinoNanoBLE33_Watchdog.h"
/*
 * Author: Acellama88
 * Description: Wrapper class for enabling the Watchdog (WDT) timer on Nano 33 BLE.
 * Date: 4/4/21
 */

/*
 * Init function to set back to defaults after constructor has ran.
 * WDT must be disabled before modifying the value.
 */
bool Nano33Watchdog::init()
{
	if(running)
		return false;
	NRF_WDT->CONFIG         = 0x01;     // Configure WDT to run when CPU is asleep
	CRV = 3932159; // 120 seconds
	NRF_WDT->CRV            = CRV;  // Timeout set to 120 seconds, timeout[s] = (CRV-1)/32768
	return true;
}

/*
 * Sets the CRV value of the WDT by passing in Seconds
 * Assumption: CRV will be less than 2^32
 * WDT must be disabled before modifying the value.
 */
bool Nano33Watchdog::setTimerSeconds(uint seconds)
{
	if(running)
		return false;
	CRV =  seconds * CLOCK_MHz + 1;
	NRF_WDT->CRV = CRV;
	return true;
}

/*
 * Sets the CRV value of the WDT by passing in Milliseconds
 * Assumption: CRV will be less than 2^32
 * WDT must be disabled before modifying the value.
 */
bool Nano33Watchdog::setTimerMilliseconds(uint milliseconds)
{
	if(running)
		return false;
	CRV =  (milliseconds * CLOCK_MHz) / 1000 + 1;  //Math done multiplying first to maintain precision, but could more easily overflow.
	NRF_WDT->CRV = CRV;
	return true;
}

/*
 * Directly sets the CRV value of the WDT
 * Assumption: CRV will be less than 2^32
 * WDT must be disabled before modifying the value.
 */
bool Nano33Watchdog::setCRV(uint newCRV)
{
	if(running)
		return false;
	CRV = newCRV;
	NRF_WDT->CRV = CRV;
	return true;
}

/*
 * Returns the CRV represented in seconds.
 * Assumption: CRV was created based on seconds
 */
uint Nano33Watchdog::getTimerSeconds()
{
	return (CRV - 1) / CLOCK_MHz;
}

/*
 * Returns the CRV represented in milliseconds.
 * Assumption: CRV was created based on milliseconds
 */
uint Nano33Watchdog::getTimerMilliseconds()
{
	return (CRV - 1) * 1000 / CLOCK_MHz; //Math done multiplying first to maintain precision, but could more easily overflow.
}

/*
 * Returns the CRV value
 */
uint Nano33Watchdog::getCRV()
{
	return CRV;
}

/*
 * Reloads the WDT
 */
void Nano33Watchdog::kick()
{
	if(running)
		NRF_WDT->RR[0] = WDT_RR_RR_Reload;

}
