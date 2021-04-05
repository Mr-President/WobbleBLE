#include <Nano33BLEAccelerometer.h>
#include "Nano33BLETemperature.h"
#include "ArduinoNanoBLE33_Watchdog.h"
#include <math.h>
#include <ArduinoBLE.h>

/**************************************
 * Compilation Adders
 **************************************/
//#define __DEBUG__           //Adds writing to the USB Serial Port
#define __WDT__             //Adds configuring the Watchdog Timer

/**************************************
 * TILT Color Definitions
 **************************************/
#define TILT_RED            0x10
#define TILT_GRN            0x20
#define TILT_BLK            0x30
#define TILT_PUR            0x40
#define TILT_ORNG           0x50
#define TILT_BLU            0x60
#define TILT_YLW            0x70
#define TILT_PNK            0x80

#define TILT_COLOR          TILT_BLK //Choose which color to report as
#define TILT_COLOR_UUID_LOC 3        //Location in UUID that represents color: shouldn't need to be changed

Nano33BLEAccelerometerData accelerometerData;
Nano33BLETemperatureData temperatureData;

#ifdef __DEBUG__
int count = 1;  //Number of packets sent
#endif


#ifdef __WDT__

#include "ArduinoNanoBLE33_Watchdog.h"
Nano33Watchdog WDT;
#define WATCHDOG_TIMER_SECONDS 60  //How much time must pass before resetting the micro

#endif

/**************************************
 * BLE Duty Cycle
 **************************************/
#define BLE_ON_TIME_SECONDS 5  //How long to leave on BLE Advertise
#define DELAY_TIME_SECONDS 10  //How long to wait before re-enabling BLE


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
  float angle;
    if(Accelerometer.pop(accelerometerData))
    {
      float x = accelerometerData.x; //get x y z data and put them into variables
      float y = accelerometerData.y;
      float z = accelerometerData.z;
      float mag = sqrt(sq(x) + sq(y) + sq(z)); // find the magnitude of the overall vector
      float xmag = abs(x); // abosolute value of x because its fucked
      float oh = xmag / mag; // ratio of opposite(the x value) and the hypotonuse (the total vector length)
      float degree = asin(oh); //arcsine returns the angle this makes to the perpendicular 
      angle = degree * 57.2958; // converts to an angle
    }
  return(angle);
}

int angle_to_sg(){
  float ang = angle();
  float sg = -1.52624*ang + 1104;
  int rsg = sg;
  return(rsg);
}
/*int temperature()
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
  //Base ID String, UUID is specific, and UUID includes color used (set color in #define TILT_COLOR)
  String id ="A495BB00C5B14B44B5121370F02D74DE";

  //Disable LEDs
  pinMode(LED_BUILTIN, OUTPUT); //sets the L led to low
  digitalWrite(LED_PWR, LOW); //sets the power LED to low

  //Convert UUID to a byte array
  string_to_byte(id, _uuidarray);
  //Set Tilt App color in UUID
  _uuidarray[TILT_COLOR_UUID_LOC] = TILT_COLOR;

  //Turn on Accelerometer and BLE
  Accelerometer.begin();
  BLE.begin();

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

  delay(DELAY_TIME_SECONDS * 1000);
}
