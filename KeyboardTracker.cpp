#include "KeyboardTracker.h"
#include "SmartThingsUtils.h"
#include "Config.h"
#include <SABleKeyboard.h>

namespace SA
{
  KeyboardTracker::KeyboardTracker(std::shared_ptr<SABleKeyboard> keyboard)
    : m_keyboard(keyboard)
  {}

  void KeyboardTracker::Update(NetworkClientSecure& client, const char* token)
  {
    if (!m_isAuthenticated.has_value())
    {
      m_isAuthenticated = Utils::IsSwitchEnabled(client, token, Config::c_keyboardConnectedDevice, true);
    }

    if (*m_isAuthenticated == m_keyboard->IsAuthenticated())
    {
      return;
    }

    m_isAuthenticated = m_keyboard->IsAuthenticated();

    Utils::SetSwitchValue(client, token, Config::c_keyboardConnectedDevice, *m_isAuthenticated);
  }
}