#include "TVModeDetector.h"

#include <HTTPClient.h>
#include "SmartThingsUtils.h"
#include <optional>
#include "StringDict.h"
#include "Config.h"
#include <WString.h>

namespace SA
{
  void TVModeDetector::Update(NetworkClientSecure& client, const char* token)
  { 
    UpdateState(client, token);
  }

  void TVModeDetector::UpdateState(NetworkClientSecure& client, const char* token)
  {
    HTTPClient https;

    Utils::RefreshDevice(client, token, SA::Config::c_TVDeviceID);
    delay(1000);

    bool shouldTVModeBeOn = false;

    {
      char buffer[256];
      snprintf(buffer, sizeof(buffer), "%s/devices/%s/status", Config::c_smartThingsURL, SA::Config::c_TVDeviceID);
      if (!https.begin(client, buffer))
      {
        Serial.printf("[%hs] Unable to connect to status\n", __FUNCTION__);
        return;
      }

      https.addHeader("Authorization", String("Bearer ") + token);
      https.addHeader("Content-Type", "application/json");

      ON_SCOPE_EXIT( https.end(); );

      const int httpCode = https.GET();

      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] GET failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
        return;
      }

      String payload = https.getString();
      auto states = StringDict(payload.c_str(), "/root/components/main/");
      if (auto switchValue = states.GetValue<bool>("switch/switch/value"))
      {
        if (*switchValue)
        {
          if (auto tvChannelName = states.GetValue<const char*>("tvChannel/tvChannelName/value"))
          {
            if (strlen(*tvChannelName) == 0)
            {
              if (auto tvSource = states.GetValue<const char*>("samsungvd.mediaInputSource/inputSource/value"))
              {
                if (Utils::StrCmpIgnoreCase(*tvSource, "HDMI1"))
                {
                  shouldTVModeBeOn = true;
                }
              }
            }
          }
        }
      }
    }

    {
      char urlBuffer[256];
      snprintf(urlBuffer, sizeof(urlBuffer), "%s/devices/%s/commands", Config::c_smartThingsURL, SA::Config::c_outputDeviceID);
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
              "command": "%s"
            }
          ]
        }
        )rl";
        
        char bodyBuffer[512];
        snprintf(bodyBuffer, sizeof(bodyBuffer), body, shouldTVModeBeOn ? "on" : "off");

        const int httpCode = https.POST(bodyBuffer);
        Serial.printf("Turned %s \n", shouldTVModeBeOn ? "ON" : "OFF");

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
  }
}