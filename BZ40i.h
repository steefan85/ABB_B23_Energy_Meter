/* Library for reading Berg BZ40i Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  2019-2021 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on SDM_Energy_Meter 2016-2020 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/
//------------------------------------------------------------------------------
#ifndef BZ40i_h
#define BZ40i_h
//------------------------------------------------------------------------------
#include <Arduino.h>
#include <BZ40i_Config_User.h>
#if defined ( USE_HARDWARESERIAL )
  #include <HardwareSerial.h>
#else
  #include <SoftwareSerial.h>
#endif
//------------------------------------------------------------------------------
//DEFAULT CONFIG (DO NOT CHANGE ANYTHING!!! for changes use BZ40i_Config_User.h):
//------------------------------------------------------------------------------
#if !defined (BZ40i_UART_BAUD)
  #define BZ40i_UART_BAUD                     4800                              // default baudrate
#endif

#if !defined ( DERE_PIN )
  #define DERE_PIN                            NOT_A_PIN                         // default digital pin for control MAX485 DE/RE lines (connect DE & /RE together to this pin)
#endif

#ifdef USE_HARDWARESERIAL

  #if !defined ( BZ40i_UART_CONFIG )
    #define BZ40i_UART_CONFIG                 SERIAL_8N1                        // default hardware uart config
  #endif

  #if defined ( ESP8266 ) && !defined ( SWAPHWSERIAL )
    #define SWAPHWSERIAL                              0                         // (only esp8266) when hwserial used, then swap uart pins from 3/1 to 13/15 (default not swap)
  #endif

  #if defined ( ESP32 )
    #if !defined ( BZ40i_RX_PIN )
      #define BZ40i_RX_PIN                            -1                        // use default rx pin for selected port
    #endif
    #if !defined ( BZ40i_TX_PIN )
      #define BZ40i_TX_PIN                            -1                        // use default tx pin for selected port
    #endif
  #endif

#else

  #if defined ( ESP8266 ) || defined ( ESP32 )
    #if !defined ( BZ40i_UART_CONFIG )
      #define BZ40i_UART_CONFIG                       SWSERIAL_8N1              // default softwareware uart config for esp8266/esp32
    #endif
  #endif

  #if !defined ( BZ40i_RX_PIN )
    #define BZ40i_RX_PIN                              -1
  #endif
  #if !defined ( BZ40i_TX_PIN )
    #define BZ40i_TX_PIN                              -1
  #endif

#endif

#if !defined ( WAITING_TURNAROUND_DELAY )
  #define WAITING_TURNAROUND_DELAY                    500                       // time in ms to wait for process current request
#endif

#if !defined ( RESPONSE_TIMEOUT )
  #define RESPONSE_TIMEOUT                            500                       //  time in ms to wait for return response from all devices before next request
#endif

//------------------------------------------------------------------------------
#define FRAMESIZE_OUT                            9                                //size of out array
#define FRAMESIZE_IN                            13                                //size of in array
#define BZ40i_REPLY_BYTE_COUNT                0x08                                //number of bytes with data

#if FRAMESIZE_OUT > FRAMESIZE_IN
    #define FRAMESIZE_MAX FRAMESIZE_OUT
#else
    #define FRAMESIZE_MAX FRAMESIZE_IN
#endif

#define BZ40i_B_01                            0x01                                //BYTE 1 -> slave address (default value 1 read from node 1)
#define BZ40i_B_02                            0x03                                //BYTE 2 -> function code (default value 0x03 read from registers)
                                                                                  //BYTES 3 & 4 (BELOW)

//BZ40i registers

//Voltages
#define BZ40i_U_SYSTEM                        0x5B00
#define ABB_U_LN_P1                           0x5B00
#define BZ40i_U_LN_P2                         0x0008
#define BZ40i_U_LN_P3                         0x000C
#define BZ40i_U_LL_P12                        0x0010
#define BZ40i_U_LL_P23                        0x0014
#define BZ40i_U_LL_P31                        0x0018

//Current
#define BZ40i_I_SYSTEM                        0x001C
#define BZ40i_I_P1                            0x0020
#define BZ40i_I_P2                            0x0024
#define BZ40i_I_P3                            0x0028
#define BZ40i_I_N                             0x00C0

//Power Factor
#define BZ40i_PF_SYSTEM                       0x002C
#define BZ40i_PF_P1                           0x0030
#define BZ40i_PF_P2                           0x0034
#define BZ40i_PF_P3                           0x0038

//Apparent Power
#define BZ40i_S_SYSTEM                        0x004C
#define BZ40i_S_P1                            0x0050
#define BZ40i_S_P2                            0x0054
#define BZ40i_S_P3                            0x0058

//Active Power
#define BZ40i_P_SYSTEM                        0x005C
#define BZ40i_P_P1                            0x0060
#define BZ40i_P_P2                            0x0064
#define BZ40i_P_P3                            0x0068

//Reactive Power
#define BZ40i_Q_SYSTEM                        0x006C
#define BZ40i_Q_P1                            0x0070
#define BZ40i_Q_P2                            0x0074
#define BZ40i_Q_P3                            0x0078

//Frequency
#define ABB_F                                 0x5B2C

//Counter Import
#define ABB_IMPORT_P                          0x5000
#define BZ40i_IMPORT_Q_LAG                    0x0080
#define BZ40i_IMPORT_Q_LEAD                   0x00B0
#define BZ40i_IMPORT_S                        0x00B8

//Counter Export
#define BZ40i_EXPORT_P                        0x0084
#define BZ40i_EXPORT_Q_LAG                    0x0088
#define BZ40i_EXPORT_Q_LEAD                   0x00B4
#define BZ40i_EXPORT_S                        0x00BC

//Peak Voltage
#define BZ40i_U_PK_LN_P1                      0x00D0
#define BZ40i_U_PK_LN_P2                      0x00D4
#define BZ40i_U_PK_LN_P3                      0x00D8
#define BZ40i_U_PK_LL_P12                     0x00DC
#define BZ40i_U_PK_LL_P23                     0x00E0
#define BZ40i_U_PK_LL_P31                     0x00E4

//Peak Current
#define BZ40i_I_PK_P1                         0x00E8
#define BZ40i_I_PK_P2                         0x00EC
#define BZ40i_I_PK_P3                         0x00F0
#define BZ40i_I_PK_N                          0x0100
#define BZ40i_I_PK_D_P1                       0x00F4
#define BZ40i_I_PK_D_P2                       0x00F8
#define BZ40i_I_PK_D_P3                       0x00FC

//Peak Power
#define BZ40i_P_PK                            0x0104
#define BZ40i_S_PK                            0x0108

//Demand
#define BZ40i_I_D_P1                          0x010C
#define BZ40i_I_D_P2                          0x0110
#define BZ40i_I_D_P3                          0x0114
#define BZ40i_I_D_SYSTEM                      0x0120
#define BZ40i_P_D                             0x0118
#define BZ40i_S_D                             0x011C

//Other - these use different word sizes and will not work yet
#define BZ40i_PHASE_ORDER                     0x00C8 // 4 words
#define BZ40i_SERIAL                          0xE000 // 5 words
#define BZ40i_FIRMWARE                        0xE005 // 7 words
#define BZ40i_MODEL                           0xE00C // 1 word
#define BZ40i_OUTPUTS                         0xE00F // 1 word

#define BZ40i_BUS_NODEID                      0xE020 // 1 word
#define BZ40i_BUS_BAUD                        0xE028 // 1 word
#define BZ40i_BUS_PARITY                      0xE029 // 1 word
#define BZ40i_BUS_BITS                        0xE02A // 1 word
#define BZ40i_CT_RATIO                        0xE031 // 1 word
#define BZ40i_PT_RATIO                        0xE034 // 1 word
#define BZ40i_PHASEMODE                       0xE038 // 1 word
#define BZ40i_D_TIME                          0xE03C // 1 word, minutes 1,3,5,10,15,30,60
#define BZ40i_RESET_COUNT                     0xE0A0 // 1 word
#define BZ40i_RESET_VAL                       0xE0A1 // 1 word

#define BZ40i_O1_CFG                          0xE100 // 5 words
#define BZ40i_O1_COEFF_DP                     0xE105 // 1 word
#define BZ40i_O1_COEFF_MP                     0xE106 // 1 word

#define BZ40i_O2_CFG                          0xE108 // 5 words
#define BZ40i_O2_COEFF_DP                     0xE10D // 1 word
#define BZ40i_O2_COEFF_MP                     0xE10E // 1 word

// BZ40i Variable Mapping

#define BZ40i_BAUD_2400                       0x03
#define BZ40i_BAUD_4800                       0x04
#define BZ40i_BAUD_9600                       0x05
#define BZ40i_BAUD_19200                      0x06
#define BZ40i_BAUD_38400                      0x07
#define BZ40i_BAUD_57600                      0x08

#define BZ40i_PARITY_RTU                      0x00 // None
#define BZ40i_PARITY_ASCII                    0x01 // Even

#define BZ40i_DATA_ASCII                      0x00 // 7bit
#define BZ40i_DATA_RTU                        0x01 // 8bit
#define BZ40i_DATA_SW                         0x02

#define BZ40i_PHASEMODE_1W                    0x01 // single phase
#define BZ40i_PHASEMODE_3W                    0x03 // three phase
#define BZ40i_PHASEMODE_4W                    0x04 // three phase + neutral

#define BZ40i_RESET_COUNT_NONE                0x00
#define BZ40i_RESET_COUNT_ECOUNT              0x01

#define BZ40i_RESET_VAL_NONE                  0x00
#define BZ40i_RESET_VAL_PEAK                  0x01
#define BZ40i_RESET_VAL_DMDPEAK               0x02
#define BZ40i_RESET_VAL_DMD                   0x03
#define BZ40i_RESET_VAL_ALL                   0x04

#define BZ40i_O_CFG_IN_P                      0x30
#define BZ40i_O_CFG_IN_S                      0x1C
#define BZ40i_O_CFG_IN_Q_LAG                  0x31
#define BZ40i_O_CFG_IN_Q_LEAD                 0x33
#define BZ40i_O_CFG_OUT_P                     0x32
#define BZ40i_O_CFG_OUT_S                     0x1D
#define BZ40i_O_CFG_OUT_Q_LAG                 0x2E
#define BZ40i_O_CFG_OUT_Q_LEAD                0x2F
#define BZ40i_O_CFG_NONE                      0xFE


#define BZ40i_B_05                            0x00                                //BYTE 5
#define BZ40i_B_06                            0x04                                //BYTE 6 - Read 4 words
//------------------------------------------------------------------------------
#define BZ40i_ERR_NO_ERROR                    0                                   //no error
#define BZ40i_ERR_CRC_ERROR                   1                                   //crc error
#define BZ40i_ERR_WRONG_BYTES                 2                                   //bytes b0,b1 or b2 wrong
#define BZ40i_ERR_NOT_ENOUGHT_BYTES           3                                   //not enough bytes from BZ40i
#define BZ40i_ERR_TIMEOUT                     4                                   //timeout
//------------------------------------------------------------------------------
class BZ40i {
  public:
#if defined ( USE_HARDWARESERIAL )                                              //  hardware serial
  #if defined ( ESP8266 )                                                       //  on esp8266
    BZ40i(HardwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN, int config = BZ40i_UART_CONFIG, bool swapuart = SWAPHWSERIAL);
  #elif defined ( ESP32 )                                                       //  on esp32
    BZ40i(HardwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN, int config = BZ40i_UART_CONFIG, int8_t rx_pin = BZ40i_RX_PIN, int8_t tx_pin = BZ40i_TX_PIN);
  #else                                                                         //  on avr
    BZ40i(HardwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN, int config = BZ40i_UART_CONFIG);
  #endif
#else                                                                           //  software serial
  #if defined ( ESP8266 ) || defined ( ESP32 )                                  //  on esp8266/esp32
    BZ40i(SoftwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN, int config = BZ40i_UART_CONFIG, int8_t rx_pin = BZ40i_RX_PIN, int8_t tx_pin = BZ40i_TX_PIN);
  #else                                         
    BZ40i(SoftwareSerial& serial, long baud = BZ40i_UART_BAUD, int dere_pin = DERE_PIN);
  #endif
#endif
    virtual ~BZ40i();

    void begin(void);
    float readVal(uint16_t reg, byte type = 0, uint8_t node = BZ40i_B_01);      //read value from register = reg and from deviceId = node
                                                                                //type 0=unsigned, 1=signed, 2=raw
    uint16_t getErrCode(bool _clear = false);                                   //return last errorcode (optional clear this value, default flase)
    uint32_t getErrCount(bool _clear = false);                                  //return total errors count (optional clear this value, default flase)
    uint32_t getSuccCount(bool _clear = false);                                 // return total success count (optional clear this value, default false)
    void clearErrCode();                                                        //clear last errorcode
    void clearErrCount();                                                       //clear total errors count
    void clearSuccCount();                                                      // clear total success count

  private:
#if defined ( USE_HARDWARESERIAL )
    HardwareSerial& bz40iSer;
#else
    SoftwareSerial& bz40iSer;
#endif

#if defined ( USE_HARDWARESERIAL )
    int _config = BZ40i_UART_CONFIG;
    #if defined ( ESP8266 )
      bool _swapuart = SWAPHWSERIAL;
    #elif defined ( ESP32 )
      int8_t _rx_pin = -1;
      int8_t _tx_pin = -1;
    #endif
#else
  #if defined ( ESP8266 ) || defined ( ESP32 )
    int _config = BZ40i_UART_CONFIG;
  #endif
  int8_t _rx_pin = -1;
  int8_t _tx_pin = -1; 
#endif
    long _baud = BZ40i_UART_BAUD;
    int _dere_pin = DERE_PIN;
    uint16_t readingerrcode = BZ40i_ERR_NO_ERROR;                               //4 = timeout; 3 = not enough bytes; 2 = number of bytes OK but bytes b0,b1 or b2 wrong, 1 = crc error
    uint32_t readingerrcount = 0;                                               //total errors couter
    uint32_t readingsuccesscount = 0;                                           // total success counter
    uint16_t calculateCRC(uint8_t *array, uint8_t num);
    void flush(unsigned long _flushtime = 0);                                   //  read serial if any old data is available or for a given time in ms
    void dereSet(bool _state = LOW);                                            //  for control MAX485 DE/RE pins, LOW receive from BZ40i, HIGH transmit to BZ40i
};
#endif // BZ40i_h
