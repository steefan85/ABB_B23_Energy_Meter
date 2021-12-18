/**
 * Simple ABB_B23 Example
 * 
 * Dump measurements to serial console
 */

//REMEMBER! uncomment #define USE_HARDWARESERIAL
//in ABB_B23_Config_User.h file if you want to use hardware uart

#include <ABB_B23.h>                                                              //import ABB_B23 library

#if defined ( USE_HARDWARESERIAL )                                              //for HWSERIAL
  #if defined ( ESP8266 )                                                       //for ESP8266
    ABB_B23 ABB_B23(Serial1, ABB_B23_UART_BAUD, DERE_PIN, ABB_B23_UART_CONFIG);                //config ABB_B23
  #elif defined ( ESP32 )                                                       //for ESP32
    ABB_B23 ABB_B23(Serial1, ABB_B23_UART_BAUD, DERE_PIN, ABB_B23_UART_CONFIG, ABB_B23_RX_PIN, ABB_B23_TX_PIN); //config ABB_B23
  #else                                                                         //for AVR
   ABB_B23 ABB_B23(Serial1, ABB_B23_UART_BAUD, DERE_PIN);                             //config ABB_B23 on Serial1 (if available!)
  #endif
#else                                                                           //for SWSERIAL
  #include <SoftwareSerial.h>                                                   //import SoftwareSerial library
  #if defined ( ESP8266 ) || defined ( ESP32 )                                  //for ESP
    SoftwareSerial swSerABB_B23;                                                  //config SoftwareSerial
    ABB_B23 ABB_B23(swSerABB_B23, ABB_B23_UART_BAUD, DERE_PIN, ABB_B23_UART_CONFIG, ABB_B23_RX_PIN, ABB_B23_TX_PIN); //config ABB_B23
  #else                                                                         //for AVR
    SoftwareSerial swSerABB_B23(ABB_B23_RX_PIN, ABB_B23_TX_PIN);                      //config SoftwareSerial
    ABB_B23 ABB_B23(swSerABB_B23, ABB_B23_UART_BAUD, DERE_PIN);                         //config ABB_B23
  #endif
#endif

#define NBREG 3

typedef struct {
  String dname;
  const uint16_t regarr;
  byte isSigned;
} ABB_B23_struct;

ABB_B23_struct ABB_B23arr[NBREG] = {
  {"ABB_F",               ABB_F,              1},   //1 = Read 1 word unsigned
  {"ABB_U_LN_P1",         ABB_U_LN_P1,        2},   //2 = Read 2 words unsigned
    {"ABB_IMPORT_P",        ABB_IMPORT_P,     4},   //4 = Read 4 words unsigned

};

void setup() {
  Serial.begin(115200);                                                         //initialize serial
  ABB_B23.begin();                                                                //initialize ABB_B23 communication
}

void loop() {
  float temp = NAN;
  Serial.println("Reading...");

  for (uint8_t i = 0; i < NBREG; i++) {
    Serial.print("  ADDR: 0x");
    Serial.print(ABB_B23arr[i].regarr, 16);
    temp = ABB_B23.readVal(ABB_B23arr[i].regarr, ABB_B23arr[i].isSigned);

    Serial.print(" = ");
    Serial.print(temp);

    Serial.print(" (");
    Serial.print(ABB_B23arr[i].dname);
    Serial.println(")");

    yield();
  }

  delay(1000);                                                                  //wait a while before next loop
}