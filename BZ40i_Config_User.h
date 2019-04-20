/* Library for reading Berg BZ40i Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  2019 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on BZ40i_Energy_Meter 2016-2019 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
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
//#define BZ40i_UART_BAUD                       9600                                

//------------------------------------------------------------------------------

/*
*  define user DERE_PIN for control MAX485 DE/RE lines (connect DE & /RE together to this pin)
*/
//#define DERE_PIN                            NOT_A_PIN                           

//------------------------------------------------------------------------------

#ifdef USE_HARDWARESERIAL

  /*
  *  define user BZ40i_UART_CONFIG
  */
  //#define BZ40i_UART_CONFIG                   SERIAL_8N1                          

  //----------------------------------------------------------------------------

  /*
  *  define user SWAPHWSERIAL, if true(1) then swap uart pins from 3/1 to 13/15 (only ESP8266)
  */
  //#define SWAPHWSERIAL                      0                                   

#endif

//------------------------------------------------------------------------------

/*
*  define user MAX_MILLIS_TO_WAIT to wait for response from BZ40i
*/
//#define MAX_MILLIS_TO_WAIT                  500                                 

//------------------------------------------------------------------------------
