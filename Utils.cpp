#include "Utils.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include "Config.h"

namespace SA::Utils
{
  void InitializeSerial()
  {
    Serial.begin(115200);
    while (!Serial) {}
  }

  WiFiMulti InitializeWiFi()
  {
      WiFiMulti WiFiMulti;
      WiFi.mode(WIFI_STA);
      WiFiMulti.addAP(SA::Config::c_wifiNetworkName, SA::Config::c_wifiPassword);

      Serial.print("Waiting for WiFi to connect...");
      while ((WiFiMulti.run() != WL_CONNECTED)) 
      {
        Serial.print(".");
      }
      Serial.println(" connected");

      return WiFiMulti;
  }

  void InitializeClock()
  {
      configTime(0, 0, "pool.ntp.org");

      Serial.print(F("Waiting for NTP time sync: "));
      time_t nowSecs = time(nullptr);
      while (nowSecs < 8 * 3600 * 2) 
      {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
      }

      Serial.println();
      struct tm timeinfo;
      gmtime_r(&nowSecs, &timeinfo);
      Serial.print(F("Current time: "));
      char buf[26];
      Serial.print(asctime_r(&timeinfo, buf));
  }

  bool StrCmpIgnoreCase(const char* l, const char* r)
  {
    for(int offset = 0;; ++offset)
    {
      const char lChar = *(l + offset);
      const char rChar = *(r + offset);
      if (std::tolower(lChar) != std::tolower(rChar))
      {
        return false;
      }

      if (lChar == '\0')
      {
        return true;
      }
    }

    return false;
  }

  template<>
  std::optional<const char*> ParseValue<const char*>(const char* str)
  {
    return str;
  }

  template<>
  std::optional<int> ParseValue<int>(const char* str)
  {
    char *endptr;
    int result = strtol(str, &endptr, 10);
    if (endptr == str) 
    {
        return {};
    }
    return result;
  }

  template<>
  std::optional<bool> ParseValue<bool>(const char* str)
  {
    if (const std::optional<int> asInt = ParseValue<int>(str))
    {
      return *asInt > 0;
    }
    else if (const std::optional<const char*> asStr = ParseValue<const char*>(str))
    {
      if (SA::Utils::StrCmpIgnoreCase(*asStr, "true"))
      {
        return true;
      }
      else if (SA::Utils::StrCmpIgnoreCase(*asStr, "false"))
      {
        return false;
      }
      else if (SA::Utils::StrCmpIgnoreCase(*asStr, "on"))
      {
        return true;
      }
      else if (SA::Utils::StrCmpIgnoreCase(*asStr, "off"))
      {
        return false;
      }
    }
    
    return {};
  }

  template<>
  std::optional<float> ParseValue<float>(const char* str)
  {
    char *endptr;
    float result = strtof(str, &endptr);
    if (endptr == str) 
    {
        return {};
    }
    return result;
  }
}