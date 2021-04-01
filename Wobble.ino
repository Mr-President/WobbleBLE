#include <Nano33BLEAccelerometer.h>
#include "Nano33BLETemperature.h"
#include <math.h>
#include <ArduinoBLE.h>
Nano33BLEAccelerometerData accelerometerData;
Nano33BLETemperatureData temperatureData;

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
  if(BLE.begin()){ //if we are succesful it should carry out the following tasks
    BLE.setManufacturerData(data, 25); //set the data listed above as mfg data
    BLE.advertise(); //turn on advertise
    delay(1 * 1000); 
    BLE.stopAdvertise(); //stop advertise
    BLE.end(); // end BLE services
  }
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
  //uncomment the UUID below that you would like to use. The associated colors are at the end
  //String id ="test"; //testing only will not read on app
  //String id ="A495BB10C5B14B44B5121370F02D74DE"; //Red
  //String id ="A495BB20C5B14B44B5121370F02D74DE"; //Green
  String id ="A495BB30C5B14B44B5121370F02D74DE"; //Black
  //String id ="a495bb40c5b14b44b5121370f02d74de"; //Purple
  //String id ="a495bb50c5b14b44b5121370f02d74de"; //Orange
  //String id ="a495bb60c5b14b44b5121370f02d74de"; //Blue
  //String id ="a495bb70c5b14b44b5121370f02d74de"; //Yellow
  //String id ="a495bb80c5b14b44b5121370f02d74de"; //Pink
  pinMode(LED_BUILTIN, OUTPUT); //sets the L led to low
  digitalWrite(LED_PWR, LOW); //sets the power LED to low
  string_to_byte(id, _uuidarray); // convert the selected UUID to a array of Bytes
}
void loop() { // main stuff
  int majdat = angle() * 100;
  int mindat = angle_to_sg();
  byte major[2];
  major[0] = (majdat >>8); //bit shift to convert too two byte 
  major[1] = majdat; // other part of bit shift
  byte minor[2];
  minor[0] = (mindat >>8); //same as above but for the minor variable
  minor[1] = mindat;
  transmit(_uuidarray, major, minor);
  delay(1 * 1000);
}
