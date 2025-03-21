#include <Arduino.h>
#include "FS.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"
#include "ArduinoJson.h"
#include "esp32fota.h"


extern const char* ssid;
extern const char* password;
extern esp32FOTA otg;
extern HTTPClient http; 
 
extern const char* manifest_url;


extern BluetoothSerial SerialBT;
#define LED_PIN 2
#define POW 35

extern bool flag_led;
extern uint8_t protocol;
extern String device_name;
extern uint16_t ver;
extern  uint8_t mode;  
extern int val_error_cd;
extern long long timer_0;
extern long long timer_1;
extern long long timer_2;
extern long long timer_3;
extern long long timer_4;
extern long long timer_5;
extern long long time_232;
extern String message_last;
extern long long time_count;
extern float weght;
extern float weght_last;
extern uint8_t count;
extern float weght_last_2;

       
extern uint16_t UID; 
extern String message_bt;
extern long long time_message_weight;      
extern String CHIPID; 








extern void eepromWrite(uint8_t data, uint8_t block, uint8_t page);
extern uint8_t eepromReadSize(uint8_t block, uint8_t page, uint8_t size);
extern uint8_t eepromRead(uint8_t block, uint8_t page);
extern void bt(void);
extern void paser_bt(String str);
extern void send_bt(void);
extern bool eepromIni(void);
extern bool rs232(void);
extern float maps(float x, float in_min, float in_max, float out_min, float out_max);
extern bool eepromIni(void);
extern bool setup_wifi(void);
extern void update(void);
extern bool ini();
extern void my_progress_callback(size_t progress, size_t size);
extern uint16_t crc16(const uint8_t* data, uint8_t length);
