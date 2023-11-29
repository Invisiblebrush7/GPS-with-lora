String lora_band = "915000000"; //Banda de frecuencia (Hz) dependera el País/Región donde se encuentre 
String lora_networkid = "18";   //Identificación de la red Lora
String lora_address = "1";      //Dirección del módulo Lora
String lora_RX_address = "2";   //Dirección del módulo receptor Lora

int led = 22; //Led conectado al pin D22 del esp32
int pulsador = 23; //pulsador conectado al pin D23 del esp32
int led2 = 13; //Led conectado al pin D13 del esp32
int pulsador2 = 4; //pulsador conectado al pin D4 del esp32

void setup(){
  
  Serial2.begin(115200,SERIAL_8N1, 16,17); //puerto serie2 (pin 16RXD,17TXD UART del esp32) Y (velocidad 115200)
  pinMode(pulsador,INPUT_PULLUP); //Modo de entrada el pin D23 del pulsador
  pinMode(led,OUTPUT); //Modo salida el pin D22 del Led

  pinMode(pulsador2,INPUT_PULLUP); //Modo de entrada el pin D27 del pulsador
  pinMode(led2,OUTPUT); //Modo salida el pin D34 del Led
  
  delay(1500);
  Serial2.println("AT+BAND=" + lora_band); //Configuracion del modulo/Puerto serie (UART 2)
  delay(500);
  Serial2.println("AT+ADDRESS=" + lora_address);
  delay(500);
  Serial2.println("AT+NETWORKID=" + lora_networkid);
  delay(1500);
}

void loop()
{
  delay(100);

  if(digitalRead(pulsador) == LOW){ //Pulsador OFF
    digitalWrite(led, HIGH); //led1 ON
    Serial2.println("AT+SEND="+ lora_RX_address +",6,LED1ON"); //Enviar datos al modulo receptor "ON" 2 bytes
  }else{
     digitalWrite(led, LOW); //led OFF
  }

  if(digitalRead(pulsador2) == LOW){ //Pulsador OFF
    digitalWrite(led2, HIGH); //led2 ON
    Serial2.println("AT+SEND="+ lora_RX_address +",6,LED2ON"); //Enviar datos al modulo receptor "ON2" 3 bytes
  }else{
     digitalWrite(led2, LOW); //led OFF
  }

}
