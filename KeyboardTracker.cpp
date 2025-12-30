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
    if (!m_hasValue)
    {
      m_isAuthenticated = Utils::GetSwitchValue(client, token, Config::c_keyboardConnectedDevice);
      m_hasValue = true;
    }

    const bool wasAuthenticated = m_isAuthenticated;
    m_isAuthenticated = m_keyboard->IsAuthenticated();
    if (wasAuthenticated != m_isAuthenticated)
    {
      Utils::SetSwitchValue(client, token, Config::c_keyboardConnectedDevice, m_isAuthenticated);
      Serial.printf("Changed keyboard status: %s \n", m_isAuthenticated ? "ON" : "OFF");
    }
  }
}