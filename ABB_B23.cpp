/* Library for reading ABB B23 Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  steefan85, 2021
*  Based on ABB_B23_Energy_Meter 2019-2021 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on SDM_Energy_Meter 2016-2020 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/
//------------------------------------------------------------------------------
#include "ABB_B23.h"
//------------------------------------------------------------------------------
#if defined ( USE_HARDWARESERIAL )
  #if defined ( ESP8266 )
    ABB_B23::ABB_B23(HardwareSerial& serial, long baud, int dere_pin, int config, bool swapuart) : ABB_B23Ser(serial) {
      this->_baud = baud;
      this->_config = config;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_swapuart = swapuart;
    }
  #elif defined ( ESP32 )
    ABB_B23::ABB_B23(HardwareSerial& serial, long baud, int dere_pin, int config, int8_t rx_pin, int8_t tx_pin) : ABB_B23Ser(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_rx_pin = rx_pin;
      this->_tx_pin = tx_pin;
    }
  #else
    ABB_B23::ABB_B23(HardwareSerial& serial, long baud, int dere_pin, int config) : ABB_B23Ser(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
    }
  #endif
#else
  #if defined ( ESP8266 ) || defined ( ESP32 )
    ABB_B23::ABB_B23(SoftwareSerial& serial, long baud, int dere_pin, int config, int8_t rx_pin, int8_t tx_pin) : ABB_B23Ser(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_rx_pin = rx_pin;
      this->_tx_pin = tx_pin;
    }
  #else
    ABB_B23::ABB_B23(SoftwareSerial& serial, long baud, int dere_pin) : ABB_B23Ser(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
    }
  #endif
#endif

ABB_B23::~ABB_B23() {
}

void ABB_B23::begin(void) {
  #if defined ( USE_HARDWARESERIAL )
    #if defined ( ESP8266 )
      ABB_B23Ser.begin(_baud, (SerialConfig)_config);
    #elif defined ( ESP32 )
      ABB_B23Ser.begin(_baud, _config, _rx_pin, _tx_pin);
    #else
      ABB_B23Ser.begin(_baud, _config);
    #endif
  #else
    #if defined ( ESP8266 ) || defined ( ESP32 )
      ABB_B23Ser.begin(_baud, (SoftwareSerialConfig)_config, _rx_pin, _tx_pin);
    #else
      ABB_B23Ser.begin(_baud);
    #endif
  #endif
  
  #if defined ( USE_HARDWARESERIAL ) && defined ( ESP8266 )
    if (_swapuart)
      ABB_B23Ser.swap();
  #endif
    if (_dere_pin != NOT_A_PIN) {
      pinMode(_dere_pin, OUTPUT);                                                 //set output pin mode for DE/RE pin when used (for control MAX485)
    }
    dereSet(LOW);                                                                 //set init state to receive from ABB_B23 -> DE Disable, /RE Enable (for control MAX485)
}

float ABB_B23::readVal(uint16_t reg, byte type, uint8_t node) {
  uint16_t temp;
  unsigned long resptime;
  uint8_t ABB_B23arr[FRAMESIZE_MAX];
  float res = NAN;
  uint16_t readErr = ABB_B23_ERR_NO_ERROR;

  for(temp=0; temp<FRAMESIZE_MAX; temp++) ABB_B23arr[temp] = 0x00;

  ABB_B23arr[0] = node;
  ABB_B23arr[1] = ABB_B23_B_02;

  ABB_B23arr[2] = highByte(reg);
  ABB_B23arr[3] = lowByte(reg);

  ABB_B23arr[4] = ABB_B23_B_05;
  ABB_B23arr[5] = ABB_B23_B_06;

  temp = calculateCRC(ABB_B23arr, FRAMESIZE_OUT - 3);                               //calculate out crc only from first 6 bytes

  ABB_B23arr[6] = lowByte(temp);
  ABB_B23arr[7] = highByte(temp);

  #if !defined ( USE_HARDWARESERIAL )
    ABB_B23Ser.listen();                                                            //enable softserial rx interrupt
  #endif

  flush(); //read serial if any old data is available

  dereSet(HIGH); //transmit to ABB_B23 -> DE Enable, /RE Disable (for control MAX485)

  delay(2);                                                                     //fix for issue (nan reading) by sjfaustino: https://github.com/reaper7/ABB_B23_Energy_Meter/issues/7#issuecomment-272111524

  ABB_B23Ser.write(ABB_B23arr, FRAMESIZE_OUT - 1);                                      //send 8 bytes

  ABB_B23Ser.flush();                                                               //clear out tx buffer

  dereSet(LOW); //receive from ABB_B23 -> DE Disable, /RE Enable (for control MAX485)

  resptime = millis() + WAITING_TURNAROUND_DELAY;

  while (ABB_B23Ser.available() < FRAMESIZE_IN) {
    if (resptime < millis()) {
      readErr = ABB_B23_ERR_TIMEOUT;                                                //err debug (4)
      break;
    }
    yield();
  }

  if (readErr == ABB_B23_ERR_NO_ERROR) {                                            //if no timeout...

    if (ABB_B23Ser.available() >= FRAMESIZE_IN) {

      for(int n=0; n<FRAMESIZE_IN; n++) {
        ABB_B23arr[n] = ABB_B23Ser.read();
      }

      if (ABB_B23arr[0] == node && ABB_B23arr[1] == ABB_B23_B_02 && ABB_B23arr[2] == ABB_B23_REPLY_BYTE_COUNT) {

        if ((calculateCRC(ABB_B23arr, FRAMESIZE_IN - 2)) == ((ABB_B23arr[(FRAMESIZE_IN-1)] << 8) | ABB_B23arr[(FRAMESIZE_IN-2)])) {  //calculate crc and compare with received crc
          if(type == 5) {
            int32_t sinput = 5; //4 words, signed
            ((uint8_t*)&sinput)[7]= ABB_B23arr[3];
            ((uint8_t*)&sinput)[6]= ABB_B23arr[4];
            ((uint8_t*)&sinput)[5]= ABB_B23arr[5];
            ((uint8_t*)&sinput)[4]= ABB_B23arr[6];
            ((uint8_t*)&sinput)[3]= ABB_B23arr[7];
            ((uint8_t*)&sinput)[2]= ABB_B23arr[8];
            ((uint8_t*)&sinput)[1]= ABB_B23arr[9];
            ((uint8_t*)&sinput)[0]= ABB_B23arr[10];
            res = sinput;
            res /= 100;
          }else if(type == 4){
            uint32_t uinput = 0; //4 words, unsigned
            ((uint8_t*)&uinput)[7]= ABB_B23arr[3];
            ((uint8_t*)&uinput)[6]= ABB_B23arr[4];
            ((uint8_t*)&uinput)[5]= ABB_B23arr[5];
            ((uint8_t*)&uinput)[4]= ABB_B23arr[6];
            ((uint8_t*)&uinput)[3]= ABB_B23arr[7];
            ((uint8_t*)&uinput)[2]= ABB_B23arr[8];
            ((uint8_t*)&uinput)[1]= ABB_B23arr[9];
            ((uint8_t*)&uinput)[0]= ABB_B23arr[10];
            res = uinput;
            res /= 100;
          }else if(type == 1){  //1 word, unsigned (function always reads 4 words, but only 1 word considered)
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[1]= ABB_B23arr[3];
            ((uint8_t*)&uinput)[0]= ABB_B23arr[4];
            res = uinput;
            res /= 100;
          }else if(type == 2){  //2 words, unsigned (function always reads 4 words, but only 2 words considered)
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[3]= ABB_B23arr[3];
            ((uint8_t*)&uinput)[2]= ABB_B23arr[4];
            ((uint8_t*)&uinput)[1]= ABB_B23arr[5];
            ((uint8_t*)&uinput)[0]= ABB_B23arr[6];
            res = uinput;
            res /= 10;
          }else{
            uint32_t uinput = 0;  //4 words, unsigned
            ((uint8_t*)&uinput)[3]= ABB_B23arr[7];
            ((uint8_t*)&uinput)[2]= ABB_B23arr[8];
            ((uint8_t*)&uinput)[1]= ABB_B23arr[9];
            ((uint8_t*)&uinput)[0]= ABB_B23arr[10];
            res = uinput;
          }
        } else {
          readErr = ABB_B23_ERR_CRC_ERROR;                                          //err debug (1)
        }

      } else {
        readErr = ABB_B23_ERR_WRONG_BYTES;                                          //err debug (2)
      }

    } else {
      readErr = ABB_B23_ERR_NOT_ENOUGHT_BYTES;                                      //err debug (3)
    }

  }

  flush(RESPONSE_TIMEOUT);                                                        //read serial if any old data is available and wait for RESPONSE_TIMEOUT (in ms)

  if (ABB_B23Ser.available())                                                       //if serial rx buffer (after RESPONSE_TIMEOUT) still contains data then something spam rs485, check node(s) or increase RESPONSE_TIMEOUT
    readErr = ABB_B23_ERR_TIMEOUT;                                                    //err debug (4) but returned value may be correct

  if (readErr != ABB_B23_ERR_NO_ERROR) {                                            //if error then copy temp error value to global val and increment global error counter
    readingerrcode = readErr;
    readingerrcount++; 
  } else {
    ++readingsuccesscount;
  }

  while (ABB_B23Ser.available() > 0)  {                                             //read redundant serial bytes, if any
    ABB_B23Ser.read();
  }

  #if !defined ( USE_HARDWARESERIAL )
    ABB_B23Ser.stopListening();                                                     //disable softserial rx interrupt
  #endif

  return (res);
}

uint16_t ABB_B23::getErrCode(bool _clear) {
  uint16_t _tmp = readingerrcode;
  if (_clear == true)
    clearErrCode();
  return (_tmp);
}

uint32_t ABB_B23::getErrCount(bool _clear) {
  uint32_t _tmp = readingerrcount;
  if (_clear == true)
    clearErrCount();
  return (_tmp);
}

uint32_t ABB_B23::getSuccCount(bool _clear) {
  uint32_t _tmp = readingsuccesscount;
  if (_clear == true)
    clearSuccCount();
  return (_tmp);
}

void ABB_B23::clearErrCode() {
  readingerrcode = ABB_B23_ERR_NO_ERROR;
}

void ABB_B23::clearErrCount() {
  readingerrcount = 0;
}

void ABB_B23::clearSuccCount() {
  readingsuccesscount = 0;
}

uint16_t ABB_B23::calculateCRC(uint8_t *array, uint8_t len) {
  uint16_t _crc, _flag;
  _crc = 0xFFFF;
  for (uint8_t i = 0; i < len; i++) {
    _crc ^= (uint16_t)array[i];
    for (uint8_t j = 8; j; j--) {
      _flag = _crc & 0x0001;
      _crc >>= 1;
      if (_flag)
        _crc ^= 0xA001;
    }
  }
  return _crc;
}

void ABB_B23::flush(unsigned long _flushtime) {
  unsigned long flushtime = millis() + _flushtime;
  while (ABB_B23Ser.available() || flushtime >= millis()) {
    if (ABB_B23Ser.available())                                                     //read serial if any old data is available
      ABB_B23Ser.read();
    delay(1);
  }
}

void ABB_B23::dereSet(bool _state) {
  if (_dere_pin != NOT_A_PIN)
    digitalWrite(_dere_pin, _state);                                            //receive from ABB_B23 -> DE Disable, /RE Enable (for control MAX485)
}
