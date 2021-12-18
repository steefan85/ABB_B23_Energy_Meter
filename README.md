## Library for reading ABB B23 Modbus Energy meters. ##

### SECTIONS: ###
#### [INTRODUCTION](#introduction) ####
#### [CONFIGURING](#configuring) ####
#### [INITIALIZING](#initializing) ####
#### [READING](#reading) ####
#### [PROBLEMS](#problems) ####
#### [CREDITS](#credits) ####

---

### Introduction: ###
This library allows you reading ABB B23 energy meter(s) using:
- [x] Hardware Serial (<i><b>recommended option</b>, smallest number of reads errors, especially for esp8266</i>) <b><i>or</i></b>
- [x] Software Serial, attached as core libraries for esp8266 and avr</br>
     (<i>the new version of esp Software Serial library</br>
      has a different initialization compared to avr!</br>
      <b>This version of ABB_B23 library (>=1.1.0) works only with esp Software Serial 6.0.0 or higher!</b></br>
      If you have an older esp Software Serial version</br>
      use library version 1.0.x</i>)</br>

you also need rs232<->rs485 converter:
- [x] with automatic flow direction control (<i>look at images below</i>) <b><i>or</i></b>
- [x] with additional pins for flow control, like MAX485</br>
     (<i>in this case MAX485 DE and RE pins must be connected together to one of uC pin</br>
     and this pin must be passed when initializing the library</i>)

---

### Configuring: ###
Default configuration is specified in the [ABB_B23.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23.h#L19) file, and parameters are set to:</br>
<i>Software Serial mode, baud 4800, uart config SERIAL_8N1, without DE/RE pin,</br>
uart pins for esp32 hwserial and esp32/esp8266/avr swserial as NOT_A_PIN (-1).</br></br>
For esp32 hwserial this means using the default pins for the selected uart port,</br>
specified in the core library (HardwareSerial.cpp).</br>
For swserial option (esp32/esp8266/avr) is necessary</br>
to specify the pin numbers, as described below.</i>

User can set the parameters in two ways:
- by editing the [ABB_B23_Config_User.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23_Config_User.h) file
- by passing values during initialization (section below)

[ABB_B23_Config_User.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23_Config_User.h) file includes also two parameters that can be adjusted depending on your needs:
- WAITING_TURNAROUND_DELAY (default set to 200ms) defines the time (after sending the query) for the response from the slave device.
  If the slave device does not send the required number of bytes (FRAMESIZE) within this time, an ABB_B23_ERR_TIMEOUT error will be returned.
- RESPONSE_TIMEOUT (default set to 500ms) defines the time (after sending the request and receiving the reply) to a possible response 
  from other slave devices on the bus, during this time it will not be possible to execute another query.
  It is a protection time for devices that are not able to quickly respond to inquiries.

NOTE for Hardware Serial mode: <i>to force the Hardware Serial mode,</br>
user must edit the corresponding entry in [ABB_B23_Config_User.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23_Config_User.h#L17) file.</br>
adding #define USE_HARDWARESERIAL to the main ino file is not enough.</i>

Keep in mind you need to set your ABB_B23 to RTU-mode with the same baudrate as defined here, for example "BAU r4.8k".

---

### Initializing: ###
If the user configuration is specified in the [ABB_B23_Config_User.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23_Config_User.h) file</br>
or if the default configuration from the [ABB_B23.h](https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23.h#L19) file is suitable</br>
initialization is limited to passing serial port reference (software or hardware)</br>
and looks as follows:
```cpp
//lib init when Software Serial is used:
#include <ABB_B23.h>
#include <SoftwareSerial.h>

// for ESP8266 and ESP32
SoftwareSerial swSerABB_B23;
//              _______________________________software serial reference
//             |
ABB_B23 ABB_B23(swSerABB_B23);


// for AVR
SoftwareSerial swSerABB_B23(ABB_B23_RX_PIN, ABB_B23_TX_PIN);
//                               |           |_tx pin definition(from ABB_B23_Config_User.h)
//                               |_____________rx pin definition(from ABB_B23_Config_User.h)
//
//              _______________________________software serial reference
//             |
ABB_B23 ABB_B23(swSerABB_B23);
```

```cpp
//lib init when Hardware Serial is used:
#include <ABB_B23.h>

//            _________________________________hardware serial reference
//           |
ABB_B23 ABB_B23(Serial);
```
If the user wants to temporarily change the configuration during the initialization process</br>
then can pass additional parameters as below:
```cpp
//lib init when Software Serial is used:
#include <ABB_B23.h>
#include <SoftwareSerial.h>

// for ESP8266 and ESP32
SoftwareSerial swSerABB_B23;
//              ________________________________________software serial reference
//             |      __________________________________baudrate(optional, default from ABB_B23_Config_User.h)
//             |     |           _______________________dere pin for max485(optional, default from ABB_B23_Config_User.h)
//             |     |          |              _________software uart config(optional, default from ABB_B23_Config_User.h)
//             |     |          |             |    _____rx pin number(optional, default from ABB_B23_Config_User.h)
//             |     |          |             |   |    _tx pin number(optional, default from ABB_B23_Config_User.h)
//             |     |          |             |   |   | 
ABB_B23 ABB_B23(swSerABB_B23, 9600, NOT_A_PIN, SWSERIAL_8N1, 13, 15);


// for AVR
SoftwareSerial swSerABB_B23(10, 11);
//              ________________________________________software serial reference
//             |      __________________________________baudrate(optional, default from ABB_B23_Config_User.h)   
//             |     |           _______________________dere pin for max485(optional, default from ABB_B23_Config_User.h)
//             |     |          |
ABB_B23 ABB_B23(swSerABB_B23, 9600, NOT_A_PIN);
```

```cpp
//lib init when Hardware Serial is used:
#include <ABB_B23.h>

// for ESP8266
//            ______________________________________hardware serial reference
//           |      ________________________________baudrate(optional, default from ABB_B23_Config_User.h)
//           |     |           _____________________dere pin for max485(optional, default from ABB_B23_Config_User.h)
//           |     |          |            _________hardware uart config(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |       __swap hw serial pins from 3/1 to 13/15(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |      |
ABB_B23 ABB_B23(Serial, 9600, NOT_A_PIN, SERIAL_8N1, false);


// for ESP32
//            ______________________________________hardware serial reference
//           |      ________________________________baudrate(optional, default from ABB_B23_Config_User.h)
//           |     |           _____________________dere pin for max485(optional, default from ABB_B23_Config_User.h)
//           |     |          |            _________hardware uart config(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |    _____rx pin number(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |   |    _tx pin number(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |   |   | 
ABB_B23 ABB_B23(Serial, 9600, NOT_A_PIN, SERIAL_8N1, 13, 15);


// for AVR
//            ______________________________________hardware serial reference
//           |      ________________________________baudrate(optional, default from ABB_B23_Config_User.h)
//           |     |           _____________________dere pin for max485(optional, default from ABB_B23_Config_User.h)
//           |     |          |            _________hardware uart config(optional, default from ABB_B23_Config_User.h)
//           |     |          |           |
//           |     |          |           |
ABB_B23 ABB_B23(Serial, 9600, NOT_A_PIN, SERIAL_8N1);
```
NOTE for ESP8266: <i>when GPIO15 is used (especially for swapped hardware serial):</br>
some converters have built-in pullup resistors on TX/RX lines from rs232 side,</br>
connection this type of converters to ESP8266 pin GPIO15 block booting process.</br>
In this case you can replace the pull-up resistor on converter with higher value (100k),</br>
to ensure low level on GPIO15 by built-in in most ESP8266 modules pulldown resistor.</br></i>

---

### Reading: ###
List of available registers for ABB B23:</br>
https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23.h#L88
```cpp
//reading voltage from ABB_B23 with slave address 0x01 (default)
//                                      __________register name
//                                     |
float voltage = ABB_B23.readVal(ABB_B23_U_SYSTEM);

//reading power from 1st ABB_B23 with slave address ID = 0x01
//reading power from 2nd ABB_B23 with slave address ID = 0x02
//useful with several meters on RS485 line
//                                      __________register name
//                                     |      ____ABB_B23 device ID  
//                                     |     |
float power1 = ABB_B23.readVal(ABB_B23_P_SYSTEM, 0x01);
float power2 = ABB_B23.readVal(ABB_B23_P_SYSTEM, 0x02);
```
NOTE: <i>if you reading multiple ABB_B23 devices on the same RS485 line,</br>
remember to set the same transmission parameters on each device,</br>
only ID must be different for each ABB_B23 device.</i>

---

### Problems: ###
Sometimes <b>readVal</b> return <b>NaN</b> value (not a number),</br>
this means that the requested value could not be read from the ABB_B23 module for various reasons.</br>

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
uint16_t lasterror = ABB_B23.getErrCode(true);

//clear error code also available with:
ABB_B23.clearErrCode();
```
Errors list returned by <b>getErrCode</b>:</br>
https://github.com/steefan85/ABB_B23_Energy_Meter/blob/master/ABB_B23.h#L243</br>

You can also check total number of errors using function:
```cpp
//get total errors counter
//                                       _________optional parameter,
//                                      |         true -> read and reset errors counter
//                                      |         false or no parameter -> read errors counter
//                                      |         but not reset stored counter (for future checking)
uint16_t cnterrors = ABB_B23.getErrCount(true);

//clear errors counter also available with:
ABB_B23.clearErrCount();
```

And finally you can read the counter of correctly made readings:
```cpp
//get total success counter
//                                         ___optional parameter,
//                                        |   true -> read and reset success counter
//                                        |   false or no parameter -> read success counter
//                                        |   but not reset stored counter (for future checking)
uint16_t cntsuccess = ABB_B23.getSuccCount(true);

//clear success counter also available with:
ABB_B23.clearSuccCount();
```

---

### Credits: ###

:+1: ABB_B23_Energy_Meter library by adlerweb (https://github.com/adlerweb/ABB_B23_Energy_Meter)</br>
:+1: SDM_Energy_Meter library by Reaper7 (https://github.com/reaper7/SDM_Energy_Meter)</br>
:+1: ESP SoftwareSerial library by Peter Lerup (https://github.com/plerup/espsoftwareserial)</br>
:+1: crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino)</br>
:+1: influxdb example by AndersV209 (https://github.com/reaper7/SDM_Energy_Meter/pull/45)</br>
