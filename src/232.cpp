#include "header.h"

bool rs232()
{
  String message = "";
  if (Serial2.available() > 0)
  {

    message = Serial2.readString();
     Serial.print("message:");
     Serial.print(message);
  }



  String uid = "Trailer" + String(UID);
  message_bt = "{\"id\":" +uid +", \"weight\": " +message +"}";
  time_message_weight = millis();
  return true;
}