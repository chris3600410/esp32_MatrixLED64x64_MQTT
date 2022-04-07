#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "arduino_secrets.h"

// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

///////please enter your sensitive data in the Secret tab/arduino_secrets.h

char ssid[] = SECRET_SSID;                 // your network SSID (name)
char pass[] = SECRET_PASS;                 // your network password (use for WPA, or use as key for WEP)
char mqtt_server[] = SECRET_MQTT_SERVER;   // your MQTT SERVER IP
char mqtt_user[] = SECRET_MQTT_USER;       // your MQTT SERVER USER
char mqtt_pass[] = SECRET_MQTT_PASS;       // your MQTT SERVER PASSWORD
char ota_pass[] = SECRET_OTA_PASS;         // your OTA PASSWORD


#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);


WiFiClient espClient;
PubSubClient client(espClient);

//int spacer = 1;                 // Länge eines Leerzeichens
//int width = 5 + spacer;         // Schriftgröße

String hostname = "INFOdisplay";
String Zeile1 = "...INFO...";
String Zeile2 = "..Display..";
String Zeile3 = "";
String Zeile4 = "";
String Zeile5 = "";
String Zeile6 = "";
String Zeile7 = "";
String Zeile8 = "";

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.setHostname(hostname.c_str()); //define hostname
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("MatrixDisplay");
  ArduinoOTA.setPassword(ota_pass);
  ArduinoOTA.begin();
}

void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  String PayloadString = "";
  for (int i = 0; i < length; i++) { PayloadString = PayloadString + (char)payload[i]; }

  Serial.println("New message arrived");  
  Serial.println(topic);  
  Serial.println(PayloadString);  
  
  if(strcmp(topic, "INFODisplay/zeile1") == 0) {  
    Serial.println("clearScreen and set new Text Zeile1");
    dma_display->clearScreen();      
    Zeile1 = PayloadString;
  }
  if(strcmp(topic, "INFODisplay/zeile2") == 0) {
    Serial.println("clearScreen and set new Text Zeile2");
    dma_display->clearScreen();      
    Zeile2 = PayloadString;
  } 
  if(strcmp(topic, "INFODisplay/zeile3") == 0) {
    Serial.println("clearScreen and set new Text Zeile3");
    dma_display->clearScreen();      
    Zeile3 = PayloadString;
  } 
  if(strcmp(topic, "INFODisplay/zeile4") == 0) {
    Serial.println("clearScreen and set new Text Zeile4");
    dma_display->clearScreen();      
    Zeile4 = PayloadString;
  }   
  if(strcmp(topic, "INFODisplay/zeile5") == 0) {
    Serial.println("clearScreen and set new Text Zeile5");
    dma_display->clearScreen();      
    Zeile5 = PayloadString;
  } 
  if(strcmp(topic, "INFODisplay/zeile6") == 0) {
    Serial.println("clearScreen and set new Text Zeile6");
    dma_display->clearScreen();      
    Zeile6 = PayloadString;
  } 
  if(strcmp(topic, "INFODisplay/zeile7") == 0) {
    Serial.println("clearScreen and set new Text Zeile7");
    dma_display->clearScreen();      
    Zeile7 = PayloadString;
  } 
  if(strcmp(topic, "INFODisplay/zeile8") == 0) {
    Serial.println("clearScreen and set new Text Zeile8");
    dma_display->clearScreen();      
    Zeile8 = PayloadString;
  } 
  
}

void reconnect() {
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pass)) {
      Serial.println("subscribe objects"); 
      client.subscribe("INFODisplay/zeile1");  
      client.subscribe("INFODisplay/zeile2");  
      client.subscribe("INFODisplay/zeile3"); 
      client.subscribe("INFODisplay/zeile4");
      client.subscribe("INFODisplay/zeile5");
      client.subscribe("INFODisplay/zeile6");
      client.subscribe("INFODisplay/zeile7");
      client.subscribe("INFODisplay/zeile8");     
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      delay(5000);
    }
  }
}

void setup() {
  
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); // range is 0-255, 0 - 0%, 255 - 100%
  dma_display->clearScreen();
  //dma_display->fillScreen(myWHITE);
  
  Serial.begin(115200);
  Serial.println("Boot display...");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(MQTTCallback);  
}

void printMatrix() {
 
  //Schreiben Zeile1
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 0);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile1);
  //Schreiben Zeile2
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 8);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile2);
  //Schreiben Zeile3
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 16);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile3); 
  //Schreiben Zeile4
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 24);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile4);
  //Schreiben Zeile5
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 32);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile5);
  //Schreiben Zeile6
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 40);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile6);
  //Schreiben Zeile7
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 48);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile7);
  //Schreiben Zeile8
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 56);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print(Zeile8);
}
void loop() {
  if (!client.connected())  {
    reconnect();
  }
  
  client.loop(); 
  ArduinoOTA.handle();   
  delay(500);
    
  printMatrix();      
}
