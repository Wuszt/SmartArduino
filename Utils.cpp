#include "Utils.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include "Config.h"
#include <Preferences.h>
#include <HTTPClient.h>
#include "base64.h"
#include "StringDict.h"

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
    if (*endptr != '\0') 
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

  String GetToken(NetworkClientSecure& client)
  {
    const char* tokenKey = "token";
    const char* refreshTokenKey = "refreshToken";
    const char* expirationTimeKey = "expirationTime";

    Preferences prefs;
    prefs.begin("SmartArduino");
    ON_SCOPE_EXIT( prefs.end(); );
    const String expirationTimeStr = prefs.getString(expirationTimeKey, "0");
    time_t expirationTime = std::stoll(expirationTimeStr.c_str());
    const time_t currentTime = time(nullptr);
    const time_t timeBeforeRefresh = 60 * 10;

    if (currentTime + timeBeforeRefresh > expirationTime)
    {
      Serial.println("Token has expired, generating new one...");
      const char* url = "https://api.smartthings.com/oauth/token";

      HTTPClient https;
      if (!https.begin(client, url)) 
      {
          Serial.printf("[%hs] Unable to connect to refresh token\n", __FUNCTION__);
          return "";
      }
      ON_SCOPE_EXIT( https.end(); );

      String authBase64;
      {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s:%s", SA::Config::c_clientID, SA::Config::c_clientSecret);
        authBase64 = base64::encode(buffer);
      }

      https.addHeader("Authorization", "Basic " + authBase64);
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");

      char body[256];
      snprintf(body, sizeof(body), "grant_type=refresh_token&client_id=%s&refresh_token=%s", SA::Config::c_clientID, prefs.getString(refreshTokenKey, SA::Config::c_initialRefreshToken).c_str());
      const int httpCode = https.POST(body);
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] POST failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }

      const String result = https.getString();

      const StringDict dict(result.c_str(), "/root/");
      const auto newToken = dict.GetValue<const char*>("access_token");
      ASSERT(newToken);
      Serial.printf("NewToken: %s\n", *newToken);
      prefs.putString(tokenKey, *newToken);

      const auto newRefreshToken = dict.GetValue<const char*>("refresh_token");
      ASSERT(newRefreshToken);
      Serial.printf("NewRefreshToken: %s\n", *newRefreshToken);
      prefs.putString(refreshTokenKey, *newRefreshToken);

      {
        const auto lifetime = dict.GetValue<int>("expires_in");
        ASSERT(lifetime);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%" PRId64, currentTime + *lifetime);
        prefs.putString(expirationTimeKey, buffer);
      }
    }
    else
    {
      const time_t remainingSeconds = expirationTime - (currentTime + timeBeforeRefresh);
      const time_t hours = remainingSeconds / 3600;
      const time_t minutes = remainingSeconds / 60 - hours * 60;
      const time_t seconds = remainingSeconds - hours * 3600 - minutes * 60;
      Serial.printf("Token is valid for the next %" PRId64 "h %" PRId64 "m %" PRId64 "s \n", hours, minutes, seconds);
    }

    return prefs.getString(tokenKey, "");
  }

  void RefreshDevice(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    HTTPClient https;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s/devices/%s/commands", Config::c_smartThingsURL, deviceID);
    if (https.begin(client, buffer))
    {
      ON_SCOPE_EXIT( https.end(); );
      https.addHeader("Authorization", String("Bearer ") + token);
      https.addHeader("Content-Type", "application/json");
      String body = R"rl(
      {
        "commands": [
          {
            "component": "main",
            "capability": "refresh",
            "command": "refresh"
          }
        ]
      }
      )rl";
      const int httpCode = https.POST(body);
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] POST failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
    }
    else
    {
      Serial.printf("[%hs] Unable to connect to commands\n", __FUNCTION__);
    }
  }
}