

// #include <wifi_setup.h>
// #include <mqtt_setup.h>
// //#include "WiFi.h"
// #include <certs.h>
// #include <ArduinoLog.h>

// #include <esp_smartconfig.h>
// #include <TaskScheduler.h>
// #include <ArduinoJson.h>
// #include <pump.h>
// #include <flash_setup.h>
// #include <SPI.h>

// #ifdef ALTERNATE_PINS
//   #define VSPI_MISO   2
//   #define VSPI_MOSI   4
//   #define VSPI_SCLK   0
//   #define VSPI_SS     33

//   #define HSPI_MISO   26
//   #define HSPI_MOSI   27
//   #define HSPI_SCLK   25
//   #define HSPI_SS     32
// #else
//   #define VSPI_MISO   MISO
//   #define VSPI_MOSI   MOSI
//   #define VSPI_SCLK   SCK
//   #define VSPI_SS     SS

//   #define HSPI_MISO   12
//   #define HSPI_MOSI   13
//   #define HSPI_SCLK   14
//   #define HSPI_SS     15
// #endif

// #if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// #define VSPI FSPI
// #endif
// int LED_BUILTIN =33;


// static const int spiClk = 1000000; // 1 MHz
// SPIClass * hspi = NULL;
// void setup() {

//      Serial.begin(115200);

//      pinMode(LED_BUILTIN, OUTPUT);
//      //spi_setup();
//      // connectToWiFi();
//      // wifiproviton();
//      //Wifi_setup();
     
//      //
//      // Serial.print(abc);

//      //  action = true;
//      Serial.print("wifi_proviton_end");
//      // while(connect_to_aws == false){
//      // if (wifiActive == true){
//      // connectToAWS();
//      // }

//      // }
//      // connectToAWS();


// }

// void loop() {
    
//   //sendJsonToAWS();
//    //if (wifiActive == true)
//    {

//      digitalWrite(LED_BUILTIN, HIGH);
//      delay(1000);
//      digitalWrite(LED_BUILTIN, LOW);
//      delay(1000);
//      // connectToAWS();
//    }
//   //if(wifiActive == true)
//   {
//     //   Serial.print(F("Humidity: 23.76"));
//     // // Serial.print(h);
//     //   Serial.print(F("%  Temperature: 78.12 "));
//     //   //Serial.print(t);
//     //   Serial.println(F("°C "));
   
//      // publishMessage();  
//   }
//   delay(1000);
// }
 #include <ArduinoLog.h>
 #include <SPI.h>

//#include <ESP32DMASPIMaster.h>
// #include <wifi_setup.h>
// #include <mqtt_setup.h>
// #include "WiFi.h"
// #include <certs.h>
// #include <esp_smartconfig.h>
// #include <TaskScheduler.h>
// #include <ArduinoJson.h>
// #include <pump.h>
// #include <flash_setup.h>
// ESP32DMASPI::Master master;

static const uint32_t BUFFER_SIZE = 20;
static const uint32_t BUFFER_SIZE2 = 1;
uint8_t LEN = 0x01;
uint8_t spi_master_tx_buf[5];
uint8_t spi_master_rx_buf[20];
static uint8_t sendBuf[16];
static uint8_t recvBuf[16];


uint8_t val = 0;
bool connect_server = false;

#define LED_BUILTIN  33
// static const int spiClk = 1000000; // 1 MHz
// SPIClass * hspi = NULL;
// void setup() {

//      Serial.begin(115200);

//      pinMode(LED_BUILTIN, OUTPUT);
//      //spi_setup();
//      // connectToWiFi();
//      // wifiproviton();
//      //Wifi_setup();
     
//      //
//      // Serial.print(abc);

//      //  action = true;
//      Serial.print("wifi_proviton_end");
//      // while(connect_to_aws == false){
//      // if (wifiActive == true){
//      // connectToAWS();
//      // }

//      // }
//      // connectToAWS();


// }




typedef struct __attribute__((packed))
{
   uint8_t len;
   uint8_t opceode;

} DateHeader_t;


typedef struct __attribute__((packed))
{
   uint8_t len;
   uint8_t opceode;
   uint32_t collarID;
   uint8_t power;
   uint8_t cs;
} Data_power_t;
typedef union __attribute__((packed))
{
   DateHeader_t *header;
   Data_power_t *rebettary;

} DatePkt_t;

static const uint8_t askPwr[] =
{
	0x03, 0x02, 0x05
};

static SPISettings spiCfg = SPISettings(1000000, SPI_MSBFIRST, SPI_MODE0);
static uint8_t askNordic(uint8_t* pSend, uint8_t sendsize, uint8_t* pRecv)
{
	uint8_t len;
    uint8_t tmp[16];
    //SPI.beginTransaction(spiCfg);
	digitalWrite(SS, LOW);
	delay(500);
    memcpy(tmp, pSend, sendsize);
    SPI.transfer(tmp, sendsize);
    delay(500);	// give some time for nRF52 to process
    
    digitalWrite(SS, HIGH);
    delay(1000);
    digitalWrite(SS, LOW);
    //digitalWrite(SS, LOW);
    // try to get the length byte
	len = SPI.transfer(0x00);
	if(len == 0xFF)
	{
		// something should be wrong, skip this time.
		digitalWrite(SS, HIGH);
        SPI.endTransaction();
		return 0;
	}

	// let's give it a chance to get again
	if(len == 0x00)
	{
		len = SPI.transfer(0x00);
	}
     
	if(len > 0)
	{
     
		*pRecv = len;
        Serial.print(len);
        delay(500);
        for(uint8_t i = 0; i < len-1; i++)
		{
			*(pRecv + i) = SPI.transfer(0x00);
            Serial.print(*(pRecv + i));
            Serial.print("/");
        }

	}
	digitalWrite(SS, HIGH);
    SPI.endTransaction();
    return len;
}

void spi_inti()
{

	// initialize digital pin LED_BUILTIN as an output.
	//pinMode(LED_BUILTIN, OUTPUT);

	SPI.begin();
    SPI.beginTransaction(spiCfg);
	SPI.setHwCs(false);
	

	Serial.print("SS   : "); Serial.println(SS);
	Serial.print("SCK  : "); Serial.println(SCK);
	Serial.print("MOSI : "); Serial.println(MOSI);
	Serial.print("MISO : "); Serial.println(MISO);

	pinMode(SS, OUTPUT);
	digitalWrite(SS, HIGH);

   
}


void setup() {
    Serial.begin(115200);
    delay(5000);
    // master.setDataMode(SPI_MODE0);           // default: SPI_MODE0
    // master.setFrequency(800000);             // default: 8MHz (too fast for bread board...)
    // master.setInputDelayNs(1000);
    // master.setMaxTransferSize(BUFFER_SIZE);  // default: 4092 bytes
    // master.begin();  // default: HSPI (CS: 15, CLK: 14, MOSI: 13, MISO: 12)
      //spi_setup();
      // connectToWiFi();
      //wifiproviton();
    spi_inti();
    //Wifi_setup();
    // Serial.print(abc);
    //  action = true;
     Serial.print("wifi_proviton_end");
    //  while(connect_to_aws == false)
    //  {
    //  if (wifiActive == true)
    //  }
   

}

void loop() {  
    memset(recvBuf, 0, sizeof(recvBuf));
	uint8_t len = askNordic((uint8_t*)askPwr, askPwr[0], recvBuf);

	Serial.print("Recv: "); Serial.print(len); Serial.println(" bytes.");
	delay(1000);

}

