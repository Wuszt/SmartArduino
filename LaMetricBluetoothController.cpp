#include "LaMetricBluetoothController.h"
#include "SmartThingsUtils.h"
#include "Config.h"
#include <HTTPClient.h>
#include "base64.h"
#include <NetworkClient.h>
#include "LaMetricUtils.h"
#include "StringDict.h"

namespace SA
{
  void LaMetricBluetoothController::Update(NetworkClientSecure& client, const char* token)
  {
    if (!m_shouldBeEnabled.has_value())
    {
      const StringDict states(Utils::GetToLaMetric(Config::c_laMetricDeviceBluetooth).c_str(), "/root/");
      if (auto value = states.GetValue<bool>("active"))
      {
        m_shouldBeEnabled = *value;
      }
      else
      {
        Serial.println("Failed to fetch LaMetric bluetooth state");
        return;
      }
    }

    const bool shouldBluetoothBeEnabled = !Utils::IsSwitchEnabled(client, token, Config::c_disableLaMetricBluetoothDevice, false);
    if (*m_shouldBeEnabled == shouldBluetoothBeEnabled)
    {
      return;
    }

    m_shouldBeEnabled = shouldBluetoothBeEnabled;
    
    {
      char bodyBuffer[256];
      snprintf(bodyBuffer, sizeof(bodyBuffer), R"rl({ "active": %s })rl", shouldBluetoothBeEnabled ? "true" : "false");
      Utils::PutToLaMetric(Config::c_laMetricDeviceBluetooth, bodyBuffer);
    }
  }
}