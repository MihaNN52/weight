#include "header.h"
HTTPClient http;

uint16_t ver = 8;
bool sleep_off = false;// для тестирования батариеи ПОСТАВИТЬ FALSE в INI файле!!!!


bool flag_old = false; //флаг который говорит о том что нет в иги файле на сервере данные о напряжении батареии что запрещает сон
const char *manifest_url = "http://controller-poliva.ru/otg/proton.json";
BluetoothSerial SerialBT;
const char *ssid = "proyagodu";
const char *password = "1234567890";
String device_name = "weigher"; // weigher  до 6 версии - замена резисторов для замера напряжения питания
esp32FOTA otg(device_name, ver);

bool flag_led = false;
long long timer_0 = 0;
long long timer_1 = 0;
long long timer_2 = 0;
long long timer_3 = 0;
long long timer_4 = 0;
long long timer_5 = 0;
long long time_232 = 0;



uint8_t mode = 1;
uint8_t protocol = 3;
uint16_t UID = 0; // Номер 4
String message_bt = "";
String message_last = "";
long long time_message_weight = 0;
String CHIPID = "";
float weght = 0;
float weght_last = 0;
uint8_t count = 0;
long long time_count = 0;
float weght_last_2 = 0;

uint16_t power_low = 0;
uint16_t power_hight = 0;
uint16_t power_low_volt = 0;
uint16_t power_hight_volt = 0;


void setup()
{
   pinMode(LED_PIN, OUTPUT);
   Wire.begin();
   Serial.begin(115200);
   // Serial2.begin(57600, SERIAL_8N1, 16, 17);

   Serial.println("[SETUP] Start");
   Serial2.println("[SETUP] Start2");

   time_232 = millis() + 10000; // время ожидания +10 сек
   eepromIni();
   if (protocol == 1)
   {
      Serial2.begin(9600);
   }
   if (protocol == 2)
   {
      Serial2.begin(57600);
      Serial.println("0000000000000000000000");
   }
   if (protocol == 3)
   {
      Serial2.begin(9600);
   }
   Serial2.println("[SETUP] Start2");

   uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
   // Serial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); // print High 2 bytes
   // Serial.printf("%08X\n", (uint32_t)chipid);                       // print Low 4bytes.
   //  uint64_t id = 0x9C3F9DBF713C;
   //  printf("%llx\n", chipid);
   uint8_t id_6 = (chipid & 0x0000000000FF);
   // printf("%x\n", id_1);
   uint16_t id_5 = (chipid & 0x00000000FF00) >> 8;
   // printf("%x\n", id_2);
   uint16_t id_4 = (chipid & 0x000000ff0000) >> 16;
   // printf("%x\n", id_3);
   uint16_t id_3 = (chipid & 0x0000ff000000) >> 24;
   // printf("%x\n", id_4);
   uint16_t id_2 = (chipid & 0x00ff00000000) >> 32;
   // printf("%x\n", id_5);
   uint16_t id_1 = (chipid & 0xff0000000000) >> 40;
   // printf("%x\n", id_6);
   CHIPID = String(id_6, HEX) + ":" + String(id_5, HEX) + ":" + String(id_4, HEX) + ":" + String(id_3, HEX) + ":" + String(id_2, HEX) + ":" + String(id_1, HEX) + "_";



   float val = 0;
      val = analogRead(POW);
      val = maps(val, power_low, power_hight, power_low_volt/100.0, power_hight_volt/100.0);
      Serial.print("[SETUP] Power volt:");
      Serial.println(val);
      if(val <= power_low_volt/100.0 && !sleep_off && !flag_old ){
         Serial.print("[SETUP] Sleep low power");
         esp_sleep_enable_timer_wakeup(40 * 1000000);
         esp_deep_sleep_start();
      }

   String uid = "WEIGHT_" + CHIPID + String(UID);
   SerialBT.begin(uid);
   Serial.print("[SETUP] BT Name:");
   Serial.println(uid);

   if (val >= (power_low_volt/100.0 + 0.11) || sleep_off)
   {

      otg.setProgressCb(my_progress_callback);
      otg.setManifestURL(manifest_url);
      otg.printConfig();
      if (setup_wifi())
      {  ini();
         update();
         WiFi.mode(WIFI_OFF);
      }

         

   } 
   digitalWrite(LED_PIN, HIGH);
   delay(3000);
   digitalWrite(LED_PIN, LOW);

   /*
      if(protocol == 2) //команда непрерывного вывода веса
      {
         uint8_t cmd_set_mode_10[] = {
            0xF8, 0x55, 0xCE,  // Header
            0x00, 0x02,        // Length = 2
            0x91,              // CMD_TCP_SET_WORK_MODE
            0x0A,              // Mode 10: Industrial indicator mode
            0x00, 0x00         // Placeholder for CRC
        };

        // Вычисляем CRC-16 по первым 7 байтам
        uint16_t crc = crc16(cmd_set_mode_10, 7);
        cmd_set_mode_10[7] = (crc >> 8) & 0xFF;  // CRC High
        cmd_set_mode_10[8] = crc & 0xFF;         // CRC Low
        Serial2.write(cmd_set_mode_10, sizeof(cmd_set_mode_10));
        Serial.print("111111111111111111111111");
      }
        */
}

void loop()
{
   rs232();
   bt();

   delay(50);

   if (millis() - time_232 > 15000)
   {
      if (protocol == 1 || protocol == 2)
      {
         Serial.println("[LOOP] Sleep 40 sec");
         digitalWrite(LED_PIN, HIGH);
         delay(3000);
         digitalWrite(LED_PIN, LOW);
         time_232 = millis();
         esp_sleep_enable_timer_wakeup(40 * 1000000);
         esp_deep_sleep_start();
      }
   }

   if (millis() - timer_1 > 500)
   {
      timer_1 = millis();
   }
   if (millis() - timer_2 > 3000)
   {
      timer_2 = millis();

      if (millis() - time_message_weight < 6000)
      {

         digitalWrite(LED_PIN, HIGH);
         SerialBT.println(message_bt);
         Serial.println("[LOOP] message_bt");
         delay(200);
         digitalWrite(LED_PIN, LOW);
      }
      else
         digitalWrite(LED_PIN, LOW);
   }

   if (millis() - timer_3 > 5000)
   {
      timer_3 = millis();
      Serial.println("[LOOP] ...");

      float val = 0;
      val = analogRead(POW);
      val = maps(val, power_low, power_hight, power_low_volt/100.0, power_hight_volt/100.0);
      if(val <= power_low_volt/100.0 && !sleep_off && !flag_old){
         Serial.print("[POWER] Sleep low power");
         esp_sleep_enable_timer_wakeup(40 * 1000000);
         esp_deep_sleep_start();
      }

      if(val <= 3.9 && !sleep_off){
         Serial.print("[POWER] Sleep low power.");
         esp_sleep_enable_timer_wakeup(40 * 1000000);
         esp_deep_sleep_start();
      }


      if (millis() < 180000)
      {
         float val_1 = 0;
         val_1 = analogRead(POW);
         Serial.print("[POWER] Power acp:");
         Serial.println(val_1);
         val_1 = maps(val_1, power_low, power_hight, power_low_volt/100.0, power_hight_volt/100.0);
         Serial.print("[POWER] Power volt:");
         Serial.println(val_1);

         
         if(power_low_volt/100.0 > 5.6 && power_low_volt/100.0 < 6.0){
            val_1 = maps(val,power_low_volt/100.0, power_hight_volt/100.0, 3.70, 4.2);
            Serial.print("[POWER] Power volt 3.7-4.2 variant:");
            Serial.println(val_1);
         }
      }
   }
}