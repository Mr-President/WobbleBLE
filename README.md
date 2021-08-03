# Wobble on Arduino Nano 33 BLE
## Introduction:
  The wobble is another in a long list of open-source DIY floating hydrometers designed to be left in beer while it ferments. This implementation leverages the capabilities of the Arduino Nano 33BLE communicate specific gravity and temperature readings through low power Bluetooth. The information is communicated via the iBeacon standard where specific gravity is in the primary variable and temperature in the secondary variable. This is identical to existing commercialized products in the market and can be read via their software. Data can also be read manually using any iBeacon scanner.


  There is a commercialized product that offers similar functionality currently on the market, but the design here offers a significant cost savings if you can complete the build. You will need some competency with soldering, 3D printing, and possibly excel linear regression. Please note that the accuracy of the measurement will depend on quality of the build and your commitment to getting a high accuracy linear fit. If you are only interested in the level of fermentation activity, then no calibration is needed.


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
