#include <WiFi.h>
#include <PubSubClient.h> //Install library "PubSubClient" by Nick O'Leary
#include <ArduinoJson.h>

/****************************************
 * Define Constants
 ****************************************/
// ---- LoRA ----
String lora_band = "915000000"; //Banda de frecuencia (Hz) dependera el País/Región donde se encuentre
String lora_networkid = "18"; //Identificación de la red Lora
String lora_address = "2"; //Dirección del módulo Lora

String textoEntrada; //Almacena los datos recibidos del emisor
// ---- END LoRA ----

// ---- WIFI----
#define WIFISSID "WIFI" // Put your WifiSSID here
#define PASSWORD "WIFI_PASSWORD" // Put your wifi password here
WiFiClient ubidots;
// ---- END WIFI ----

// ---- MQTT----
#define TOKEN "UBIDOTS_TOKEN" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "this-is-my-cool-client-name" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string;
//it should be a random and unique ascii string and different from all other devices
#define VARIABLE_LABEL_latitude "latitude" // Assing the variable label
#define VARIABLE_LABEL_longitude "longitude" // Assing the variable label
#define DEVICE_LABEL "chaleco" // Assig the device label
char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[200];
char topic[150];
// Space to store values to send
char str_latitude[10];
char str_longitude[10];
PubSubClient client(ubidots);
// ---- END MQTT----

/****************************************
 * Auxiliar Functions
 ****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
void setupWifi(){
  WiFi.begin(WIFISSID, PASSWORD);
  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);    
}
void setupLora(){
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
  setupWifi();
  setupLora();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if(Serial2.available()){
    String receivedData = Serial2.readStringUntil('\n');
    Serial.println("Received raw data: " + receivedData);

    int jsonStart = receivedData.indexOf('{');
    int jsonEnd = receivedData.lastIndexOf('}');

    if (jsonStart != -1 && jsonEnd != -1) {
      String jsonPayload = receivedData.substring(jsonStart, jsonEnd + 1);
      Serial.println("Extracted JSON payload: " + jsonPayload);

      // Attempt to parse JSON
      // Allocate a buffer to store the incoming packet
      DynamicJsonDocument doc(256);
      // Read the packet into the JSON document
      DeserializationError error = deserializeJson(doc, jsonPayload);

    // Check if the parsing was successful
    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
    } else {
        // Extract latitude and longitude
        float latitude = doc["position"]["lat"];
        float longitude = doc["position"]["lng"];
  
        // Now you can use latitude and longitude as needed
        if (latitude == 0.0 || longitude == 0.0){
          Serial.println("Latitude: 0, Longitude: 0 -> No data sent");
        } else {
          Serial.print("Latitude: ");
          Serial.println(latitude, 6);
          Serial.print("Longitude: ");
          Serial.println(longitude, 6);
    
          // Prepare payload
          sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
          sprintf(payload, "%s", ""); // Cleans the payload
          sprintf(payload, "{\"position\": {\"value\": 1, \"context\": {"); // Start of JSON structure
          
          // Add latitude
          sprintf(payload, "%s\"lat\": \"%f\",", payload, latitude);
          // Add longitude
          sprintf(payload, "%s\"lng\": \"%f\"", payload, longitude);
        
          // Send payload
          sprintf(payload, "%s}}}", payload); // Close JSON structure
          Serial.println("Publishing data to Ubidots Cloud");
          client.publish(topic, payload);     
        }
     
      }
    }
  }
  delay(1000);
}
