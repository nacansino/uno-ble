HM-1X Bluetooth Arduino Library
==============================

The HM-1X Bluetooth 4.x/5.0 modules with support for SPP and BLE profiles.
The full compatibility table can be seen at the original [manufacturer's website](http://www.jnhuamao.cn/).
This is a fork of Sparkfun's Arduino library that was originally made for HM12/13.
This should be compatible with any HM-1X devices from 10-19, but as of this writing it is only tested for HM-19.

The modules use a UART communication interface.

This library supports communication with the module via either SoftwareSerial, HardwareSerial, or I2C via a Qwiic serial interface.

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

License Information
-------------------

This product is _**open source**_! 

Various bits of the code have different licenses applied. Anything SparkFun wrote is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anything derivative under the same license.
