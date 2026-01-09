#include "KeyboardTrigger.h"
#include "Logger.h"
#include "Config.h"
#include <HTTPClient.h>
#include <WString.h>
#include "SmartThingsManager.h"
#include "StringDict.h"
#include <SABleKeyboard.h>

namespace SA
{
  bool ShouldBeTriggered(const char* deviceID)
  {
    return SmartThingsManager::Get().IsSwitchEnabled(deviceID, true);
  }

  void ReportCompletion(const char* deviceID)
  {
    SmartThingsManager::Get().SetSwitchValue(deviceID, false);
  }

  KeyboardTrigger::KeyboardTrigger(SABleKeyboard& keyboard, const char* triggeringDeviceID)
    : m_keyboard(keyboard)
    , m_triggeringDeviceID(triggeringDeviceID)
  {}

  void KeyboardTrigger::Update()
  {
    if (ShouldBeTriggered(m_triggeringDeviceID))
    {
      if (m_keyboard.IsAuthenticated()) 
      {
        OnTriggered();
        ReportCompletion(m_triggeringDeviceID);
      }
    }
  }
}