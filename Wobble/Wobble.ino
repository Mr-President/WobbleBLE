#include <Nano33BLEAccelerometer.h>
#include "Nano33BLETemperature.h"
#include "ArduinoNanoBLE33_Watchdog.h"
#include <math.h>
#include <ArduinoBLE.h>
#include <Arduino.h>

/**************************************
 * Compilation Adders
 **************************************/
//#define __DEBUG__           //Adds writing to the USB Serial Port
#define __WDT__             //Adds configuring the Watchdog Timer

/**************************************
 * BLE Duty Cycle
 **************************************/
#define BLE_ON_TIME_SECONDS 5  //How long to leave on BLE Advertise
#define DELAY_TIME_SECONDS 15  //How long to wait before re-enabling BLE

/**************************************
 * Disable UART
 **************************************/
int main(void){
init();
initVariant();

//Disabling UART0 (saves around 300-500µA) - @Jul10199555 contribution
NRF_UART0->TASKS_STOPTX = 1;
NRF_UART0->TASKS_STOPRX = 1;
NRF_UART0->ENABLE = 0;

*(volatile uint32_t *)0x40002FFC = 0;
*(volatile uint32_t *)0x40002FFC;
*(volatile uint32_t *)0x40002FFC = 1; //Setting up UART registers again due to a library issue

//Removing USB CDC feature
//#if defined(SERIAL_CDC)
//  PluggableUSBD().begin();
//  SerialUSB.begin(115200);
//#endif

  setup();
  for(;;){
    loop();
//If you won't be using serial communication comment next line
//    if(arduino::serialEventRun) arduino::serialEventRun();
  }
  return 0;
}

/**************************************
 * WOB Color Definitions to help you keep multiple straight
 **************************************/
#define WOB_Red            0x10
#define WOB_Grn            0x20
#define WOB_Blk            0x30
#define WOB_Prpl           0x40
#define WOB_Org            0x50
#define WOB_Blu            0x60
#define WOB_Ylw            0x70
#define WOB_Pnk            0x80

#define WOB_COLOR          WOB_Grn //Choose which color to report as
#define WOB_COLOR_UUID_LOC 3        //Location in UUID that represents color: shouldn't need to be changed

Nano33BLEAccelerometerData accelerometerData;
Nano33BLETemperatureData temperatureData;

#ifdef __DEBUG__
int count = 1;  //Number of packets sent
#endif


#ifdef __WDT__

#include "ArduinoNanoBLE33_Watchdog.h"
Nano33Watchdog WDT;
#define WATCHDOG_TIMER_SECONDS 180  //How much time must pass before resetting the micro

#endif


void string_to_byte(String _uuid, byte bytes[]) //voodoo black magic that I took from some other code and had a software engineer help make work better
{
  char uuidChar[_uuid.length()+1]; //initialize character 
  strcpy(uuidChar,_uuid.c_str()); //copy a null terminated char to uuidChar
  int pos=0;
  for (unsigned int i = 0; i<sizeof(uuidChar);i += 2){
    char hexValue[3]; 
    hexValue[0]=uuidChar[i];
    hexValue[1]=uuidChar[i+1];
    int intValue=strtol(hexValue,0, 16);
    bytes[pos]=(char)intValue;
    pos++;
  }
}


void transmit(byte _uuid[16], byte _major[2], byte _minor[2]) //function transmits data as iBeacon
{
  byte data[25] = {0x4C,0x00,0x02,0x15,_uuid[0],_uuid[1],_uuid[2],_uuid[3],_uuid[4],_uuid[5],_uuid[6],_uuid[7],_uuid[8],_uuid[9],_uuid[10],_uuid[11],_uuid[12],_uuid[13],_uuid[14],_uuid[15],_major[0],_major[1],_minor[0],_minor[1],59};
  //create the data array above. This array consists of information for iBeacon, UUID, Major, Minor, fixed tx value
  BLE.setManufacturerData(data,25);
  BLE.advertise();
  delay(BLE_ON_TIME_SECONDS * 1000);
  BLE.stopAdvertise();
}

float angle()
{
  Accelerometer.begin();
  float angle;
    if(Accelerometer.pop(accelerometerData))
    {
      float x = accelerometerData.x; //get x y z data and put them into variables
      float y = accelerometerData.y;
      float z = accelerometerData.z;
      float mag = 1; // magnitude of the full vector should be 1
      float xmag = abs(x); // abosolute value of x because its fucked
      float oh = xmag / mag; // ratio of opposite(the x value) and the hypotonuse (the total vector length)
      float degree = asin(oh); //arcsine returns the angle this makes to the perpendicular 
      angle = degree * 57.2958; // converts to an angle
    }
  return(angle);
}

int angle_to_sg(){
  int ang = angle()*100;
  int sg = -.069725*ang + 1560.3;
  return(sg);
}

/*int temperature()
  IN PROGRESS
{
  int temp;
  if(Temperature.pop(temperatureData))
  {
    float ctemp = temperatureData.temperatureCelsius; 
    temp = ctemp;
  }
  return(temp);
}*/

byte _uuidarray[16]; //empty byte array to store the UUID once we convert it

void setup() {
  //Base ID String, UUID is specific, and UUID includes color used (set color in #define WOB_COLOR)
  String id ="A495BB00C5B14B44B5121370F02D74DE";

  //Disable LEDs
  pinMode(LED_BUILTIN, OUTPUT); //sets the L led to low
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_PWR, LOW); //sets the power LED to low

  //Convert UUID to a byte array
  string_to_byte(id, _uuidarray);
  //Set WOB App color in UUID
  _uuidarray[WOB_COLOR_UUID_LOC] = WOB_COLOR;

  #ifdef __DEBUG__
    Serial.begin(9600);
  #endif

  #ifdef __WDT__
    WDT.setTimerSeconds(WATCHDOG_TIMER_SECONDS);
    WDT.begin();
  #endif
}

void update(int* ang, int* specgrav)
{
 float a = angle() * 100;
 int sg = angle_to_sg();

  *ang = (int)a;
  *specgrav = sg;
}

void loop() 
{ // main stuff
 digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH);
 digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH);
 BLE.begin();
  int majdat, mindat;
  update(&majdat, &mindat);
  byte major[2];
  major[0] = (majdat >>8); //bit shift to convert too two byte 
  major[1] = majdat; // other part of bit shift
  byte minor[2];
  minor[0] = (mindat >>8); //same as above but for the minor variable
  minor[1] = mindat;
  transmit(_uuidarray, major, minor);

#ifdef __DEBUG__
  String MajMess = "Major: ";
  String MinMess = "Minor: ";
  String PacMess = "Packet: ";
  Serial.println(MajMess + majdat);
  Serial.println(MinMess + mindat);
  Serial.println(PacMess + count++);
  Serial.println("");
  Serial.flush();
#endif

#ifdef __WDT__
  WDT.kick();
#endif

digitalWrite(PIN_ENABLE_SENSORS_3V3, LOW);
digitalWrite(PIN_ENABLE_I2C_PULLUP, LOW);\
BLE.end();

delay(DELAY_TIME_SECONDS * 1000);
}
