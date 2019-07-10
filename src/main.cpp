#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <dht.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(3, 4); // Arduino TX, RX , 
dht DHT;
#define DHT11_PIN A0
#define LUXPIN_A  A1

/* Schedule TX every this many seconds (might become longer due to duty cycle limitations).*/
const unsigned TX_INTERVAL = 600;


TinyGPS gps;
// GPS
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;

// The TinyGPS++ object
typedef union {
    float f[2];                
    unsigned char bytes[8];  
} floatArr2Val;
floatArr2Val latlong;



/*
MCM 2018/19 IPG - LoRa teste inicial para envio para TTN
Daniel Mendes, Luis Pimentel
*/
static uint8_t payload[16]; // reserve 16 bytes in memory
/* Para o Segundo UNO
static const PROGMEM u1_t NWKSKEY[16] = { 0x78, 0x81, COLOCAR AQUI };
static const u1_t PROGMEM APPSKEY[16] = { 0xC9, 0xF5, COLOCAR AQUI };
static const u4_t DEVADDR = 0x26011F77; // <-- Change this address for every node!
*/

static const PROGMEM u1_t NWKSKEY[16] = { 0x82, 0x60 };  //COLOCAR AQUI os secrets
static const u1_t PROGMEM APPSKEY[16] = { 0x59, 0x76 }; //COLOCAR AQUI os secrets
static const u4_t DEVADDR = 0x26011874; // <-- Change this address for every node!

unsigned int count = 1;        //For times count

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);


void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;


// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 6, 7},
};



//-------------------------------- SENSORES ---------------------------------------------------------------------------------
void dhtTem()
{
// DHT11 so inteiros 
//.. mas simular para mais tarde colocar DHT22 com float
float tem,hum;
  int chk = DHT.read11(DHT11_PIN);
  if (chk == DHTLIB_OK )
  {
      tem = DHT.temperature*1.0;
      hum = DHT.humidity*1.0;
    int16_t temperatura = 100 * tem;
    int16_t humidade = 100* hum;

    payload[10] = temperatura >> 8;
    payload[11] = temperatura;
    //mesma coisa aqui para proximos 16b
    payload[12] = humidade >> 8;
    payload[13] = humidade;
  }    
}

void light(){
  int16_t lux;
  lux=analogRead(LUXPIN_A);
  delay(100);
  payload[8] = lux >> 8;
  payload[9] = lux;
}

void gpsValue(){
  float latitude;
  float longitude;
//  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  gps.f_get_position(&flat, &flon, &age);
//  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 9);
//  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 9);
  Serial.println();
  smartdelay(2000);

    latitude  = flat;
    longitude = flon;

        if((latitude && longitude) && latitude != latlong.f[0]
            && longitude != latlong.f[1]){     
            latlong.f[0] = latitude;
            latlong.f[1] = longitude;
            
            Serial.print("LatLong: ");
            for(int i = 0; i < 8; i++){
              //  Serial.print(latlong.bytes[i], HEX);
                payload[i] = latlong.bytes[i];
            }
            Serial.println("");
          }
}
//-------------------------------- SENSORES ---------------------------------------------------------------------------------




// ----------------------------- Imprimir CENAS -------------------------------------------------------------------------
void printPayload(){
      Serial.println("----- PAYLOAD ------");
    Serial.print("LatLon: ");
    for(int i = 0; i < 8; i++){
        Serial.print(payload[i], HEX);
    }
    Serial.println("");

    Serial.print("Lux: ");
    for(int i = 8; i < 10; i++){
        Serial.print(payload[i], HEX);
    }
    Serial.println("");

    Serial.print("TemHum: ");
    for(int i = 10; i < 14; i++){
        Serial.print(payload[i], HEX);
    }
    Serial.println("");
    Serial.println("------ PAYLOAD ------ ");
    for(int i = 0; i < 17; i++){
        Serial.print(payload[i], HEX);
    }
    Serial.println("");
    Serial.println("------ PAYLOAD ------ ");
    Serial.println("");
}
// ----------------------------- Imprimir CENAS -------------------------------------------------------------------------









// ----------------------------- ENVIAR -------------------------------------------------------------------------

void do_send(osjob_t* j){
        /*DEBUG PAYLOAD*/
        printPayload();

        gpsValue();        
        light();
        dhtTem();

    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("OP_TXRXPEND, not sending");
    } else {
        // Prepare upstream data transmission at the next possible time.
        //  LMIC_setTxData2(1,datasend,sizeof(datasend)-1,0);

        LMIC_setTxData2(1, payload, sizeof(payload), 0);
        Serial.println("Packet queued");
        Serial.print("LMIC.freq:");
        Serial.println(LMIC.freq);
        Serial.println("Receive data:");
    } 
    // Next TX is scheduled after TX_COMPLETE event.
}


void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(ev);
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print(F("Data Received: "));
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);






void setup()
{
  // initialize both serial ports:
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600); // SoftSerial port to get GPS data. 
  while (!Serial) {
     ;
  };
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    /*LMIC_setClockError(MAX_CLOCK_ERROR * 1/100);
     Set static session parameters. Instead of dynamically establishing a session
     by joining the network, precomputed session parameters are be provided.*/
    #ifdef PROGMEM
    /* On AVR, these values are stored in flash and only copied to RAM
       once. Copy them to a temporary buffer here, LMIC_setSession will
       copy them into a buffer of its own again.*/
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly 
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif
    
    // DAEY single CHANNEL gateway
        #define CHANNEL  1
        for (uint8_t i = 0; i < 9; i++) {
            if (i != CHANNEL) {
                LMIC_disableChannel(i);
            }
        }

    #if defined(CFG_eu868)
    //LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    //LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    #elif defined(CFG_us915)
    // NA-US channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    LMIC_selectSubBand(1);
    #endif
    // Disable link check validation
    LMIC_setLinkCheckMode(0);
    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;
    // Set data rate and transmit power (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,20);
    // Start job
    do_send(&sendjob);
}



static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
    {
      ss.print(Serial.read());
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}







//----------------------------------------------  LOOP ------------------------------------------------------
void loop()
{
  os_runloop_once();
}
