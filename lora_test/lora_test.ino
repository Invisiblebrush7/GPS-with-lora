// ---- RFID ----
 #include <require_cpp11.h>
#include <MFRC522.h>
#include <deprecated.h>
#include <MFRC522Extended.h>

#define RST_PIN 22 //Pin 22 para el reset del RC522
#define SS_PIN 21 //Pin 21 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

int readRFID(){
  delay(500);
  // Revisamos si hay nuevas tarjetas presentes
  // Look for new cards
  if ( mfrc522.PICC_IsNewCardPresent()) {
    delay(500);
    //Seleccionamos una tarjeta
    if ( mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Present");
      // Enviamos serialemente su UID
      Serial.println("Card UID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      // Terminamos la lectura de la tarjeta actual
      return 1;
      mfrc522.PICC_HaltA();
    }
  } else {
    Serial.println("No card... waiting for one");
    return 0;
  }    
}
// ---- END RFID ----


// ---- LoRA ----
String lora_band = "915000000"; //Banda de frecuencia (Hz) dependerá el País/Región donde se encuentre
String lora_networkid = "18"; //Identificación de la red Lora
String lora_address = "1"; //Dirección del módulo Lora
String lora_RX_address = "2"; //Dirección del módulo receptor Lora
// ---- END LoRA ----

void setup(){
  // ---- RFID ----
  Serial.begin(115200); //Iniciamos la comunicación serial
  SPI.begin(); //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos el MFRC522
  Serial.println("Lectura del UID");
  // ---- END RFID---- 
  
  // ---- LoRA----
  Serial2.begin(115200, SERIAL_8N1, 16,17); //puerto serie2 (pin 16RXD,17TXD UART del esp32) Y (velocidad 115200)
  //pinMode(led,OUTPUT); //Modo salida el pin D22 del Led
  delay(1500);
  Serial2.println("AT+BAND=" + lora_band); //Configuracion del modulo/Puerto serie (UART 2)
  delay(500);
  Serial2.println("AT+ADDRESS=" + lora_address);
  delay(500);
  Serial2.println("AT+NETWORKID=" + lora_networkid);
  // delay(1500);
  // ---- END LoRA----
}
void loop() {
  delay(100);
  int isRfidPresent = readRFID();
  if(isRfidPresent == 1) { //Pulsador OFF
    // digitalWrite(led, HIGH); //led ON
    Serial2.println("AT+SEND="+ lora_RX_address +",2,ON"); //Enviar datos al modulo receptor "ON" 2 bytes
  } else{
    // digitalWrite(led, LOW); //led OFF
  }
}
