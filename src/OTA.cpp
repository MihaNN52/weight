#include "header.h"

bool setup_wifi()
{
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("[WiFi] Connecting to WiFi");
  WiFi.begin(ssid, password); // no WiFi creds in this demo :-)
  uint8_t val = 20;
  while (WiFi.status() != WL_CONNECTED && val--)
  {
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("[WiFi] IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("[WiFi] Hostname: ");
    Serial.println(WiFi.getHostname());

    Serial.print("[WiFi] ESP Mac Address: ");
    Serial.println(WiFi.macAddress());

    Serial.print("[WiFi] Subnet Mask: ");
    Serial.println(WiFi.subnetMask());

    Serial.print("[WiFi] Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[WiFi] DNS: ");
    Serial.println(WiFi.dnsIP());

    digitalWrite(LED_PIN,HIGH);
    delay(1000);
    digitalWrite(LED_PIN,LOW);
    delay(1000);
    digitalWrite(LED_PIN,HIGH);
    delay(1000);
    digitalWrite(LED_PIN,LOW);
    delay(1000);
    digitalWrite(LED_PIN,HIGH);
    delay(1000);
    digitalWrite(LED_PIN,LOW);
    return true;
  }
  Serial.println(" ");
  Serial.println("[WiFi] No Internet");
  return false;
}

void my_progress_callback(size_t progress, size_t size)
{
  if (progress == size || progress == 0)
    Serial.println();
  Serial.print(".");
  digitalWrite(LED_PIN,flag_led);
  flag_led = !flag_led;
}

void update()
{
  ini();
  bool updatedNeeded = otg.execHTTPcheck();

  if (updatedNeeded)
  {

    String str = "";
    // str = "https://api.telegram.org/bot6424919932:AAEXhZSKrz8ek-opsIsx_tjI0VOLqdcpLlI/sendMessage?chat_id=-647328575&text=Ha-ha-ha";
    str = "http://controller-poliva.ru/bot.php?key=54E25F63-7532-4F7F-A7F1-CAFB402B2EB0&msg=";
    str += "Начинаем%20обновление%20устройства%20UID:" + String(UID) + ",%20" + device_name + "%20MAC%20";
    str += WiFi.macAddress() + "%20C%20версии%20" + String(ver);
    http.begin(str);
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.POST("");
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();

    otg.execOTA();
    Serial.println("[Update]  ok");

    WiFi.disconnect();
  }
  Serial.println("[Update]  not");
  WiFi.disconnect();
  delay(2000);
}

bool ini()
{

  // Объявить объект класса HttpClient
  String str = "http://controller-poliva.ru/ini/";
  str += WiFi.macAddress();
  str += ".json";
  http.begin(str);                              // Укажите адрес запроса
  http.addHeader("Content-Type", "text/plain"); // Укажите заголовок типа содержимого

  int httpCode = http.POST("");      // Отправьте запрос
  String payload = http.getString(); // Получите полезную нагрузку ответа

  Serial.println(httpCode); // Распечатать код возврата HTTP
  http.end();
  if (httpCode == 200)
  {
    Serial.println(payload); // Полезная нагрузка для ответа на запрос печати

    StaticJsonDocument<1100> doc;
    DeserializationError err = deserializeJson(doc, payload);
    /*
    switch (err.code()) {
    case DeserializationError::Ok:
        Serial.println(F("Deserialization succeeded"));
        break;
    case DeserializationError::InvalidInput:
        Serial.println(F("Invalid input!"));
        break;
    case DeserializationError::NoMemory:
        Serial.println(F("Not enough memory"));
        break;
    default:
        Serial.println(F("Deserialization failed"));
        break;

}
*/
    if (!doc["mode"] || !doc["uid"] || !doc["protocol"] || !doc["type"])
    {
      Serial.print("[INI] UJson ошибка синтаксиса, входное сообщение:");
      Serial.println(payload);
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.f_str());

      str = "";
      // str = "https://api.telegram.org/bot6424919932:AAEXhZSKrz8ek-opsIsx_tjI0VOLqdcpLlI/sendMessage?chat_id=-647328575&text=Ha-ha-ha";
      str = "http://controller-poliva.ru/bot.php?key=54E25F63-7532-4F7F-A7F1-CAFB402B2EB0&msg=";
      str += "Ошибка%20в%20файле%20настроек%20UID:" + String(UID) + ",%20" + device_name + "%20MAC%20";
      str += WiFi.macAddress();
      http.begin(str);
      http.addHeader("Content-Type", "text/plain"); // Укажите заголовок типа содержимого
      int httpCode = http.POST("");                 // Отправьте запрос
      payload = http.getString();                   // Получите полезную нагрузку ответа
      Serial.println(httpCode);
      Serial.println(payload);
      http.end();

      return false;
    }
    bool restart = false;
    if (doc["mode"])
    {
      uint8_t x = doc["mode"];
      eepromWrite(x, 0, 0);
      Serial.printf("[INI] Mode = %d\n", x);
      if(x != mode)restart = true;
    }

    if (doc["uid"])
    {
      uint16_t x = doc["uid"];
      eepromWrite(x >> 8, 0, 1);
      eepromWrite(x, 0, 2);
      Serial.printf("[INI] UID = %d\n", x);
      if(x != UID)restart = true;
    }

    if (doc["protocol"])
    {
      uint16_t x = doc["protocol"];
      eepromWrite(x, 0, 3);
      Serial.printf("[INI] protocol = %d\n", x);
      if(x != protocol)restart = true;
    }

    if (doc["type"])
    {
      String type = doc["type"];
      if (device_name != type)
      {
        // надо делать обновление прошивки на другой тип
        esp32FOTA otg(type, ver);
        otg.setManifestURL(manifest_url);
        otg.printConfig();
        if (setup_wifi())
        {
          update();
        }
      }
    }
    if (restart)
      ESP.restart();
  }
  else
  {
    Serial.println("[INI] Device not found server");
    http.begin("http://controller-poliva.ru/input.php");
    http.addHeader("token", "update");
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<900> doc;

    doc["mac"] = WiFi.macAddress(); //WiFi.macAddress();
    doc["type"] = device_name;
    doc["protocol"] = protocol;
    doc["mode"] = mode;
    doc["uid"] = UID;
    doc["ver"] = ver;

    str = "";
    serializeJson(doc, str);

    int httpResponseCode = http.POST(str);

    // Проверка ответа сервера
    if (httpResponseCode > 0)
    {
      Serial.printf("[INI] HTTP Response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println(response);
    }
    else
    {
      Serial.printf("[INI] Error sending POST request: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  http.end(); // Закрыть соединение
  return true;
}