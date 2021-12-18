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
#define ABB_U_LN_P1                         0x5B00
#define ABB_U_LN_P2                         0x5B02
#define ABB_U_LN_P3                         0x5B04
#define ABB_U_LL_P12                        0x5B06
#define ABB_U_LL_P23                        0x5B08
#define ABB_U_LL_P31                        0x5B0A

//Current
#define ABB_I_P1                            0x5B0C
#define ABB_I_P2                            0x5B0E
#define ABB_I_P3                            0x5B10
#define ABB_I_N                             0x5B12

//Power Factor
#define ABB_PF_SYSTEM                       0x5B3A
#define ABB_PF_P1                           0x5B3B
#define ABB_PF_P2                           0x5B3C
#define ABB_PF_P3                           0x5B3D

//Apparent Power
#define ABB_S_SYSTEM                        0x5B24
#define ABB_S_P1                            0x5B26
#define ABB_S_P2                            0x5B28
#define ABB_S_P3                            0x5B2A

//Active Power
#define ABB_P_SYSTEM                        0x5B14
#define ABB_P_P1                            0x5B16
#define ABB_P_P2                            0x5B18
#define ABB_P_P3                            0x5B1A

//Reactive Power
#define ABB_Q_SYSTEM                        0x5B1C
#define ABB_Q_P1                            0x5B1E
#define ABB_Q_P2                            0x5B20
#define ABB_Q_P3                            0x5B22

//Frequency
#define ABB_F                                 0x5B2C

//Counter Import
#define ABB_IMPORT_P                          0x5000

//Counter Export
#define ABB_EXPORT_P                        0x5004

//Counter Net
#define ABB_ACTIVE_NET                        0x5008  //Signed

//Peak Voltage
#define BZ40i_U_PK_LN_P1                      0x00D0
#define BZ40i_U_PK_LN_P2                      0x00D4
#define BZ40i_U_PK_LN_P3                      0x00D8
#define BZ40i_U_PK_LL_P12                     0x00DC
#define BZ40i_U_PK_LL_P23                     0x00E0
#define BZ40i_U_PK_LL_P31                     0x00E4

//Words to read
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
