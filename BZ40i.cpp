/* Library for reading Berg BZ40i Modbus Energy meters.
*  Reading via Hardware or Software Serial library & rs232<->rs485 converter
*  2019 Florian "adlerweb" Knodt · www.adlerweb.info
*  Based on SDM_Energy_Meter 2016-2019 Reaper7
*  crc calculation by Jaime García (https://github.com/peninquen/Modbus-Energy-Monitor-Arduino/)
*/
//------------------------------------------------------------------------------
#include "BZ40i.h"
//------------------------------------------------------------------------------
#ifdef USE_HARDWARESERIAL
BZ40i::BZ40i(HardwareSerial& serial, long baud, int dere_pin, int config, bool swapuart) : sdmSer(serial) {
  this->_baud = baud;
  this->_config = config;
  this->_dere_pin = dere_pin;
  this->_swapuart = swapuart;
}
#else
BZ40i::BZ40i(SoftwareSerial& serial, long baud, int dere_pin) : sdmSer(serial) {
  this->_baud = baud;
  this->_dere_pin = dere_pin;
}
#endif

BZ40i::~BZ40i() {
}

void BZ40i::begin(void) {
#ifdef USE_HARDWARESERIAL
  #ifdef ESP8266
    sdmSer.begin(_baud, (SerialConfig)_config);
  #else
    sdmSer.begin(_baud, _config);
  #endif
#else
  sdmSer.begin(_baud);
#endif
#ifdef USE_HARDWARESERIAL
  #ifdef ESP8266
    if (_swapuart)
      sdmSer.swap();
  #endif
#endif
  if (_dere_pin != NOT_A_PIN)	                                                  //set output pin mode for DE/RE pin when used (for control MAX485)
    pinMode(_dere_pin, OUTPUT);
}

float BZ40i::readVal(uint16_t reg, byte type, uint8_t node) {
  uint16_t temp;
  unsigned long resptime;
  uint8_t sdmarr[FRAMESIZE_MAX];
  float res = NAN;
  uint16_t readErr = BZ40i_ERR_NO_ERROR;

  for(temp=0; temp<FRAMESIZE_MAX; temp++) sdmarr[temp] = 0x00;

  sdmarr[0] = node;
  sdmarr[1] = BZ40i_B_02;

  sdmarr[2] = highByte(reg);
  sdmarr[3] = lowByte(reg);

  sdmarr[4] = BZ40i_B_05;
  sdmarr[5] = BZ40i_B_06;

  temp = calculateCRC(sdmarr, FRAMESIZE_OUT - 3);                               //calculate out crc only from first 6 bytes

  sdmarr[6] = lowByte(temp);
  sdmarr[7] = highByte(temp);

  #ifndef USE_HARDWARESERIAL
    sdmSer.listen();                                                            //enable softserial rx interrupt
  #endif

  while (sdmSer.available() > 0)  {                                             //read serial if any old data is available
    sdmSer.read();
  }

  if (_dere_pin != NOT_A_PIN)                                                   //transmit to BZ40i  -> DE Enable, /RE Disable (for control MAX485)
    digitalWrite(_dere_pin, HIGH);

  delay(2);                                                                     //fix for issue (nan reading) by sjfaustino: https://github.com/reaper7/BZ40i_Energy_Meter/issues/7#issuecomment-272111524

  sdmSer.write(sdmarr, FRAMESIZE_OUT - 1);                                      //send 8 bytes

  sdmSer.flush();                                                               //clear out tx buffer

  if (_dere_pin != NOT_A_PIN)                                                   //receive from BZ40i -> DE Disable, /RE Enable (for control MAX485)
    digitalWrite(_dere_pin, LOW);

  resptime = millis() + MAX_MILLIS_TO_WAIT;

  while (sdmSer.available() < FRAMESIZE_IN) {
    if (resptime < millis()) {
      readErr = BZ40i_ERR_TIMEOUT;                                                //err debug (4)
      break;
    }
    yield();
  }

  if (readErr == BZ40i_ERR_NO_ERROR) {                                            //if no timeout...

    if(sdmSer.available() >= FRAMESIZE_IN) {

      for(int n=0; n<FRAMESIZE_IN; n++) {
        sdmarr[n] = sdmSer.read();
      }

      if (sdmarr[0] == node && sdmarr[1] == BZ40i_B_02 && sdmarr[2] == BZ40i_REPLY_BYTE_COUNT) {

        if ((calculateCRC(sdmarr, FRAMESIZE_IN - 2)) == ((sdmarr[(FRAMESIZE_IN-1)] << 8) | sdmarr[(FRAMESIZE_IN-2)])) {  //calculate crc and compare with received crc
          if(type == 1) {
            int32_t sinput = 0;
            ((uint8_t*)&sinput)[3]= sdmarr[7];
            ((uint8_t*)&sinput)[2]= sdmarr[8];
            ((uint8_t*)&sinput)[1]= sdmarr[9];
            ((uint8_t*)&sinput)[0]= sdmarr[10];
            res = sinput;
            res /= 1000;
          }else if(type == 0){
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[3]= sdmarr[7];
            ((uint8_t*)&uinput)[2]= sdmarr[8];
            ((uint8_t*)&uinput)[1]= sdmarr[9];
            ((uint8_t*)&uinput)[0]= sdmarr[10];
            res = uinput;
            res /= 1000;
          }else{
            uint32_t uinput = 0;
            ((uint8_t*)&uinput)[3]= sdmarr[7];
            ((uint8_t*)&uinput)[2]= sdmarr[8];
            ((uint8_t*)&uinput)[1]= sdmarr[9];
            ((uint8_t*)&uinput)[0]= sdmarr[10];
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

  if (readErr != BZ40i_ERR_NO_ERROR) {                                            //if error then copy temp error value to global val and increment global error counter
    readingerrcode = readErr;
    readingerrcount++; 
  }

  while (sdmSer.available() > 0)  {                                             //read redundant serial bytes, if any
    sdmSer.read();
  }

#ifndef USE_HARDWARESERIAL
  sdmSer.end();                                                                 //disable softserial rx interrupt
#endif

  return (res);
}

uint16_t BZ40i::getErrCode(bool _clear) {
  uint16_t _tmp = readingerrcode;
  if (_clear == true)
    clearErrCode();
  return (_tmp);
}

uint16_t BZ40i::getErrCount(bool _clear) {
  uint16_t _tmp = readingerrcount;
  if (_clear == true)
    clearErrCount();
  return (_tmp);
}

void BZ40i::clearErrCode() {
  readingerrcode = BZ40i_ERR_NO_ERROR;
}

void BZ40i::clearErrCount() {
  readingerrcount = 0;
}

uint16_t BZ40i::calculateCRC(uint8_t *array, uint8_t num) {
  uint16_t _crc, _flag;
  _crc = 0xFFFF;
  for (uint8_t i = 0; i < num; i++) {
    _crc = _crc ^ array[i];
    for (uint8_t j = 8; j; j--) {
      _flag = _crc & 0x0001;
      _crc >>= 1;
      if (_flag)
        _crc ^= 0xA001;
    }
  }
  return _crc;
}
