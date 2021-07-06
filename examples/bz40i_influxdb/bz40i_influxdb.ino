
//REMEMBER! uncomment #define USE_HARDWARESERIAL 
//in BZ40i_Config_User.h file if you want to use hardware uart

#include <BZ40i.h>                      //import BZ40i library
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <InfluxDbClient.h>           //https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino



// WiFi Parameters
const char* ssid = "SSID";
const char* password = "PASS";
#define HOSTNAME "BZ40i"              // Friedly hostname



// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
// InfluxDB v1 server url "http://192.168.10.2:8086"
#define INFLUXDB_URL "http://INFLUXDB IP:8086"
// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
//#define INFLUXDB_TOKEN "server token"
// InfluxDB v2 organization id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
//#define INFLUXDB_ORG "org id"
// InfluxDB v2 bucket name (Use: InfluxDB UI -> Load Data -> Buckets)
//#define INFLUXDB_BUCKET "bucket name"
//influxdb v1 database
#define INFLUXDB_DB_NAME "DATABASE"
//influxdb v1 user and pass
#define INFLUXDB_USER "USERNAME"
#define INFLUXDB_PASSWORD "PASSWORD"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time:   "PST8PDT"
//  Eastern:        "EST5EDT"
//  Japanesse:      "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
#define WRITE_PRECISION WritePrecision::S
#define MAX_BATCH_SIZE 10
#define WRITE_BUFFER_SIZE 30

// InfluxDB client instance with preconfigured InfluxCloud certificate
//InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

// Number for loops to sync time using NTP
int iterations = 0;

//REMEMBER TO SET BAUD IN BZ40i_Config_User.h TO WHAT YOUR METER SET SET TO

#if defined ( USE_HARDWARESERIAL )                                              //for HWSERIAL

#if defined ( ESP8266 )                                                         //for ESP8266
    BZ40i bz40i(Serial, BZ40i_UART_BAUD, DERE_PIN, BZ40i_UART_CONFIG);          //config BZ40i
#else                                                                           //for AVR
    BZ40i bz40i(Serial, BZ40i_UART_BAUD, DERE_PIN);                             //config BZ40i on Serial1 (if available!)
#endif

#else                                                                           //for SWSERIAL

#include <SoftwareSerial.h>                                                     //import SoftwareSerial library
#if defined ( ESP8266 )                                                         //for ESP
SoftwareSerial swSerBZ40i;                                                      //config SoftwareSerial
    BZ40i bz40i(swSerBZ40i, BZ40i_UART_BAUD, DERE_PIN, BZ40i_UART_CONFIG, BZ40i_RX_PIN, BZ40i_TX_PIN); //config BZ40i
#else                                                                           //for AVR
    SoftwareSerial swSerBZ40i(BZ40i_RX_PIN, BZ40i_TX_PIN);                      //config SoftwareSerial
    BZ40i bz40i(swSerBZ40i, BZ40i_UART_BAUD, DERE_PIN);                         //config BZ40i
#endif

#endif //#if defined ( USE_HARDWARESERIAL )

#define READBZ40iEVERY  1000                                                    //read BZ40i every 2000ms
#define NBREG 37


typedef struct {
  float regvalarr;
  const uint16_t regarr;
  const String regtext;
  byte isSigned;
} bz40i_struct;


bz40i_struct bz40iarr[NBREG] = {
  {"BZ40i_U_SYSTEM",        0.00, BZ40i_U_SYSTEM,       0},
  {"BZ40i_U_LN_P1",         0.00, BZ40i_U_LN_P1,        0},
  {"BZ40i_U_LN_P2",         0.00, BZ40i_U_LN_P2,        0},
  {"BZ40i_U_LN_P3",         0.00, BZ40i_U_LN_P3,        0},
  {"BZ40i_U_LL_P12",        0.00, BZ40i_U_LL_P12,       0},
  {"BZ40i_U_LL_P23",        0.00, BZ40i_U_LL_P23,       0},
  {"BZ40i_U_LL_P31",        0.00, BZ40i_U_LL_P31,       0},
  {"BZ40i_I_SYSTEM",        0.00, BZ40i_I_SYSTEM,       1},
  {"BZ40i_I_P1",            0.00, BZ40i_I_P1,           1},
  {"BZ40i_I_P2",            0.00, BZ40i_I_P2,           1},
  {"BZ40i_I_P3",            0.00, BZ40i_I_P3,           1},
  {"BZ40i_I_N",             0.00, BZ40i_I_N,            0},
  {"BZ40i_PF_SYSTEM",       0.00, BZ40i_PF_SYSTEM,      1},
  {"BZ40i_PF_P1",           0.00, BZ40i_PF_P1,          1},
  {"BZ40i_PF_P2",           0.00, BZ40i_PF_P2,          1},
  {"BZ40i_PF_P3",           0.00, BZ40i_PF_P3,          1},
  {"BZ40i_S_SYSTEM",        0.00, BZ40i_S_SYSTEM,       1},
  {"BZ40i_S_P1",            0.00, BZ40i_S_P1,           1},
  {"BZ40i_S_P2",            0.00, BZ40i_S_P2,           1},
  {"BZ40i_S_P3",            0.00, BZ40i_S_P3,           1},
  {"BZ40i_P_SYSTEM",        0.00, BZ40i_P_SYSTEM,       1},
  {"BZ40i_P_P1",            0.00, BZ40i_P_P1,           1},
  {"BZ40i_P_P2",            0.00, BZ40i_P_P2,           1},
  {"BZ40i_P_P3",            0.00, BZ40i_P_P3,           1},
  {"BZ40i_Q_SYSTEM",        0.00, BZ40i_Q_SYSTEM,       1},
  {"BZ40i_Q_P1",            0.00, BZ40i_Q_P1,           1},
  {"BZ40i_Q_P2",            0.00, BZ40i_Q_P2,           1},
  {"BZ40i_Q_P3",            0.00, BZ40i_Q_P3,           1},
  {"BZ40i_F",               0.00, BZ40i_F,              2},
  {"BZ40i_IMPORT_P",        0.00, BZ40i_IMPORT_P,       0},
  {"BZ40i_IMPORT_Q_LAG",    0.00, BZ40i_IMPORT_Q_LAG,   0},
  {"BZ40i_IMPORT_Q_LEAD",   0.00, BZ40i_IMPORT_Q_LEAD,  0},
  {"BZ40i_IMPORT_S",        0.00, BZ40i_IMPORT_S,       0},
  {"BZ40i_EXPORT_P",        0.00, BZ40i_EXPORT_P,       0},
  {"BZ40i_EXPORT_Q_LAG",    0.00, BZ40i_EXPORT_Q_LAG,   0},
  {"BZ40i_EXPORT_Q_LEAD",   0.00, BZ40i_EXPORT_Q_LEAD,  0},
  {"BZ40i_EXPORT_S",        0.00, BZ40i_EXPORT_S,       0}
};

unsigned long readtime;
time_t ntpLastUpdate;
int ntpSyncTime = 3600;
bool read_done = false;


void setup() {
  //Serial.begin(115200);                                                       //initialize serial
  bz40i.begin();                                                                //initialize BZ40i communication

  // Setup wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Connecting...");
  }
  
  // ***************************************************************************
  // Setup: MDNS responder
  // ***************************************************************************
  MDNS.begin(HOSTNAME);
  //Serial.print("Hostname: ");
  //Serial.print(HOSTNAME);

  ArduinoOTA.begin();


  // Set InfluxDB 1 authentication params
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);
  
  // Sync time for certificate validation
  timeSync();

  // Check server connection
  if (client.validateConnection()) {
    //Serial.print("Connected to InfluxDB: ");
    //Serial.println(client.getServerUrl());
  } else {
    //Serial.print("InfluxDB connection failed: ");
    //Serial.println(client.getLastErrorMessage());
  }

  //Enable messages batching and retry buffer
  client.setWriteOptions(WRITE_PRECISION, MAX_BATCH_SIZE, WRITE_BUFFER_SIZE);


  
}

void loop() {
  // Sync time for batching once per hour
  if(time(nullptr)-ntpLastUpdate > ntpSyncTime) {//if (iterations++ >= 360) {
    timeSync();
    iterations = 0;
  }

  if (millis() - readtime >= READBZ40iEVERY) {
    bz40iRead();
    readtime = millis();
  }

  if(read_done){
//put data to influx in buffer
    time_t tnow = time(nullptr);
    for (int i = 0; i < NBREG; i++) {
      Point powerMeter("BZ40i");
      //powerMeter.addTag("device", "Main");
      powerMeter.addTag("Type", bz40iarr[i].regtext );
      //powerMeter.addTag("channel", String(WiFi.channel(i)));
      //powerMeter.addTag("open", String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN));
      powerMeter.addField("value", bz40iarr[i].regvalarr);
      powerMeter.setTime(tnow);  //set the time

      // Print what are we exactly writing
      //Serial.print("Writing: ");
      //Serial.println(powerMeter.toLineProtocol());

      // Write point into buffer - low priority measures
      client.writePoint(powerMeter);
 
    }





  // End of the iteration - force write of all the values into InfluxDB as single transaction
  //Serial.println("Flushing data into InfluxDB");
  if (!client.flushBuffer()) {
    //Serial.print("InfluxDB flush failed: ");
    //Serial.println(client.getLastErrorMessage());
    //Serial.print("Full buffer: ");
    //Serial.println(client.isBufferFull() ? "Yes" : "No");
  }
  

  

  
    for(int i = 0; i < NBREG; i++) {
      //Serial.print(bz40iarr[i].regtext);
      //Serial.print(" = ");
      //Serial.println(bz40iarr[i].regvalarr);
    
    }
    read_done = false;
    
    
  }

  ArduinoOTA.handle();
  yield();
  /*Serial.println("Wait 10s");
  delay(10000);*/
}


//------------------------------------------------------------------------------

void timeSync() {
  // Synchronize UTC time with NTP servers
  // Accurate time is necessary for certificate validaton and writing in batches
  configTime(0, 0, "pool.ntp.org", "time.nis.gov");
  // Set timezone
  setenv("TZ", TZ_INFO, 1);

  // Wait till time is synced
  //Serial.print("Syncing time");
  int i = 0;
  while (time(nullptr) < 1000000000ul && i < 100) {
    //Serial.print(".");
    delay(100);
    i++;
  }
  //Serial.println();

  // Show time
  time_t tnow = time(nullptr);
  ntpLastUpdate = time(nullptr);
  //Serial.print("Synchronized time: ");
  //Serial.println(String(ctime(&tnow)));
}



void bz40iRead() {
  float tmpval = NAN;

  for (uint8_t i = 0; i < NBREG; i++) {
    tmpval = bz40i.readVal(bz40iarr[i].regarr);

    if (isnan(tmpval))
      bz40iarr[i].regvalarr = 0.00;
    else
      bz40iarr[i].regvalarr = tmpval;

    yield();
  }
  read_done = true;
}