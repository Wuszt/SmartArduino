#include "KeyboardTrigger.h"
#include "Logger.h"
#include "Config.h"
#include <HTTPClient.h>
#include <WString.h>
#include "SmartThingsUtils.h"
#include "StringDict.h"
#include <SABleKeyboard.h>

namespace SA
{
  bool ShouldBeTriggered(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    return Utils::IsSwitchEnabled(client, token, deviceID, true);
  }

  void ReportCompletion(NetworkClientSecure& client, const char* token, const char* deviceID)
  {
    SA::Utils::SetSwitchValue(client, token, deviceID, false);
  }

  KeyboardTrigger::KeyboardTrigger(std::shared_ptr<SABleKeyboard> keyboard, const char* triggeringDeviceID)
    : m_keyboard(keyboard)
    , m_triggeringDeviceID(triggeringDeviceID)
  {}

  void KeyboardTrigger::Update(NetworkClientSecure& client, const char* token)
  {
    if (ShouldBeTriggered(client, token, m_triggeringDeviceID))
    {
      delay(500);
      if (m_keyboard->IsAuthenticated()) 
      {
        OnTriggered();
        ReportCompletion(client, token, m_triggeringDeviceID);
      }
    }
  }
}