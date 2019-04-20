/* Library for reading Berg BZ40i Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  2019 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on BZ40i_Energy_Meter 2016-2019 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/
//------------------------------------------------------------------------------
#ifndef BZ40i_h
#define BZ40i_h
//------------------------------------------------------------------------------
#include <Arduino.h>
#include <BZ40i_Config_User.h>
#ifdef USE_HARDWARESERIAL
  #include <HardwareSerial.h>
#else
  #include <SoftwareSerial.h>
#endif
//------------------------------------------------------------------------------
//DEFAULT CONFIG (DO NOT CHANGE ANYTHING!!! for changes use BZ40i_Config_User.h):
//------------------------------------------------------------------------------
#ifndef BZ40i_UART_BAUD
  #define BZ40i_UART_BAUD                     4800                                //default baudrate
#endif

#ifndef DERE_PIN
  #define DERE_PIN                          NOT_A_PIN                           //default digital pin for control MAX485 DE/RE lines (connect DE & /RE together to this pin)
#endif

#ifdef USE_HARDWARESERIAL

  #ifndef BZ40i_UART_CONFIG
    #define BZ40i_UART_CONFIG                 SERIAL_8N1                          //default hardware uart config
  #endif

  #ifndef SWAPHWSERIAL
    #define SWAPHWSERIAL                    0                                   //(only esp8266) when hwserial used, then swap uart pins from 3/1 to 13/15 (default not swap)
  #endif

#endif

#ifndef MAX_MILLIS_TO_WAIT
  #define MAX_MILLIS_TO_WAIT                500                                 //default max time to wait for response from BZ40i
#endif
//------------------------------------------------------------------------------
#define FRAMESIZE                           9                                   //size of out/in array
#define BZ40i_REPLY_BYTE_COUNT                0x04                                //number of bytes with data

#define BZ40i_B_01                            0x01                                //BYTE 1 -> slave address (default value 1 read from node 1)
#define BZ40i_B_02                            0x04                                //BYTE 2 -> function code (default value 0x04 read from 3X input registers)
                                                                                //BYTES 3 & 4 (BELOW)
//BZ40i 120 registers
#define BZ40i120C_VOLTAGE                     0x0000                              //V
#define BZ40i120C_CURRENT                     0x0006                              //A
#define BZ40i120C_POWER                       0x000C                              //W
#define BZ40i120C_ACTIVE_APPARENT_POWER       0x0012                              //VA
#define BZ40i120C_REACTIVE_APPARENT_POWER     0x0018                              //VAR
#define BZ40i120C_POWER_FACTOR                0x001E                              //
#define BZ40i120C_FREQUENCY                   0x0046                              //Hz
#define BZ40i120C_IMPORT_ACTIVE_ENERGY        0x0048                              //Wh
#define BZ40i120C_EXPORT_ACTIVE_ENERGY        0x004A                              //Wh
#define BZ40i120C_TOTAL_ACTIVE_ENERGY         0x0156                              //Wh
//BZ40i 220 registers
#define BZ40i220T_VOLTAGE                     0x0000                              //V
#define BZ40i220T_CURRENT                     0x0006                              //A
#define BZ40i220T_POWER                       0x000C                              //W
#define BZ40i220T_ACTIVE_APPARENT_POWER       0x0012                              //VA
#define BZ40i220T_REACTIVE_APPARENT_POWER     0x0018                              //VAR
#define BZ40i220T_POWER_FACTOR                0x001E                              //
#define BZ40i220T_PHASE_ANGLE                 0x0024                              //DEGREE
#define BZ40i220T_FREQUENCY                   0x0046                              //Hz
#define BZ40i220T_IMPORT_ACTIVE_ENERGY        0x0048                              //Wh
#define BZ40i220T_EXPORT_ACTIVE_ENERGY        0x004A                              //Wh
#define BZ40i220T_IMPORT_REACTIVE_ENERGY      0x004C                              //VARh
#define BZ40i220T_EXPORT_REACTIVE_ENERGY      0x004E                              //VARh
#define BZ40i220T_TOTAL_ACTIVE_ENERGY         0x0156                              //Wh
#define BZ40i220T_TOTAL_REACTIVE_ENERGY       0x0158                              //VARh
//BZ40i 230 registers
#define BZ40i230_VOLTAGE                      0x0000                              //V
#define BZ40i230_CURRENT                      0x0006                              //A
#define BZ40i230_POWER                        0x000C                              //W
#define BZ40i230_ACTIVE_APPARENT_POWER        0x0012                              //VA
#define BZ40i230_REACTIVE_APPARENT_POWER      0x0018                              //VAR
#define BZ40i230_POWER_FACTOR                 0x001E                              //
#define BZ40i230_PHASE_ANGLE                  0x0024                              //DEGREE
#define BZ40i230_FREQUENCY                    0x0046                              //Hz
#define BZ40i230_IMPORT_ACTIVE_ENERGY         0x0048                              //Wh
#define BZ40i230_EXPORT_ACTIVE_ENERGY         0x004A                              //Wh
#define BZ40i230_IMPORT_REACTIVE_ENERGY       0x004C                              //VARh
#define BZ40i230_EXPORT_REACTIVE_ENERGY       0x004E                              //VARh
#define BZ40i230_TOTAL_SYSTEM_POWER_DEMAND    0x0054                              //W
#define BZ40i230_MAXIMUM_SYSTEM_POWER_DEMAND  0x0056                              //W
#define BZ40i230_CURRENT_POSITIVE_POWER_DEMAND 0x0058                             //W
#define BZ40i230_MAXIMUM_POSITIVE_POWER_DEMAND 0x005A                             //W
#define BZ40i230_CURRENT_REVERSE_POWER_DEMAND 0x005C                              //W
#define BZ40i230_MAXIMUM_REVERSE_POWER_DEMAND 0x005E                              //W
#define BZ40i230_CURRENT_DEMAND               0x0102                              //Amps
#define BZ40i230_MAXIMUM_CURRENT_DEMAND       0x0108                              //Amps
#define BZ40i230_TOTAL_ACTIVE_ENERGY          0x0156                              //kwh
#define BZ40i230_TOTAL_REACTIVE_ENERGY        0x0158                              //kvarh
#define BZ40i230_CURRENT_RESETTABLE_TOTAL_ACTIVE_ENERGY   0x0180                  //Wh
#define BZ40i230_CURRENT_RESETTABLE_TOTAL_REACTIVE_ENERGY 0x0182                  //VARh
//BZ40i 630 registers
#define BZ40i630_VOLTAGE1                     0x0000                              //V
#define BZ40i630_VOLTAGE2                     0x0002                              //V
#define BZ40i630_VOLTAGE3                     0x0004                              //V
#define BZ40i630_CURRENT1                     0x0006                              //A
#define BZ40i630_CURRENT2                     0x0008                              //A
#define BZ40i630_CURRENT3                     0x000A                              //A
#define BZ40i630_CURRENTSUM                   0x0030                              //A
#define BZ40i630_POWER1                       0x000C                              //W
#define BZ40i630_POWER2                       0x000E                              //W
#define BZ40i630_POWER3                       0x0010                              //W
#define BZ40i630_POWERTOTAL                   0x0034                              //W
#define BZ40i630_VOLT_AMPS1                   0x0012                              //VA
#define BZ40i630_VOLT_AMPS2                   0x0014                              //VA
#define BZ40i630_VOLT_AMPS3                   0x0016                              //VA
#define BZ40i630_VOLT_AMPS_TOTAL              0x0038                              //VA
#define BZ40i630_VOLT_AMPS_REACTIVE1          0x0018                              //VAr
#define BZ40i630_VOLT_AMPS_REACTIVE2          0x001A                              //VAr
#define BZ40i630_VOLT_AMPS_REACTIVE3          0x001C                              //VAr
#define BZ40i630_VOLT_AMPS_REACTIVE_TOTAL     0x003C                              //VAr
#define BZ40i630_POWER_FACTOR1                0x001E
#define BZ40i630_POWER_FACTOR2                0x0020
#define BZ40i630_POWER_FACTOR3                0x0022
#define BZ40i630_POWER_FACTOR_TOTAL           0x003E
#define BZ40i630_PHASE_ANGLE1                 0x0024                              //Degrees
#define BZ40i630_PHASE_ANGLE2                 0x0026                              //Degrees
#define BZ40i630_PHASE_ANGLE3                 0x0028                              //Degrees
#define BZ40i630_PHASE_ANGLE_TOTAL            0x0042                              //Degrees
#define BZ40i630_VOLTAGE_AVERAGE              0x002A                              //V
#define BZ40i630_CURRENT_AVERAGE              0x002E                              //A
#define BZ40i630_FREQUENCY                    0x0046                              //HZ
#define BZ40i630_IMPORT_ACTIVE_ENERGY         0x0048                              //Wh
#define BZ40i630_EXPORT_ACTIVE_ENERGY         0x004A                              //Wh
#define BZ40i630_IMPORT_REACTIVE_ENERGY       0x004C                              //VARh
#define BZ40i630_EXPORT_REACTIVE_ENERGY       0x004E                              //VARh
#define BZ40i630_TOTAL_SYSTEM_POWER_DEMAND    0x0054                              //W
#define BZ40i630_MAXIMUM_TOTAL_SYSTEM_POWER   0x0056                              //W
#define BZ40i630_PHASE_1_LN_VOLTS_THD         0x00EA                              //%
#define BZ40i630_PHASE_2_LN_VOLTS_THD         0x00EC                              //%
#define BZ40i630_PHASE_3_LN_VOLTS_THD         0x00EE                              //%
#define BZ40i630_AVERAGE_VOLTS_THD            0x00F8                              //%
#define BZ40i630_PHASE_1_CURRENT_THD          0x00F0                              //%
#define BZ40i630_PHASE_2_CURRENT_THD          0x00F2                              //%
#define BZ40i630_PHASE_3_CURRENT_THD          0x00F4                              //%
#define BZ40i630_AVERAGE_CURRENT_THD          0x00FA                              //%
#define BZ40i630_IMPORT1                      0x015a                              //kWh
#define BZ40i630_IMPORT2                      0x015c                              //kWh
#define BZ40i630_IMPORT3                      0x015e                              //kWh
#define BZ40i630_EXPORT1                      0x0160                              //kWh
#define BZ40i630_EXPORT2                      0x0162                              //kWh
#define BZ40i630_EXPORT3                      0x0164                              //kWh
#define BZ40i630_TOTAL_ENERGY1                0x0166                              //kWh
#define BZ40i630_TOTAL_ENERGY2                0x0168                              //kWh
#define BZ40i630_TOTAL_ENERGY3                0x016a                              //kWh

#define BZ40i_B_05                            0x00                                //BYTE 5
#define BZ40i_B_06                            0x02                                //BYTE 6
//------------------------------------------------------------------------------
#define BZ40i_ERR_NO_ERROR                    0                                   //no error
#define BZ40i_ERR_CRC_ERROR                   1                                   //crc error
#define BZ40i_ERR_WRONG_BYTES                 2                                   //bytes b0,b1 or b2 wrong
#define BZ40i_ERR_NOT_ENOUGHT_BYTES           3                                   //not enough bytes from sdm
#define BZ40i_ERR_TIMEOUT                     4                                   //timeout
//------------------------------------------------------------------------------
class BZ40i {
  public:
#ifdef USE_HARDWARESERIAL
    BZ40i(HardwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN, int config = BZ40i_UART_CONFIG, bool swapuart = SWAPHWSERIAL);
#else
    BZ40i(SoftwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN);
#endif
    virtual ~BZ40i();

    void begin(void);
    float readVal(uint16_t reg, uint8_t node = BZ40i_B_01);                       //read value from register = reg and from deviceId = node
    uint16_t getErrCode(bool _clear = false);                                   //return last errorcode (optional clear this value, default flase)
    uint16_t getErrCount(bool _clear = false);                                  //return total errors count (optional clear this value, default flase)
    void clearErrCode();                                                        //clear last errorcode
    void clearErrCount();                                                       //clear total errors count

  private:
#ifdef USE_HARDWARESERIAL
    HardwareSerial& sdmSer;
#else
    SoftwareSerial& sdmSer;
#endif

#ifdef USE_HARDWARESERIAL
    int _config = BZ40i_UART_CONFIG;
    bool _swapuart = SWAPHWSERIAL;
#endif
    long _baud = BZ40i_UART_BAUD;
    int _dere_pin = DERE_PIN;
    uint16_t readingerrcode = BZ40i_ERR_NO_ERROR;                                 //4 = timeout; 3 = not enough bytes; 2 = number of bytes OK but bytes b0,b1 or b2 wrong, 1 = crc error
    uint16_t readingerrcount = 0;                                               //total errors couter 
    uint16_t calculateCRC(uint8_t *array, uint8_t num);
};
#endif //BZ40i_h
