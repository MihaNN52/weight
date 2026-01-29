#include "header.h"

uint8_t eepromRead(uint8_t block, uint8_t page)
{                                 // чтение одного байта из памяти
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

bool eepromIni()
{

    // eepromWrite(0x55,0, 1);
    // eepromWrite(0x66,0, 2);
    // delay(100);

    // UID = 0;
    Serial.println("");
    Serial.print("[EEPROM] Version:");
    Serial.println(ver);
    mode = eepromRead(0, 0);
    if (mode == 255)
        mode = 1;
    Serial.print("[EEPROM] Mode:");
    Serial.println(mode);

    UID = (eepromRead(0, 1) << 8) + eepromRead(0, 2);
    if (UID == 65535)
        UID = 1;
    Serial.print("[EEPROM] UID:");
    Serial.println(UID);

    protocol = eepromRead(0, 3);
    if (protocol == 255)
        protocol = 3;
    Serial.print("[EEPROM] Protocol:");
    Serial.println(protocol);

    power_low = (eepromRead(0, 4) << 8) + eepromRead(0, 5);
    if (power_low < 300 || power_low > 4100)
        power_low = 1959;
    Serial.print("[EEPROM] Power_LOW:");
    Serial.println(power_low);

    power_hight = (eepromRead(0, 6) << 8) + eepromRead(0, 7);
    if (power_hight < 300 || power_hight > 30000)
        power_hight = 2390;
    Serial.print("[EEPROM] Power_HIGHT:");
    Serial.println(power_hight);

    power_low_volt = (eepromRead(0, 8) << 8) + eepromRead(0, 9);
    if (power_low_volt < 300 || power_low_volt > 26000)
        power_low_volt = 354;
    Serial.print("[EEPROM] Power_LOW VOLT:");
    Serial.println(power_low_volt);

    power_hight_volt = (eepromRead(0, 10) << 8) + eepromRead(0, 11);
    if (power_hight_volt < 300 || power_hight_volt > 26000)
        power_hight_volt = 426;
    Serial.print("[EEPROM] Power_HIGHT VOLT:");
    Serial.println(power_hight_volt);

    uint8_t x_sleep_off = eepromRead(0, 12);
    if(x_sleep_off == 255) {sleep_off =  false;}else{sleep_off = true;}
    Serial.print("[EEPROM] Sleep off:");
    Serial.println(x_sleep_off);

    // если это старая версия без настроек точек напряжения и нет сна при низком напряжении
    if (power_low == 1959 && power_hight == 2390 && power_low_volt == 354 && power_hight_volt == 426)
    {
        flag_old = true;
        //sleep_off = false;
        Serial.println("[EEPROM] Old version < 40");

    }

    return true;
}