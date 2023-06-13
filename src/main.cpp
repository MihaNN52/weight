#include "header.h"

BluetoothSerial SerialBT;

long long timer_0 = 0;
long long timer_1 = 0;
long long timer_2 = 0;
long long timer_3 = 0;
long long timer_4 = 0;
long long timer_5 = 0;
long long time_232 = 0;
uint16_t ver = 1;
uint8_t mode = 0; //1 самые первые весы
uint16_t UID = 0; //Номер
String message_bt = "";
String message_last = "";
long long time_message_weight = 0;
String CHIPID = "";
float weght = 0;
float weght_last = 0;

void setup()
{
   pinMode(LED_PIN, OUTPUT);
   Wire.begin();
   Serial.begin(115200);
   Serial2.begin(9600);
   Serial.println("Start");
   Serial2.println("Start2");
   delay(1000);

  eepromIni();
    uint64_t chipid = ESP.getEfuseMac();                             // The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); // print High 2 bytes
  Serial.printf("%08X\n", (uint32_t)chipid);                       // print Low 4bytes.
  // uint64_t id = 0x9C3F9DBF713C;
  // printf("%llx\n", chipid);
  uint8_t id_1 = (chipid & 0x0000000000FF);
  // printf("%x\n", id_1);
  uint16_t id_2 = (chipid & 0x00000000FF00) >> 8;
  // printf("%x\n", id_2);
  uint16_t id_3 = (chipid & 0x000000ff0000) >> 16;
  // printf("%x\n", id_3);
  uint16_t id_4 = (chipid & 0x0000ff000000) >> 24;
  // printf("%x\n", id_4);
  uint16_t id_5 = (chipid & 0x00ff00000000) >> 32;
  // printf("%x\n", id_5);
  uint16_t id_6 = (chipid & 0xff0000000000) >> 40;
  // printf("%x\n", id_6);
  CHIPID = String(id_6, HEX) + ":" + String(id_5, HEX) + ":" + String(id_4, HEX) + ":" + String(id_3, HEX) + ":" + String(id_2, HEX) + ":" + String(id_1, HEX) + "_";

  String uid = "WEIGHT_" + CHIPID + String(UID);
  SerialBT.begin(uid);

}

void loop()
{
   rs232();
   bt();
   
   delay(50);

   if (millis() - timer_1 > 500)
   {
      timer_1 = millis();
   }
   if (millis() - timer_2 > 3000)
   {
      timer_2 = millis();
      if(millis() - time_message_weight  < 9000){
         digitalWrite(LED_PIN,HIGH);
         SerialBT.println(message_bt);
         Serial.println("message_bt");
         delay(200);
         digitalWrite(LED_PIN,LOW);
      } else
      digitalWrite(LED_PIN,HIGH);
   }

   if (millis() - timer_3 > 10000)
   {
      timer_3 = millis();
   }
   if (millis() - timer_4 > 60000)
   {
      timer_4 = millis();
   }
}