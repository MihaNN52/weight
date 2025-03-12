#include "header.h"

uint8_t eepromRead(uint8_t block, uint8_t page)
{                                 //чтение одного байта из памяти
    Wire.beginTransmission(0x50); // B1010xxx 0101 000 0x50 типо адресс 0x57
    Wire.write(block);            // MSB block
    Wire.write(page);             // LSB page
    Wire.endTransmission();
    Wire.requestFrom(0x50, 0x1);
    uint8_t eeprom_data_temp = 0;
    if (Wire.available())
    {                                   // пока есть, что читать
        eeprom_data_temp = Wire.read(); // получаем байт (как символ)
    }
    Wire.endTransmission();
    delay(5);
    return eeprom_data_temp;
}

uint8_t eepromReadSize(uint8_t block, uint8_t page, uint8_t size)
{                                 // чтение строки из памяти
    Wire.beginTransmission(0x50); // B1010xxx 0101 000 0x50 типо адрес
    Wire.write(block);            // MSB block
    Wire.write(page);             // LSB page
    Wire.endTransmission();
    Wire.requestFrom(0x50, size);
    uint8_t i = 0;
    uint8_t eeprom_data_temp[size];
    while (Wire.available())
    { // пока есть, что читать

        eeprom_data_temp[i] = Wire.read(); // получаем байт (как символ)
        Serial.print("eeprom_data_temp[i]= ");
        Serial.println(eeprom_data_temp[i]);
        i++;

        if (i >= size)
        {
            break;
        }
    }
    Wire.endTransmission();
    delay(5);
    return *eeprom_data_temp;
    // Serial.print("DATA:");Serial.println(eeprom_data_temp);
}

void eepromWrite(uint8_t data, uint8_t block, uint8_t page)
{
    Wire.beginTransmission(0x50); // B1010xxx 0101 000 0x50 типо адрес
    Wire.write(block);            // MSB block
    Wire.write(page);             // LSB page
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
}

bool eepromIni(){
  
    //eepromWrite(0x55,0, 1);
    //eepromWrite(0x66,0, 2);
    //delay(100);
  
  
    //UID = 0;
    Serial.print("[EEPROM] Version:");Serial.println(ver);
    mode = eepromRead(0, 0);
    Serial.print("[EEPROM] Mode:");Serial.println(mode);
  
    UID = (eepromRead(0, 1) << 8) + eepromRead(0, 2);
    Serial.print("[EEPROM] UID:");Serial.println(UID);
    
    protocol = eepromRead(0, 3);
    Serial.print("[EEPROM] Protocol:");  Serial.println(protocol);
    if(protocol == 255){
  
          protocol = 1;
          eepromWrite(protocol, 0, 15);
          Serial.print(F("[EEPROM] New Protocol:"));
          Serial.println(protocol);
    }
  
  
  return true;
  
  }