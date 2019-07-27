## Library for reading Berg BZ40i Modbus Energy meters. ##

### SECTIONS: ###
#### 1. [INTRODUCTION](#introduction) ####
#### 2. [SCREENSHOTS](#screenshots) ####
#### 3. [CONFIGURING](#configuring) ####
#### 4. [INITIALIZING](#initializing) ####
#### 5. [READING](#reading) ####
#### 6. [DEBUGING](#debuging) ####
#### 7. [CREDITS](#credits) ####

---

### Introduction: ###
This library allows you reading Berg BZ40i energy meter(s) using:
- [x] Hardware Serial (<i>recommended option, smallest number of reads errors</i>) <b><i>or</i></b>
- [x] Software Serial (<i>[library for ESP8266](https://github.com/plerup/espsoftwareserial)</i>)

you also need rs232<->rs485 converter:
- [x] with automatic flow direction control (<i>look at images below</i>) <b><i>or</i></b>
- [x] with additional pins for flow control, like MAX485</br>
     (<i>in this case MAX485 DE and RE pins must be connected together to one of uC pin</br>
     and this pin must be passed when initializing the library</i>)

---

### Screenshots: ###


---

### Configuring: ###
Default configuration is specified in the [BZ40i.h](https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i.h#L19) file, and parameters are set to:</br>
<i>Software Serial, baud 4800, uart config SERIAL_8N1, without DE/RE pin</i>.</br>

User can set the parameters in two ways:
- by editing the [BZ40i_Config_User.h](https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i_Config_User.h) file
- by passing values during initialization (section below)

NOTE for Hardware Serial mode: <i>to force the Hardware Serial mode,</br>
user must edit the corresponding entry in [BZ40i_Config_User.h](https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i_Config_User.h#L15) file.</br>
adding #define USE_HARDWARESERIAL to the main ino file is not enough.</i>

Keep in mind you need to set your BZ40i to RTU-mode with the same baudrate as defined here, for example "BAU r4.8k".

---

### Initializing: ###
If the user configuration is specified in the [BZ40i_Config_User.h](https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i_Config_User.h) file</br>
or if the default configuration from the [BZ40i.h](https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i.h#L19) file is suitable</br>
initialization is limited to passing serial port reference (software or hardware)</br>
and looks as follows:
```cpp
//lib init when Software Serial is used:
#include <SoftwareSerial.h>
#include <BZ40i.h>

SoftwareSerial swSerBZ40i(D1, D2);

//              _software serial reference
//             |
BZ40i bz40i(swSerBZ40i);


//lib init when Hardware Serial is used:
#include <BZ40i.h>

//            _hardware serial reference
//           |
BZ40i bz40i(Serial);
```
If the user wants to temporarily change the configuration during the initialization process</br>
then can pass additional parameters as below:
```cpp
//lib init when Software Serial is used:
#include <SoftwareSerial.h>
#include <BZ40i.h>

SoftwareSerial swSerBZ40i(D1, D2);

//              ______________________software serial reference
//             |          ____________baudrate(optional, default from BZ40i_Config_User.h)   
//             |         |           _dere pin for max485(optional, default from BZ40i_Config_User.h)
//             |         |          |
BZ40i bz40i(swSerBZ40i, 9600, NOT_A_PIN);


//lib init when Hardware Serial is used:
#include <BZ40i.h>

//             _____________________________________hardware serial reference
//            |      _______________________________baudrate(optional, default from BZ40i_Config_User.h)
//            |     |           ____________________dere pin for max485(optional, default from BZ40i_Config_User.h)
//            |     |          |            ________hardware uart config(optional, default from BZ40i_Config_User.h)
//            |     |          |           |       _swap hw serial pins from 3/1 to 13/15(optional, default from BZ40i_Config_User.h)
//            |     |          |           |      |
BZ40i bz40i(Serial, 9600, NOT_A_PIN, SERIAL_8N1, false);
```
NOTE for ESP8266: <i>when GPIO15 is used (especially for swapped hardware serial):</br>
some converters have built-in pullup resistors on TX/RX lines from rs232 side,</br>
connection this type of converters to ESP8266 pin GPIO15 block booting process.</br>
In this case you can replace the pull-up resistor on converter with higher value (100k),</br>
to ensure low level on GPIO15 by built-in in most ESP8266 modules pulldown resistor.</br></i>

---

### Reading: ###
List of available registers for Berg BZ40i:</br>
https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i.h#L59
```cpp
//reading voltage from BZ40i with slave address 0x01 (default)
//                                      __________register name
//                                     |
float voltage = bz40i.readVal(BZ40i_U_SYSTEM);

//reading power from 1st BZ40i with slave address ID = 0x01
//reading power from 2nd BZ40i with slave address ID = 0x02
//useful with several meters on RS485 line
//                                      __________register name
//                                     |      ____BZ40i device ID  
//                                     |     |
float power1 = bz40i.readVal(BZ40i_P_SYSTEM, 0x01);
float power2 = bz40i.readVal(BZ40i_P_SYSTEM, 0x02);
```
NOTE: <i>if you reading multiple BZ40i devices on the same RS485 line,</br>
remember to set the same transmission parameters on each device,</br>
only ID must be different for each BZ40i device.</i>

---

### Debuging: ###
Sometimes <b>readVal</b> return <b>NaN</b> value (not a number),</br>
this means that the requested value could not be read from the BZ40i module for various reasons.</br>

__Please check out open and close issues, maybe the cause of your error is explained or solved there.__

The most common problems are:
- weak or poorly filtered power supply / LDO, causing NaN readings and ESP crashes</br>
- faulty or incorrectly prepared converter</br>
- faulty esp module</br>
- many users report that between each readings should be placed <i>delay(50);</i></br>
- using GPIO15 without checking signal level (note above)</br>
- compilation error for hardware serial mode</br>

You can get last error code using function:
```cpp
//get last error code
//                                      __________optional parameter,
//                                     |          true -> read and reset error code
//                                     |          false or no parameter -> read error code
//                                     |          but not reset stored code (for future checking)
//                                     |          will be overwriten when next error occurs
uint16_t lasterror = bz40i.getErrCode(true);

//clear error code also available with:
bz40i.clearErrCode();
```
Errors list returned by <b>getErrCode</b>:</br>
https://github.com/adlerweb/BZ40i_Energy_Meter/blob/master/BZ40i.h#L214</br>

You can also check total number of errors using function:
```cpp
//get total errors counter
//                                       _________optional parameter,
//                                      |         true -> read and reset errors counter
//                                      |         false or no parameter -> read errors counter
//                                      |         but not reset stored counter (for future checking)
uint16_t cnterrors = bz40i.getErrCount(true);

//clear errors counter also available with:
bz40i.clearErrCount();
```

---

### Notes: ###

I wasn't able to get a proper timing with ESP8266 and SoftwareSerial 5.x. If you run into similar problems try downgrading to Version 4.0. Also keep in mind a lower baudrate is much more likely to work.

---

### Credits: ###

:+1: SDM_Energy_Meter library by Reaper7 (https://github.com/reaper7/SDM_Energy_Meter)</br>
:+1: ESP SoftwareSerial library by Peter Lerup (https://github.com/plerup/espsoftwareserial)</br>
:+1: crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino)</br>
