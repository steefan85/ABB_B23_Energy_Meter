/* Library for reading ABB B23 Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  steefan85, 2021
*  Based on ABB_B23_Energy_Meter 2019-2021 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on SDM_Energy_Meter 2016-2020 Reaper7
*  crc calculation by Jaime Garc�a (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/

/*
*  USER CONFIG:
*/

//------------------------------------------------------------------------------

/*
*  define or undefine USE_HARDWARESERIAL (uncomment only one or none)
*/
//#undef USE_HARDWARESERIAL
//#define USE_HARDWARESERIAL

//------------------------------------------------------------------------------

/*
*  define user baudrate
*/
#define ABB_B23_UART_BAUD                       9600

//------------------------------------------------------------------------------

/*
*  define user ABB_B23_RX_PIN and ABB_B23_TX_PIN for esp/avr Software Serial option
*  or ESP32 with Hardware Serial if default core pins are not suitable
*/
#if defined ( USE_HARDWARESERIAL )
  #if defined ( ESP32 )
    #define ABB_B23_RX_PIN                        13
    #define ABB_B23_TX_PIN                        15
  #endif
#else
  #if defined ( ESP8266 ) || defined ( ESP32 )
    #define ABB_B23_RX_PIN                        4
    #define ABB_B23_TX_PIN                        5
  #else
    #define ABB_B23_RX_PIN                        4
    #define ABB_B23_TX_PIN                        5
  #endif
#endif


//------------------------------------------------------------------------------

/*
*  define user DERE_PIN for control MAX485 DE/RE lines (connect DE & /RE together to this pin)
*/
#define DERE_PIN                            NOT_A_PIN

//------------------------------------------------------------------------------

#if defined ( USE_HARDWARESERIAL )

  /*
  *  define user ABB_B23_UART_CONFIG for hardware serial
  */
  #define ABB_B23_UART_CONFIG                   SERIAL_8N1

  //----------------------------------------------------------------------------

  /*
  *  define user SWAPHWSERIAL, if true(1) then swap uart pins from 3/1 to 13/15 (only ESP8266)
  */
  //#define SWAPHWSERIAL                      0

#else

  /*
  *  define user ABB_B23_UART_CONFIG for software serial
  */
  #define ABB_B23_UART_CONFIG                   SWSERIAL_8N1
#endif

//------------------------------------------------------------------------------

/*
*  define user WAITING_TURNAROUND_DELAY to wait for response from ABB_B23
*/
#define WAITING_TURNAROUND_DELAY              1000

//------------------------------------------------------------------------------

/*
*  define user RESPONSE_TIMEOUT time in ms to wait for return response from all devices before next request
*/
//#define RESPONSE_TIMEOUT                    500

//------------------------------------------------------------------------------
