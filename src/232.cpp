#include "header.h"

float maps(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t crc16(const uint8_t* data, uint8_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= ((uint16_t)data[i]) << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x8005;
            else
                crc <<= 1;
        }
    }
    return crc;
}

bool rs232()
{

    if (mode == 2)
    { // разбор пакета для отладки
        String message = "";
        uint8_t inChar;
        char buf[100];
        uint8_t buf_len = 0;
        while (Serial2.available() > 0)
        {

            inChar = Serial2.read();
            // Serial.print(inChar, HEX);
            // Serial.print(" ");

            if (buf_len)
            {
                buf[buf_len] = inChar;
                buf_len++;
            }
            if (!buf_len && inChar)
            {
                buf[buf_len] = inChar;
                buf_len++;
            }

            if (buf_len > 90)
                break;
            // delay(1);
            delayMicroseconds(800);
        }
        if (!buf_len)
            return false;

        if (millis() - time_message_weight < 3000)
        {
            return false;
        }
        // Формируем строку из принятых байт в HEX формате
        for (uint8_t i = 0; i < buf_len; i++)
        {
            message += "0x";
            if ((uint8_t)buf[i] < 0x10)
                message += "0"; // Добавляем ведущий ноль для единичных цифр
            message += String((uint8_t)buf[i], HEX);

            if (i < buf_len - 1)
            {
                message += ", ";
            }
        }

        String uid = "weight_" + String(UID);
        message_bt = "{\"id\":\"" + CHIPID + uid + "\", \"weight\": " + message + "}";
        time_message_weight = millis();
        Serial.print("[232] message_bt:");
        Serial.println(message_bt);
    }
    if (mode == 1 && protocol == 1) // разбор весов
    {
        String message = "";
        uint8_t inChar = 0;
        ;
        char buf[60] = {0};
        uint8_t buf_len = 0;

        while (Serial2.available() > 0)
        {

            inChar = Serial2.read();
            // Serial.print(inChar, HEX);
            // Serial.print(" ");

            if (buf_len)
            {
                buf[buf_len] = inChar;
                buf_len++;
            }
            if (!buf_len && inChar && (inChar == 0x01 || inChar == 0x20 || inChar == 0x2D))
            {
                buf[buf_len] = inChar;
                buf_len++;
            }

            if (buf_len > 25)
                break;
            // delay(1);
            delayMicroseconds(800);
        }
        if (!buf_len)
            return false;
        /*
                Serial.print("[232]PASKET IN: ");
                for (uint8_t i = 0; i <= buf_len; i++)
                {
                    Serial.print(buf[i], HEX);
                    Serial.print(" ");
                }
                Serial.println(" ");
        */
        // Serial.print("buf_len:");
        // Serial.println(buf_len);
        // протокол первых весов
        if (buf[0] == 0x01 && buf[1] == 0x02 && buf[2] == 0x53 && buf[3] == 0x20)
        {
            if (buf[6] == 0x2e && buf[4] >= 0x30 && buf[4] <= 0x39 && buf[5] >= 0x30 && buf[5] <= 0x39 && buf[7] >= 0x30 && buf[7] <= 0x39 && buf[8] >= 0x30 && buf[8] <= 0x39 && buf[9] >= 0x30 && buf[9] <= 0x39)
            {

                if (buf[4] == 0x30 && buf[5] == 0x30 && buf[7] == 0x30 && buf[8] == 0x30 && buf[9] == 0x30)
                {
                    return false;
                }

                message += buf[4];
                message += buf[5];
                message += buf[6];
                message += buf[7];
                message += buf[8];
                message += buf[9];

                float val = 0;
                val = analogRead(POW);
                // 2390 = 4.261
                // 1397 =2.608
                // 2000 =3.627
                Serial.print("[232] Power:");
                Serial.println(val);
                val = maps(val, 1959.0, 2390.0, 3.542, 4.261);

                String uid = "weight_" + String(UID);
                message_bt = "{\"id\":\"" + CHIPID + uid + "\", \"weight\": " + message + ", \"power\": " + val + "}";
                time_message_weight = millis();
                Serial.print("[232] message_bt1:");
                Serial.println(message_bt);

                return true;
            }
        }
        // протокол первых весов CAS 60
        if ((buf[0] == 0x20 || buf[0] == 0x2D) && buf[1] == 0x20 && buf[10] == 0x0D && buf[11] == 0x0A)
        {
            if (millis() - time_message_weight < 3000)
            {
                return false;
            }
            message += buf[2];
            message += buf[3];
            message += buf[4];
            message += buf[5];
            message += buf[6];
            message += buf[7];
            weght = message.toFloat();
            if (buf[0] == 0x2D)
                weght = weght * -1;

            if (weght <= 0)
            {
                // weght = 0.0;
            }

            if (weght >= weght_last_2 && weght - weght_last_2 < 0.015)
            { // message_last == message

                count++;
                time_count = millis();
            }
            if (weght_last_2 >= weght && weght_last_2 - weght < 0.015)
            { // message_last == message

                count++;
                time_count = millis();
            }

            weght_last_2 = weght;
            if (millis() - time_count > 300)
                count = 0;

            if (count < 10)
                return 0;
            else
                count = 0;

            if (buf[0] == 0x2D)
            {
                message = "0";
            }
            if (weght == weght_last && millis() - time_message_weight < 20000)
                return 0;

            /*
                    if(weght >= weght_last && weght - weght_last < 0.015){//message_last == message
                        weght = weght_last;
                        return false;
                    }
                    if(weght_last >= weght  &&  weght_last - weght < 0.015){//message_last == message
                         weght = weght_last;
                        return false;
                    }
            */

            float val = 0;
            val = analogRead(POW);
            // 2390 = 4.261
            // 1397 =2.608
            // 2000 =3.627
            Serial.print("[232] Power:");
            Serial.println(val);
            val = maps(val, 1959.0, 2390.0, 3.542, 4.261);

            String uid = "weight_" + String(UID);
            message_bt = "{\"id\":\"" + CHIPID + uid + "\", \"weight\": " + String(weght, 3) + ", \"power\": " + val + "}";
            time_message_weight = millis();
            Serial.print("[232] message_bt2:");
            Serial.println(message_bt);
            message_last = message;
            weght_last = weght;
            return true;
        }
    }
    if (mode == 1 && protocol == 2)// весы Масса К
    { 
        String message = "";
        uint8_t inChar = 0;
        
        uint8_t buf[60] = {0};
        uint8_t buf_len = 0;

        while (Serial2.available() > 0)
        {

            inChar = Serial2.read();
            // Serial.print(inChar, HEX);
            // Serial.print(" ");

            if (buf_len)
            {
                buf[buf_len] = inChar;
                buf_len++;
            }
            if (!buf_len && inChar && inChar == 0xF8)
            {
                buf[buf_len] = inChar;
                buf_len++;
            }

            if (buf_len > 25)
                break;
            // delay(1);
            delayMicroseconds(800);
        }
        if (buf[11] == 0)
            return false;
        if (!buf_len)
            return false;

        if (millis() - time_message_weight < 3000)
        {
            return false;
        }
        if (buf[0] == 0xF8 && buf[1] == 0x55 && buf[2] == 0xCE && buf[5] == 0x4C)
        { // 0x4C — CMD_TCP_ACK_WEIGHT

            uint16_t calculated_crc = crc16(buf, 12); // 12 байт: с 0 по 11
            uint16_t received_crc = ((uint16_t)buf[12] << 8) | buf[13];

            if (calculated_crc != received_crc)
            {
                Serial.println("CRC ошибка!");
                return false;
            }
            // Вес (signed long), big-endian
            int32_t weight = 0;
            weight |= ((int32_t)buf[6]) << 24;
            weight |= ((int32_t)buf[7]) << 16;
            weight |= ((int32_t)buf[8]) << 8;
            weight |= ((int32_t)buf[9]);

            uint8_t division_code = buf[10];
            int division_step;

            switch (division_code)
            {
            case 0:
                division_step = 0.1 * 10;
                break; // 0.1 г, умножим на 10 для целочисленной точности
            case 1:
                division_step = 1;
                break;
            case 2:
                division_step = 10;
                break;
            case 3:
                division_step = 100;
                break;
            case 4:
                division_step = 1000;
                break;
            default:
                division_step = 1;
                break;
            }

            // Округляем вес
            int32_t rounded_weight = (weight + division_step / 2) / division_step * division_step;
            weght = rounded_weight / 1000.0;
            printf("Вес: %d г (округлён до деления %d г): %d г\n", weight, division_step, rounded_weight);

            float val = 0;
            val = analogRead(POW);
            // 2390 = 4.261
            // 1397 =2.608
            // 2000 =3.627
            Serial.print("[232] Power:");
            Serial.println(val);
            val = maps(val, 1959.0, 2390.0, 3.542, 4.261);

            String uid = "weight_" + String(UID);
            message_bt = "{\"id\":\"" + CHIPID + uid + "\", \"weight\": " + String(weght, 3) + ", \"power\": " + val + "}";
            time_message_weight = millis();
            Serial.print("[232] message_bt2:");
            Serial.println(message_bt);
            message_last = message;
            weght_last = weght;
        }
    }
    return false;
}

