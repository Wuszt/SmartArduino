#include "CO2Alerter.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include "LaMetricManager.h"

namespace SA
{
    CO2Alerter::CO2Alerter() = default;
    CO2Alerter::~CO2Alerter() = default;

  void CO2Alerter::Update()
  {
    if (const auto result = SmartThingsManager::Get().GetDeviceValue<int>(Config::c_co2DetectorDevice, "carbonDioxideMeasurement/carbonDioxide/value", false))
    {
      constexpr int softCo2Threshold = 750;
      constexpr int hardCo2Threshold = 1000;

      if (*result > softCo2Threshold)
      {
        LaMetricManager::Frame frame;
        frame.m_text = (*result > hardCo2Threshold) ? "LOW O2!" : "LOW 02";
        frame.m_icon =  (*result > hardCo2Threshold) ? 31830 : 73113;

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