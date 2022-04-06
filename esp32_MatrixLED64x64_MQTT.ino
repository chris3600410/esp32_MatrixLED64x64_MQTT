//#include <SPI.h>
//#include <Adafruit_GFX.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

//int helligkeit = 90;                           // Helligkeit des DisplaysDefault Helligkeit 0 bis 255
//int numberOfHorizontalDisplays = 12;     //Anzahl der Module Horizontal
//int numberOfVerticalDisplays = 1;        //Anzahl der Module Vertikal

const char* ssid = "SSID";          //  your network SSID (name)
const char* password  = "PASSWORT";   // your network password
const char* mqtt_server = "192.168.178.110";
const char* mqttUser = "loxberry";
const char* mqttPassword = "PASSWORD";

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

//int ScrollWait = 70;                  // Zeit in ms für Scroll Geschwindigkeit
//int spacer = 1;                 // Länge eines Leerzeichens
//int width = 5 + spacer;         // Schriftgröße

String hostname = "Matrixdisplay";
//String Zeile1 = "...";
//String Zeile2 = "...";
uint32_t zeile1 = 0;
uint32_t zeile2 = 0;
uint32_t zeile3 = 0;
uint32_t zeile4 = 0;
uint32_t zeile5 = 0;
uint32_t zeile6 = 0;
uint32_t zeile7 = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.setHostname(hostname.c_str()); //define hostname
  //wifi_station_set_hostname( hostname.c_str() );
  WiFi.begin(ssid, password);

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
  ArduinoOTA.setPassword("PASSWORD");
  ArduinoOTA.begin();
}

void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  String sTopic = String(topic);
  
  String PayloadString = "";
  for (int i = 0; i < length; i++) { PayloadString = PayloadString + (char)payload[i]; }

  Serial.println("New message arrived");  
  Serial.println(topic);  
  Serial.println(PayloadString);


// Workaround to get int from payload
  payload[length] = '\0';
  uint32_t value = String((char*)payload).toInt();

  if (sTopic == "MatrixDisplay/zeile1") {
    zeile1 = value;
  } else if (sTopic == "MatrixDisplay/zeile2") {
    zeile2 = value;
  } else if (sTopic == "MatrixDisplay/zeile3") {
    zeile3 = value;
  } else if (sTopic == "MatrixDisplay/zeile4") {
    zeile4 = value;
  } else if (sTopic == "MatrixDisplay/zeile5") {
    zeile5 = value;
  } else if (sTopic == "MatrixDisplay/zeile6") {
    zeile6 = value;
  } else if (sTopic == "MatrixDisplay/zeile7") {
    zeile7 = value;
  }

  //valueChanged = true;



    
  
  //if(strcmp(topic, "MatrixDisplay/zeile1") == 0) {  
   // Serial.println("set new Text Zeile1");
   // dma_display->clearScreen();      
   // Zeile1 = PayloadString;
   // dma_display->setTextWrap(false);
   // dma_display->setCursor(0, 0);
  //dma_display->setTextColor(dma_display->color444(0,15,15));
    //dma_display->print(Zeile1);
    //(strcmp(topic, "MatrixDisplay/zeile2") == 0);  
    //Serial.println("set new Text Zeile2");
    //dma_display->clearScreen();      
   // Zeile2 = PayloadString;
   // dma_display->setTextWrap(false);
    //dma_display->setCursor(0, 8);
  //dma_display->setTextColor(dma_display->color444(0,15,15));
    //dma_display->print(Zeile2);
  } 
 
  //if(strcmp(topic, "MatrixDisplay2/intensity") == 0) { 
    //Serial.println("set new Intensity");      
    //helligkeit = PayloadString.toInt();
    //matrix.setIntensity(helligkeit);
  //}  
  
//  if(strcmp(topic, "MatrixDisplay2/scrollwait") == 0) { 
    //Serial.println("set new ScrollWait");      
    //ScrollWait = PayloadString.toInt();
//  }  
//}

void reconnect() {
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(),mqttUser, mqttPassword)) {
      //if (client.connect(clientId.c_str(),"mqttUser","mqttPassword")) {
      Serial.println("subscribe objects"); 
      client.subscribe("MatrixDisplay/zeile1");  
      client.subscribe("MatrixDisplay/zeile2");  
      client.subscribe("MatrixDisplay/zeile3");      
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
  

  
  //delay(1000);

//  matrix.setIntensity(helligkeit);
  //for (int matrixIndex=0 ; matrixIndex < numberOfHorizontalDisplays ; matrixIndex++ )
  //{
   // matrix.setRotation(matrixIndex, 1);        //Erste DOT Matrix Drehen 
  //}

  Serial.begin(115200);
  Serial.println("Boot display...");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(MQTTCallback);  
}

void printMatrix() {

  dma_display->setTextColor(dma_display->color444(15,15,15));
      
  //Schreiben Zeile1
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 0);
  dma_display->setTextColor(dma_display->color444(0,15,15));
  dma_display->print((Zeile1));
  //Schreiben Zeile2
  dma_display->setTextWrap(false);
  dma_display->setCursor(0, 8);
  dma_display->setTextColor(dma_display->color444(0,15,15));
  dma_display->print("Zeile2");
  
      //client.loop(); 
    //}

    //matrix.write();
    //delay(ScrollWait);
  //}
//}

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
