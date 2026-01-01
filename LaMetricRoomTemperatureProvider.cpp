#include "LaMetricRoomTemperatureProvider.h"
#include "SmartThingsUtils.h"
#include "Config.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "base64.h"
#include <NetworkClient.h>
#include <span>

namespace SA
{
  struct Frame
  {
    const char* m_text = nullptr;
    int iconID = 0;
  };

  void SetFrames(std::span<const Frame> frames, const char* widgetID)
  {
    NetworkClient laMetricClient;
    HTTPClient https;

    char urlBuffer[256];
    snprintf(urlBuffer, sizeof(urlBuffer), "%s/%s", Config::c_laMetricURL, widgetID);
    if (https.begin(laMetricClient, urlBuffer))
    {
      ON_SCOPE_EXIT( https.end(); );

      String authBase64;
      {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "dev:%s", SA::Config::c_laMetricToken);
        authBase64 = base64::encode(buffer);
      }

      https.addHeader("Authorization", "Basic " + authBase64);
      https.addHeader("Content-Type", "application/json");

      constexpr int bufferSize = 512;
      char buffer0[bufferSize] = R"rl({ "frames": [ %s ] })rl";
      char buffer1[bufferSize];
      char* currentBuff = buffer0;
      char* otherBuff = buffer1;

      for (int frameIndex = 0; frameIndex < frames.size(); ++frameIndex)
      {
        const Frame& frame = frames[frameIndex];
        char frameBuffer[128];
        snprintf(frameBuffer, sizeof(frameBuffer), R"rl(%s{ "text": "%s", "icon": %d }%s)rl", frameIndex == 0 ? "" : ", ", frame.m_text, frame.iconID, "%s");
        snprintf(otherBuff, bufferSize, currentBuff, frameBuffer);
        std::swap(currentBuff, otherBuff);
      }

      snprintf(otherBuff, bufferSize, currentBuff, "");
      const int httpCode = https.POST(otherBuff);
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] POST failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
    }
  }

  void LaMetricRoomTemperatureProvider::Update(NetworkClientSecure& STClient, const char* token)
  {
    if (const auto result = SA::Utils::GetDeviceValue<float>(STClient, token, Config::c_roomThermometerDevice, "temperatureMeasurement/temperature/value", false))
    {
      char valueBuffer[8];
      snprintf(valueBuffer, sizeof(valueBuffer), "%.1f°C", *result);

      bool isHeating = false;
      if (const auto result = SA::Utils::GetDeviceValue<const char*>(STClient, token, Config::c_roomHeaterDevice, "thermostatOperatingState/thermostatOperatingState/value", false))
      {
        isHeating = SA::Utils::StrCmpIgnoreCase(*result, "heating");
      }
      else
      {
        Serial.println("Can't fetch thermostat state");
      }

      Frame frames[] = {Frame{valueBuffer, isHeating ? 19650 : 6157}};
      SetFrames(frames, Config::c_laMetricTemperatureWidgetID);
    }
    else
    {
      Serial.println("Can't fetch room temperature");
    }
  }
}