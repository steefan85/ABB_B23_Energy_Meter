/**
 * Simple BZ40i Example
 * 
 * Dump measurements to serial console
 *  
 * Requires:
 * https://github.com/plerup/espsoftwareserial#4.0.0
 */
#include <SoftwareSerial.h>                                                     //import SoftwareSerial library
#include <BZ40i.h>                                                              //import BZ40i library

SoftwareSerial swSerBZ40i(D1, D2);                                              //config SoftwareSerial (rx->D1 / tx->D2)

BZ40i bz40i(swSerBZ40i, 4800, NOT_A_PIN);                                       //config BZ40i

#define NBREG 37

typedef struct {
  String dname;
  const uint16_t regarr;
  byte isSigned;
} bz40i_struct;

bz40i_struct bz40iarr[NBREG] = {
  {"BZ40i_U_SYSTEM",        BZ40i_U_SYSTEM,       0},
  {"BZ40i_U_LN_P1",         BZ40i_U_LN_P1,        0},
  {"BZ40i_U_LN_P2",         BZ40i_U_LN_P2,        0},
  {"BZ40i_U_LN_P3",         BZ40i_U_LN_P3,        0},
  {"BZ40i_U_LL_P12",        BZ40i_U_LL_P12,       0},
  {"BZ40i_U_LL_P23",        BZ40i_U_LL_P23,       0},
  {"BZ40i_U_LL_P31",        BZ40i_U_LL_P31,       0},
  {"BZ40i_I_SYSTEM",        BZ40i_I_SYSTEM,       1},
  {"BZ40i_I_P1",            BZ40i_I_P1,           1},
  {"BZ40i_I_P2",            BZ40i_I_P2,           1},
  {"BZ40i_I_P3",            BZ40i_I_P3,           1},
  {"BZ40i_I_N",             BZ40i_I_N,            0},
  {"BZ40i_PF_SYSTEM",       BZ40i_PF_SYSTEM,      1},
  {"BZ40i_PF_P1",           BZ40i_PF_P1,          1},
  {"BZ40i_PF_P2",           BZ40i_PF_P2,          1},
  {"BZ40i_PF_P3",           BZ40i_PF_P3,          1},
  {"BZ40i_S_SYSTEM",        BZ40i_S_SYSTEM,       1},
  {"BZ40i_S_P1",            BZ40i_S_P1,           1},
  {"BZ40i_S_P2",            BZ40i_S_P2,           1},
  {"BZ40i_S_P3",            BZ40i_S_P3,           1},
  {"BZ40i_P_SYSTEM",        BZ40i_P_SYSTEM,       1},
  {"BZ40i_P_P1",            BZ40i_P_P1,           1},
  {"BZ40i_P_P2",            BZ40i_P_P2,           1},
  {"BZ40i_P_P3",            BZ40i_P_P3,           1},
  {"BZ40i_Q_SYSTEM",        BZ40i_Q_SYSTEM,       1},
  {"BZ40i_Q_P1",            BZ40i_Q_P1,           1},
  {"BZ40i_Q_P2",            BZ40i_Q_P2,           1},
  {"BZ40i_Q_P3",            BZ40i_Q_P3,           1},
  {"BZ40i_F",               BZ40i_F,              2},
  {"BZ40i_IMPORT_P",        BZ40i_IMPORT_P,       0},
  {"BZ40i_IMPORT_Q_LAG",    BZ40i_IMPORT_Q_LAG,   0},
  {"BZ40i_IMPORT_Q_LEAD",   BZ40i_IMPORT_Q_LEAD,  0},
  {"BZ40i_IMPORT_S",        BZ40i_IMPORT_S,       0},
  {"BZ40i_EXPORT_P",        BZ40i_EXPORT_P,       0},
  {"BZ40i_EXPORT_Q_LAG",    BZ40i_EXPORT_Q_LAG,   0},
  {"BZ40i_EXPORT_Q_LEAD",   BZ40i_EXPORT_Q_LEAD,  0},
  {"BZ40i_EXPORT_S",        BZ40i_EXPORT_S,       0}
};

void setup() {
  Serial.begin(115200);                                                         //initialize serial
  bz40i.begin();                                                                //initialize BZ40i communication
}

void loop() {
  float temp = NAN;
  Serial.println("Reading...");

  for (uint8_t i = 0; i < NBREG; i++) {
    Serial.print("  ADDR: 0x");
    Serial.print(bz40iarr[i].regarr, 16);
    temp = bz40i.readVal(bz40iarr[i].regarr, bz40iarr[i].isSigned);

    Serial.print(" = ");
    Serial.print(temp);

    Serial.print(" (");
    Serial.print(bz40iarr[i].dname);
    Serial.println(")");

    yield();
  }

  delay(1000);                                                                  //wait a while before next loop
}
