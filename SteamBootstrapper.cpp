#include "SteamBootstrapper.h"
#include "Config.h"
#include <SABleKeyboard.h>

namespace SA
{
  SteamBootstrapper::SteamBootstrapper(std::shared_ptr<SABleKeyboard> keyboard)
    : KeyboardTrigger(keyboard, Config::c_steamTriggerDevice)
  {}

  void SteamBootstrapper::OnTriggered()
  {
      m_keyboard->press(KEY_RIGHT_CTRL);
      delay(500);
      m_keyboard->press(KEY_RIGHT_ALT);
      delay(500);
      m_keyboard->press(KEY_INSERT);
      delay(1000);
      m_keyboard->releaseAll();

      Serial.println("Steam triggered!");
  }
}