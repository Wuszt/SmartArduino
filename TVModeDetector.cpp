#include "TVModeDetector.h"
#include <optional>
#include "StringDict.h"
#include "Config.h"
#include <WString.h>
#include "SmartThingsManager.h"

namespace SA
{
  void TVModeDetector::Update()
  {
    if (!m_isEnabled.has_value())
    {
      m_isEnabled = SmartThingsManager::Get().IsSwitchEnabled(Config::c_outputDeviceID, true);
    }

    bool shouldTVModeBeOn = false;
    const StringDict states = SmartThingsManager::Get().GetDeviceStatus(SA::Config::c_TVDeviceID, true);
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

    if (shouldTVModeBeOn != m_isEnabled)
    {
      SmartThingsManager::Get().SetSwitchValue(Config::c_outputDeviceID, shouldTVModeBeOn);
      m_isEnabled = shouldTVModeBeOn;
    }
  }
}