#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


/****************************************
 * Define Constants
 ****************************************/
 // ------ GPS------
static const int RXPin = 23, TXPin = -1; // Used for GPS
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
// ------ END GPS ------

// ------ LoRA ------
String lora_band = "915000000"; //Banda de frecuencia (Hz) dependera el País/Región donde se encuentre 
String lora_networkid = "18";   //Identificación de la red Lora
String lora_address = "1";      //Dirección del módulo Lora
String lora_RX_address = "2";   //Dirección del módulo receptor Lora
int led = 22;
// ------ END LoRA ------

/****************************************
 * Auxiliar Functions
 ****************************************/
 // This custom version of delay() ensures that the gps object
// is being "fed".
void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
void setupLora(){
  Serial.println("Setup LoRA");
  Serial2.begin(115200, SERIAL_8N1, 16, 17); //puerto serie2 (pin 16RXD,17TXD UART del esp32)
  Serial2.println("AT+BAND=" + lora_band); ////Configuracion del modulo/Puerto serie (UART 2)
  delay(500);
  Serial2.println("AT+NETWORKID=" + lora_networkid);
  delay(500);
  Serial2.println("AT+ADDRESS=" + lora_address);
  delay(1000);
}

/**
 * MAIN FUNCTIONS
 */
void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  // Setup Lora 
  Serial2.begin(115200,SERIAL_8N1, 16,17); //puerto serie2 (pin 16RXD,17TXD UART del esp32) Y (velocidad 115200)  
  delay(1500);
  Serial2.println("AT+BAND=" + lora_band); //Configuracion del modulo/Puerto serie (UART 2)
  delay(500);
  Serial2.println("AT+ADDRESS=" + lora_address);
  delay(500);
  Serial2.println("AT+NETWORKID=" + lora_networkid);
  delay(1500);
  pinMode(led, OUTPUT); //Modo de salida el pin D22 del led
}

void loop() {
  // Create a JSON document to hold the data
  Serial.println("Reading data");
  DynamicJsonDocument doc(256);
  doc["position"]["lat"] = gps.location.lat();
  doc["position"]["lng"] = gps.location.lng();

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Send the JSON data over Serial2 (assuming Serial2 is connected to your LoRa module)
  String atCommand = "AT+SEND=" + lora_RX_address + "," + String(jsonString.length()) + "," + jsonString;
  digitalWrite(led, !digitalRead(led));
  Serial.println(atCommand);
  Serial2.println(atCommand);

  
  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
  delay(10000);
}
