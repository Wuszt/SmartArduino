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
    if (!m_isEnabled.has_value())
    {
      m_isEnabled = Utils::IsSwitchEnabled(client, token, Config::c_outputDeviceID, true);
    }

    bool shouldTVModeBeOn = false;
    const StringDict states = Utils::GetDeviceStatus(client, token, SA::Config::c_TVDeviceID, true);
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
      Utils::SetSwitchValue(client, token, Config::c_outputDeviceID, shouldTVModeBeOn);
      m_isEnabled = shouldTVModeBeOn;
    }
  }
}