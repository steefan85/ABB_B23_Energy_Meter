/* Library for reading ABB B23 Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  2021 Stefan
*  Based on Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on SDM_Energy_Meter 2016-2020 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/
//------------------------------------------------------------------------------
#include "BZ40i.h"
//------------------------------------------------------------------------------
#if defined ( USE_HARDWARESERIAL )
  #if defined ( ESP8266 )
    BZ40i::BZ40i(HardwareSerial& serial, long baud, int dere_pin, int config, bool swapuart) : bz40iSer(serial) {
      this->_baud = baud;
      this->_config = config;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_swapuart = swapuart;
    }
  #elif defined ( ESP32 )
    BZ40i::BZ40i(HardwareSerial& serial, long baud, int dere_pin, int config, int8_t rx_pin, int8_t tx_pin) : bz40iSer(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_rx_pin = rx_pin;
      this->_tx_pin = tx_pin;
    }
  #else
    BZ40i::BZ40i(HardwareSerial& serial, long baud, int dere_pin, int config) : bz40iSer(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
    }
  #endif
#else
  #if defined ( ESP8266 ) || defined ( ESP32 )
    BZ40i::BZ40i(SoftwareSerial& serial, long baud, int dere_pin, int config, int8_t rx_pin, int8_t tx_pin) : bz40iSer(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
      this->_config = config;
      this->_rx_pin = rx_pin;
      this->_tx_pin = tx_pin;
    }
  #else
    BZ40i::BZ40i(SoftwareSerial& serial, long baud, int dere_pin) : bz40iSer(serial) {
      this->_baud = baud;
      this->_dere_pin = dere_pin;
    }
  #endif
#endif

BZ40i::~BZ40i() {
}

void BZ40i::begin(void) {
  #if defined ( USE_HARDWARESERIAL )
    #if defined ( ESP8266 )
      bz40iSer.begin(_baud, (SerialConfig)_config);
    #elif defined ( ESP32 )
      bz40iSer.begin(_baud, _config, _rx_pin, _tx_pin);
    #else
      bz40iSer.begin(_baud, _config);
    #endif
  #else
    #if defined ( ESP8266 ) || defined ( ESP32 )
      bz40iSer.begin(_baud, (SoftwareSerialConfig)_config, _rx_pin, _tx_pin);
    #else
      bz40iSer.begin(_baud);
    #endif
  #endif
  
  #if defined ( USE_HARDWARESERIAL ) && defined ( ESP8266 )
    if (_swapuart)
      bz40iSer.swap();
  #endif
    if (_dere_pin != NOT_A_PIN) {
      pinMode(_dere_pin, OUTPUT);                                                 //set output pin mode for DE/RE pin when used (for control MAX485)
    }
    dereSet(LOW);                                                                 //set init state to receive from BZ40i -> DE Disable, /RE Enable (for control MAX485)
}

float BZ40i::readVal(uint16_t reg, byte type, uint8_t node) {
  uint16_t temp;
  unsigned long resptime;
  uint8_t bz40iarr[FRAMESIZE_MAX];
  float res = NAN;
  uint16_t readErr = BZ40i_ERR_NO_ERROR;

  for(temp=0; temp<FRAMESIZE_MAX; temp++) bz40iarr[temp] = 0x00;

  bz40iarr[0] = node;
  bz40iarr[1] = BZ40i_B_02;

  bz40iarr[2] = highByte(reg);
  bz40iarr[3] = lowByte(reg);

  bz40iarr[4] = BZ40i_B_05;
  bz40iarr[5] = BZ40i_B_06;

  temp = calculateCRC(bz40iarr, FRAMESIZE_OUT - 3);                               //calculate out crc only from first 6 bytes

  bz40iarr[6] = lowByte(temp);
  bz40iarr[7] = highByte(temp);

  #if !defined ( USE_HARDWARESERIAL )
    bz40iSer.listen();                                                            //enable softserial rx interrupt
  #endif

  flush(); //read serial if any old data is available

  dereSet(HIGH); //transmit to BZ40i -> DE Enable, /RE Disable (for control MAX485)

  delay(2);                                                                     //fix for issue (nan reading) by sjfaustino: https://github.com/reaper7/BZ40i_Energy_Meter/issues/7#issuecomment-272111524

  bz40iSer.write(bz40iarr, FRAMESIZE_OUT - 1);                                      //send 8 bytes

  bz40iSer.flush();                                                               //clear out tx buffer

  dereSet(LOW); //receive from BZ40i -> DE Disable, /RE Enable (for control MAX485)

  resptime = millis() + WAITING_TURNAROUND_DELAY;

  while (bz40iSer.available() < FRAMESIZE_IN) {
    if (resptime < millis()) {
      readErr = BZ40i_ERR_TIMEOUT;                                                //err debug (4)
      break;
    }
    yield();
  }

  if (readErr == BZ40i_ERR_NO_ERROR) {                                            //if no timeout...

    if (bz40iSer.available() >= FRAMESIZE_IN) {

      for(int n=0; n<FRAMESIZE_IN; n++) {
        bz40iarr[n] = bz40iSer.read();
      }

      if (bz40iarr[0] == node && bz40iarr[1] == BZ40i_B_02 && bz40iarr[2] == BZ40i_REPLY_BYTE_COUNT) {

        if ((calculateCRC(bz40iarr, FRAMESIZE_IN - 2)) == ((bz40iarr[(FRAMESIZE_IN-1)] << 8) | bz40iarr[(FRAMESIZE_IN-2)])) {  //calculate crc and compare with received crc
          if(type == 1) {
            int32_t sinput = 0; //4 words, signed
            ((uint8_t*)&sinput)[7]= bz40iarr[3];
            ((uint8_t*)&sinput)[6]= bz40iarr[4];
            ((uint8_t*)&sinput)[5]= bz40iarr[5];
            ((uint8_t*)&sinput)[4]= bz40iarr[6];
            ((uint8_t*)&sinput)[3]= bz40iarr[7];
            ((uint8_t*)&sinput)[2]= bz40iarr[8];
            ((uint8_t*)&sinput)[1]= bz40iarr[9];
            ((uint8_t*)&sinput)[0]= bz40iarr[10];
            res = sinput;
            res /= 100;
          }else if(type == 0){
            uint32_t uinput = 0; //4 words, unsigned
            ((uint8_t*)&uinput)[3]= bz40iarr[7];
            ((uint8_t*)&uinput)[2]= bz40iarr[8];
            ((uint8_t*)&uinput)[1]= bz40iarr[9];
            ((uint8_t*)&uinput)[0]= bz40iarr[10];
            res = uinput;
            res /= 100;
          }else if(type == 2){  //1 word, unsigned
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[1]= bz40iarr[3];
            ((uint8_t*)&uinput)[0]= bz40iarr[4];
            res = uinput;
            res /= 100;
          }else if(type == 3){  //2 words, unsigned
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[3]= bz40iarr[3];
            ((uint8_t*)&uinput)[2]= bz40iarr[4];
            ((uint8_t*)&uinput)[1]= bz40iarr[5];
            ((uint8_t*)&uinput)[0]= bz40iarr[6];
            res = uinput;
            res /= 10;
          }else{
            uint32_t uinput = 0;  //4 words, unsigned
            ((uint8_t*)&uinput)[3]= bz40iarr[7];
            ((uint8_t*)&uinput)[2]= bz40iarr[8];
            ((uint8_t*)&uinput)[1]= bz40iarr[9];
            ((uint8_t*)&uinput)[0]= bz40iarr[10];
            res = uinput;
          }
        } else {
          readErr = BZ40i_ERR_CRC_ERROR;                                          //err debug (1)
        }

      } else {
        readErr = BZ40i_ERR_WRONG_BYTES;                                          //err debug (2)
      }

    } else {
      readErr = BZ40i_ERR_NOT_ENOUGHT_BYTES;                                      //err debug (3)
    }

  }

  flush(RESPONSE_TIMEOUT);                                                        //read serial if any old data is available and wait for RESPONSE_TIMEOUT (in ms)

  if (bz40iSer.available())                                                       //if serial rx buffer (after RESPONSE_TIMEOUT) still contains data then something spam rs485, check node(s) or increase RESPONSE_TIMEOUT
    readErr = BZ40i_ERR_TIMEOUT;                                                    //err debug (4) but returned value may be correct

  if (readErr != BZ40i_ERR_NO_ERROR) {                                            //if error then copy temp error value to global val and increment global error counter
    readingerrcode = readErr;
    readingerrcount++; 
  } else {
    ++readingsuccesscount;
  }

  while (bz40iSer.available() > 0)  {                                             //read redundant serial bytes, if any
    bz40iSer.read();
  }

  #if !defined ( USE_HARDWARESERIAL )
    bz40iSer.stopListening();                                                     //disable softserial rx interrupt
  #endif

  return (res);
}

uint16_t BZ40i::getErrCode(bool _clear) {
  uint16_t _tmp = readingerrcode;
  if (_clear == true)
    clearErrCode();
  return (_tmp);
}

uint32_t BZ40i::getErrCount(bool _clear) {
  uint32_t _tmp = readingerrcount;
  if (_clear == true)
    clearErrCount();
  return (_tmp);
}

uint32_t BZ40i::getSuccCount(bool _clear) {
  uint32_t _tmp = readingsuccesscount;
  if (_clear == true)
    clearSuccCount();
  return (_tmp);
}

void BZ40i::clearErrCode() {
  readingerrcode = BZ40i_ERR_NO_ERROR;
}

void BZ40i::clearErrCount() {
  readingerrcount = 0;
}

void BZ40i::clearSuccCount() {
  readingsuccesscount = 0;
}

uint16_t BZ40i::calculateCRC(uint8_t *array, uint8_t len) {
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

void BZ40i::flush(unsigned long _flushtime) {
  unsigned long flushtime = millis() + _flushtime;
  while (bz40iSer.available() || flushtime >= millis()) {
    if (bz40iSer.available())                                                     //read serial if any old data is available
      bz40iSer.read();
    delay(1);
  }
}

void BZ40i::dereSet(bool _state) {
  if (_dere_pin != NOT_A_PIN)
    digitalWrite(_dere_pin, _state);                                            //receive from BZ40i -> DE Disable, /RE Enable (for control MAX485)
}
