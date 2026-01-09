#include "KeyboardTracker.h"
#include "SmartThingsManager.h"
#include "Config.h"
#include <SABleKeyboard.h>

namespace SA
{
  KeyboardTracker::KeyboardTracker(SABleKeyboard& keyboard)
    : m_keyboard(keyboard)
  {}

  void KeyboardTracker::Update()
  {
    if (!m_isAuthenticated.has_value())
    {
      m_isAuthenticated = SmartThingsManager::Get().IsSwitchEnabled(Config::c_keyboardConnectedDevice, true);
    }

    const bool isAuthenticated = m_keyboard.IsAuthenticated();
    if (*m_isAuthenticated == isAuthenticated)
    {
      return;
    }

    m_isAuthenticated = isAuthenticated;

    SmartThingsManager::Get().SetSwitchValue(Config::c_keyboardConnectedDevice, *m_isAuthenticated);
  }
}