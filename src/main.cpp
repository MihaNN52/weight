#include "header.h"

BluetoothSerial SerialBT;

long long timer_0 = 0;
long long timer_1 = 0;
long long timer_2 = 0;
long long timer_3 = 0;
long long timer_4 = 0;
long long timer_5 = 0;

uint16_t ver = 1;
uint8_t mode = 0; //
uint16_t UID = 0; //Номер
String message_bt = "";
long long time_message_weight = 0;

void setup()
{
   pinMode(LED_PIN, OUTPUT);
   Serial.begin(115200);
   Serial2.begin(19200);
   Serial.println("Start");
   delay(1000);

   eepromIni();
   String uid = "Trailer" + String(UID);

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
   if (millis() - timer_2 > 2000)
   {
      timer_2 = millis();
      if(millis() - time_message_weight  < 20000){
         SerialBT.println(message_bt);
      }
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