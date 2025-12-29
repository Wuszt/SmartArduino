#include "Logger.h"
#include "Config.h"
#include <HTTPClient.h>
#include <WString.h>
#include "Utils.h"
#include "StringDict.h"
#include <NimBleKeyboard.h>

namespace SA
{
  bool ShouldBeTriggered(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    HTTPClient https;

    Utils::RefreshDevice(client, token, deviceID);

    delay(1000);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s/devices/%s/status", Config::c_smartThingsURL, deviceID);
    if (!https.begin(client, buffer))
    {
      Serial.printf("[%hs] Unable to connect to status\n", __FUNCTION__);
      return false;
    }

    https.addHeader("Authorization", String("Bearer ") + token);
    https.addHeader("Content-Type", "application/json");

    ON_SCOPE_EXIT( https.end(); );

    const int httpCode = https.GET();

    if (httpCode != HTTP_CODE_OK)
    {
      Serial.printf("[%hs] GET failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      return false;
    }

    String payload = https.getString();
    auto states = StringDict(payload.c_str(), "/root/components/main/");

    if (auto switchValue = states.GetValue<bool>("switch/switch/value"))
    {
      return *switchValue;
    }

    return false;
  }

  void ReportCompletion(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    HTTPClient https;

    char urlBuffer[256];
    snprintf(urlBuffer, sizeof(urlBuffer), "%s/devices/%s/commands", Config::c_smartThingsURL, deviceID);
    if (https.begin(client, urlBuffer))
    {
      https.addHeader("Authorization", String("Bearer ") + token);
      https.addHeader("Content-Type", "application/json");
      const char* body = R"rl(
      {
        "commands": [
          {
            "component": "main",
            "capability": "switch",
            "command": "off"
          }
        ]
      }
      )rl";
    
      const int httpCode = https.POST(body);

      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] POST failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }

      ON_SCOPE_EXIT( https.end(); );
    }
    else
    {
      Serial.printf("[%hs] Unable to connect to commands\n", __FUNCTION__);
    }
  }

  SmartKeyboard::SmartKeyboard(std::shared_ptr<BleKeyboard> keyboard, const char* triggeringDeviceID)
    : m_keyboard(keyboard)
    , m_triggeringDeviceID(triggeringDeviceID)
  {}

  void SmartKeyboard::Update(NetworkClientSecure& client, const char* token)
  {
    if (ShouldBeTriggered(client, token, m_triggeringDeviceID))
    {
      delay(500);
      if (m_keyboard->isConnected()) 
      {
        OnTriggered();
        ReportCompletion(client, token, m_triggeringDeviceID);
      }
    }
  }
}