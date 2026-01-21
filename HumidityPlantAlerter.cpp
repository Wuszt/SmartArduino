#include "HumidityPlantAlerter.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include "LaMetricManager.h"

namespace SA
{
    HumidityPlantAlerter::HumidityPlantAlerter() = default;
    HumidityPlantAlerter::~HumidityPlantAlerter() = default;

  void HumidityPlantAlerter::Update()
  {
    if (const auto result = SmartThingsManager::Get().GetDeviceValue<float>(Config::c_humidityPlantDevice, "relativeHumidityMeasurement/humidity/value", false))
    {
      LaMetricManager::Frame frame;
      frame.m_text = "PODLEJ";
      frame.m_icon = 72618;
      frame.m_duration = 3100;

      constexpr float humidityThreshold = 5.0f;

      if (*result < humidityThreshold)
      {
        if (m_frameHandle)
        {
          LaMetricManager::Get().UpdateFrame(*m_frameHandle, std::move(frame));
        }
        else
        {
          m_frameHandle = std::make_unique<LaMetricFrameRequestHandle>(LaMetricManager::Get().AddFrame(std::move(frame)));
        }
      }
      else if (m_frameHandle)
      {
        LaMetricManager::Get().RemoveFrame(*m_frameHandle);
        m_frameHandle = nullptr;
      }
    }
  }
}