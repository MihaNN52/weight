#include "header.h"

bool rs232()
{
    String message = "";
    uint8_t inChar;
    char buf[30];
    uint8_t buf_len = 0;
    while (Serial2.available() > 0)
    {

        inChar = Serial2.read();
        Serial.print(inChar, HEX);
        Serial.print(" ");

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

        if (buf_len > 25)
            break;
    }
    if (!buf_len)
        return false;

    Serial.print("buf_len:");
    Serial.println(buf_len);
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

            int val = 0;
            val = analogRead(POW);

            String uid = "weight_" + String(UID);
            message_bt = "{\"id\":\"" + CHIPID + uid + "\", \"weight\": " + message +", \"power\": " + val + "}";
            time_message_weight = millis();
            Serial.print("message_bt");
            Serial.println(message_bt);
            return true;
        }
    }

    return false;
}