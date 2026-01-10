#include "LaMetricRoomTemperatureProvider.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include <Arduino.h>
#include <span>
#include "LaMetricManager.h"

namespace SA
{
  LaMetricRoomTemperatureProvider::LaMetricRoomTemperatureProvider()
  {
    m_requestHandle = std::make_unique<LaMetricFrameRequestHandle>(LaMetricManager::Get().AddFrame("---", 0));
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

      LaMetricManager::Get().UpdateFrame(*m_requestHandle, valueBuffer, isHeating ? 19650 : 6157);
    }
    else
    {
      Serial.println("Can't fetch room temperature");
    }
  }
}