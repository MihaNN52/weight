#include "header.h"

void bt()
{
    /*
        if (SerialBT.available()) {
        Serial.write(SerialBT.read());
      }
      */

    if (SerialBT.available() > 0)
    {
        String input_str_bt = SerialBT.readString();
        Serial.println(input_str_bt);
        paser_bt(input_str_bt);
    }
}

void paser_bt(String str)
{
    //DynamicJsonBuffer jsonBuffer(200);
    //JsonObject &root = jsonBuffer.parseObject(str);
    StaticJsonDocument<1100> root;
    DeserializationError err = deserializeJson(root, str);

    if (err)
    {
        Serial.print("Json ошибка синтаксиса, входное сообщение:");
        Serial.println(str);
        return;
    }
    //с этого момента считаем сообщение целым
    //{pump:1}
    //{"temp": 60, "hui": 20}

   

    if (root["mode_eeprom"])
    {

        uint16_t x = root["mode"];
        Serial.print("Mode eeprom:");
        Serial.println(x);
        eepromWrite(x, 0, 0);
    }
    if (root["mode"])
    {

        uint16_t x = root["mode"];
        Serial.print("Mode:");
        Serial.println(x);
        mode = x;
      
    }

    if (root["uid"])
    {

        uint16_t x = root["uid"];
        Serial.print("UID:");
        Serial.println(x);
        eepromWrite(x >> 8, 0, 1);
        eepromWrite(x, 0, 2);

        
    }

        if (root["weight_last"])
    {

         digitalWrite(LED_PIN,HIGH);
         SerialBT.println(message_bt);
         delay(200);
         digitalWrite(LED_PIN,LOW);

        
    }

  

    

    //root.printTo(str);
    String output;
    serializeJson(root, output);
}


/*
bool eepromIni(){
  
  //eepromWrite(0x55,0, 1);
  //eepromWrite(0x66,0, 2);
  //delay(100);


  //UID = 0;
  
  mode = eepromRead(0, 0);
  Serial.print("Mode:");Serial.println(mode);

  UID = (eepromRead(0, 1) << 8) + eepromRead(0, 2);
  Serial.print("UID:");Serial.println(UID);
  return true;
}
  */