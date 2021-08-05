# Wobble on Arduino Nano 33 BLE
## Introduction:

First and formost let me caviate this all by saying I am not a software engineer so the code contained here is likely rough. The higest quality portion is the watchdog written by Acellama88.

  The wobble is another in a long list of open-source DIY floating hydrometers designed to be left in beer while it ferments. This implementation leverages the capabilities of the Arduino Nano 33BLE communicate specific gravity and temperature readings through low power Bluetooth. The information is communicated via the iBeacon standard where specific gravity is in the primary variable and angle in the secondary variable. This is similar to existing commercialized products in the market and can be read via their software. The exception is that the temperature reading will be in the 1000s and represent an angle. For example 8345 is an angle of 83.45 degrees. Data can also be read manually using any iBeacon scanner.

  There is a commercialized product that offers similar functionality currently on the market, but the design here offers a significant cost savings if you can complete the build. You will need some competency with soldering, 3D printing, and possibly excel linear regression. Please note that the accuracy of the measurement will depend on quality of the build and your commitment to getting a high accuracy linear fit. In its current state this device is more for monitoring activity than getting highly accurate readings. I will continue to make changes and update as I have time, so please pass on suggestions if you have them!


## Part List:


*Arduino Nano 33BLE*: (make sure to get the Arduino WITHOUT headers)
 https://store.arduino.cc/usa/nano-33-ble
 
 
*Container*:
https://www.amazon.com/gp/product/B07F8RQYQ6/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1


*Batteries*: 
https://www.amazon.com/gp/product/B07WTQHK27/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1


*Battery contacts*:
https://www.amazon.com/gp/product/B00GN3PN46/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1


*Copper coated 4.5mm BBs* : https://www.amazon.com/gp/product/B000HKKY7C/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1


*Canning wax*: (I found this at my grocery store for a better price, but this is here as an example)
https://www.amazon.com/203-060-005-Household-Paraffin-Wax-GULFWAX-PARAFFIN/dp/B01FDJP88Q/ref=sr_1_3?dchild=1&keywords=Canning+wax&qid=1628014851&sr=8-3

## 3D Printed Parts:

Both parts can be found on the below thingiverse link. I printed at 5% gyroid infill, too keep density close I would use the same value. 

https://www.thingiverse.com/thing:4925391

## Programming Arduino 33 BLE:

The Arduino can be programmed through the Arduino interface. Download all the files in the “Wobble” folder and place them in the same folder. Open the Wobble.ino file in the Arduino editor and program as you would any other Arduino. If you want to make changes to the refresh rate (default 15 seconds max of 180 seconds before watchdog resets Arduino) then now is the time to do it. There is minimal power savings by increasing the advertise delay so I would recommend leaving it at 15 seconds. You may also change the linear fit here but for the first use you will likely need to use the default code. 

## Modifying and Powering Arduino:

The CR123a batter supplies an average of 3 volts over its lifetime so it can be used to directly power the Arduino nano through the 3.3V and ground pins. Please reference the schematics on the Arduino info page linked below and in the parts list. This will show you exactly where to solder the connections. Red wire should connect to the 3V3 and black to the ground. Then the red wire (or the positive wire) should be soldered to the non-spring portion of the battery contacts. Black (or the negative wire) should then be soldered to the spring-loaded portion of the contacts. This must be done correctly, or you could damage your Arduino. It is also worth noting that you need enough wire to span to connect to the contacts but too much wire will make it difficult to fit in the container. Trim wires to length for best fit. 
To achieve the longest battery life, the 5v trace on the underside of the Arduino needs to be cut. Please see the below post for instructions on how to do this. PLEASE NOTE that once you have cut this trace the Arduino will need external power to program. For this reason, I recommend that you cut this trace once you have completed programming for simplicity’s sake. Also note that supplying power in this way from a battery without a regulator is against the recommendations of Arduino and could shorten the lifetime of the device.
https://support.arduino.cc/hc/en-us/articles/360014735580-How-to-bypass-the-DC-regulator-in-the-NANO-33-BLE-SENSE

## Final Assembly

There are two 3D printed parts for this build. The body is the assembly that holds the battery, battery contacts and connects to the container. This part should be printed with 5% gyroid infill to match the density of my design more closely. The second part is the board holder. Because this part has small standoffs for the board that can be easily damaged, I chose to make it replaceable. Infill choice matters less for this since it will print nearly solid no matter what. 
To assemble the wobble first hot blue the body to the lid of the container tubes. The divot in the print should fit over the bump on the lid and act as an aligner. Make sure to use enough hot glue to hold it in place but not so much it overflows into the threads. Install the board on the board holder by aligning it with the pegs and pressing it into place. Install the battery contacts in the battery holder. Finally glue the board holder to the bot with a small dot of hot glue.
In the tube put in 8.65g of pellets or 25 pellets and shake so that they align. Pour 1.5g ¬of melted canning wax over these pellets to hold them in place. Seriously it takes like no wax so don’t overdo it or you won’t be able to get the body in. Once the wax has hardened you are ready to begin using the wobble with the software of your choice, simply place the battery in the battery holder, verify that the lights flick on then turn off and install the device in the tube tightening all the way to ensure a water (or beer) tight seal. 

## Optional Advanced Calibration:

This section is optional and only relevant if you would like to improve the calibration of the device and get “accurate” readings on specific gravity. First you will need to modify the firmware by uncommenting the report angle line and comment out the report sg line. Once you have done this you are ready to gather data. First fill a pitcher with water. The pitcher should be large enough that the wobble can float free but small enough that adding sugar to the pitcher to change gravity doesn’t waste all your sugar. Once the wobble is floating in the water only let it stabilize for a couple minutes then take a reading of the reported angle. Make sure to note this down. Next add sugar until you can measure an appreciable gravity change (1.020 for example) with a regular hydrometer. Remember you aren’t brewing here so don’t throw out the sample you test with the hydrometer. Once you have this solution add the wobble and let it stabilize for a few minutes. Record the angle. Next you will take these two points and place them in excel to get a linear fit where the angle is the x axis, and the SG is the y axis. This linear fit can then be put in the firmware to replace the stock equation. Make sure to revert the changes to the angle and SG reporting code. You can use more points if you would like. 

## Battery Life Expectations:

I would recommend that you always remove the battery from the device when you are not using it. The Arduino BLE is low power but will still consumer enough power to drain the battery in about 30 days. There is currently not low power notification in the device so once if it stops reporting first check the voltage of the battery. If you choose not to remove the 5V trace on the underside of the board battery life is likely to be under 1 week, so I would highly recommend cutting the trace. 

## Future:

The biggest step forward in this project would be a decrease in power consumption and improve accuracy of reading. Unfortunately, I believe that I have hit the lowest power consumption I can without implementing an external resonator or timer of some kind. I wanted to make this project as simple part wise as possible, so this is out of the question. If you happen to see this and do see a way to reduce power consumption, please feel free to raise an issue or message me directly


As for accuracy in the future I would like to look at an averaging system to take several readings quickly average them and then use that for the angle value. 


At this time information from the device can only be read using either an iBeacon scanner or already existing applications and raspberry pi builds for a similar commercialized product. I intend to start work on a raspberry pi application that will serve a similar purpose and integrate with brewfather software but there is a lot of learning I need to do first. 
