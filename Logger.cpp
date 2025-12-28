#include "Logger.h"
#include "Config.h"
#include <HTTPClient.h>
#include <WString.h>
#include "Utils.h"
#include "StringDict.h"
#include <NimBleKeyboard.h>

namespace SA
{
  bool ShouldBeTriggered(NetworkClientSecure& client, const char* token)
  {
    HTTPClient https;

    Utils::RefreshDevice(client, token, SA::Config::c_pinTriggerDevice);

    delay(1000);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s/devices/%s/status", Config::c_smartThingsURL, SA::Config::c_pinTriggerDevice);
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

  void ReportCompletion(NetworkClientSecure& client, const char* token)
  {
    HTTPClient https;

    char urlBuffer[256];
    snprintf(urlBuffer, sizeof(urlBuffer), "%s/devices/%s/commands", Config::c_smartThingsURL, SA::Config::c_pinTriggerDevice);
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

  Logger::Logger()
  {
    m_keyboard = std::make_unique<BleKeyboard>("Logger", "SmartArduino", 100);
    m_keyboard->begin();
  }

  Logger::~Logger() = default;

  void Logger::Update(NetworkClientSecure& client, const char* token)
  {
    if (ShouldBeTriggered(client, token))
    {
      delay(500);
      if (m_keyboard->isConnected()) 
      {
        {
          m_keyboard->press(KEY_LEFT_CTRL);
          delay(50);
          m_keyboard->press(KEY_LEFT_ALT);
          delay(50);
          m_keyboard->press(KEY_DELETE);
          delay(100);
          m_keyboard->releaseAll();
        }

        delay(500);
        m_keyboard->write(KEY_RETURN);

        delay(500);
        for (const char* c = Config::c_pin; *c != '\0'; ++c)
        {
          delay(50);
          m_keyboard->write(*c);
        }

        ReportCompletion(client, token);
        Serial.println("Pin inserted!");
      }
    }
  }
}