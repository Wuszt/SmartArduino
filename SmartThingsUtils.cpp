#include "SmartThingsUtils.h"
#include "Utils.h"
#include "Config.h"
#include <Preferences.h>
#include <HTTPClient.h>
#include "base64.h"

namespace SA::Utils
{
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
      //Serial.printf("Token is valid for the next %" PRId64 "h %" PRId64 "m %" PRId64 "s \n", hours, minutes, seconds);
    }

    return prefs.getString(tokenKey, "");
  }

  void RefreshDevice(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    SendCommand(client, token, deviceID, "main", "refresh", "refresh");
  }

  StringDict GetDeviceStatus(NetworkClientSecure& client, const char* token, const char* deviceID, bool forceDataRefresh)
  {
    HTTPClient https;

    if (forceDataRefresh)
    {
      Utils::RefreshDevice(client, token, deviceID); 
      delay(100);
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s/devices/%s/status", Config::c_smartThingsURL, deviceID);
    if (!https.begin(client, buffer))
    {
      Serial.printf("[%hs] Unable to connect to status\n", __FUNCTION__);
      return {};
    }

    https.addHeader("Authorization", String("Bearer ") + token);
    https.addHeader("Content-Type", "application/json");

    ON_SCOPE_EXIT( https.end(); );

    const int httpCode = https.GET();

    if (httpCode != HTTP_CODE_OK)
    {
      Serial.printf("[%hs] GET failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      return {};
    }

    String payload = https.getString();
    return StringDict(payload.c_str(), "/root/components/main/");
  }

  void SendCommand(NetworkClientSecure& client, const char* token, const char* deviceID, const char* component, const char* capability, const char* command)
  {
    HTTPClient https;

    char urlBuffer[256];
    snprintf(urlBuffer, sizeof(urlBuffer), "%s/devices/%s/commands", Config::c_smartThingsURL, deviceID);
    if (https.begin(client, urlBuffer))
    {
      ON_SCOPE_EXIT( https.end(); );
      https.addHeader("Authorization", String("Bearer ") + token);
      https.addHeader("Content-Type", "application/json");
      const char* body = R"rl(
      {
        "commands": [
          {
            "component": "%s",
            "capability": "%s",
            "command": "%s"
          }
        ]
      }
      )rl";
    
      char bodyBuffer[512];
      snprintf(bodyBuffer, sizeof(bodyBuffer), body, component, capability, command);
      const int httpCode = https.POST(bodyBuffer);

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

  void SetSwitchValue(NetworkClientSecure& client, const char* token, const char* deviceID, bool value)
  {
    SendCommand(client, token, deviceID, "main", "switch", value ? "on" : "off");
  }

  bool IsSwitchEnabled(NetworkClientSecure& client, const char* token, const char* deviceID, bool forceDataRefresh)
  {
    if (auto switchValue = GetDeviceValue<bool>(client, token, deviceID, "switch/switch/value", forceDataRefresh))
    {
      return *switchValue;
    }

    return false;
  }
}