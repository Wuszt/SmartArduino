#include "LaMetricRoomTemperatureProvider.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include <Arduino.h>
#include <span>
#include "LaMetricUtils.h"

namespace SA
{
  struct Frame
  {
    const char* m_text = nullptr;
    int iconID = 0;
  };

  void SetFrames(std::span<const Frame> frames, const char* widgetID)
  {
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
    snprintf(currentBuff, bufferSize, "%s/actions", widgetID);
    Utils::PostToLaMetric(currentBuff, otherBuff);
  }

  void LaMetricRoomTemperatureProvider::Update()
  {
    if (const auto result = SmartThingsManager::Get().GetDeviceValue<float>(Config::c_roomThermometerDevice, "temperatureMeasurement/temperature/value", false))
    {
      char valueBuffer[8];
      snprintf(valueBuffer, sizeof(valueBuffer), "%.1f°C", *result);

      bool isHeating = false;
      if (const auto result = SmartThingsManager::Get().GetDeviceValue<const char*>(Config::c_roomHeaterDevice, "thermostatOperatingState/thermostatOperatingState/value", false))
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