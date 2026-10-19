#include "LaMetricBrightnessController.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include "LaMetricUtils.h"

namespace SA
{
  void LaMetricBrightnessController::Update()
  {
    const bool shouldAutoBrightnessBeEnabled = !SmartThingsManager::Get().IsSwitchEnabled(Config::c_laMetricAutoBrightnessDevice, false);
    Utils::PutToLaMetric("device/display", shouldAutoBrightnessBeEnabled ? R"({ "brightness_mode": "auto" })" : R"({ "brightness_mode": "manual" })");
  }
}
