#include "Logger.h"
#include "Config.h"
#include <SABleKeyboard.h>

namespace SA
{
  Logger::Logger(SABleKeyboard& keyboard)
    : KeyboardTrigger(keyboard, Config::c_pinTriggerDevice)
  {}

  void Logger::OnTriggered()
  {
    {
      m_keyboard.press(KEY_LEFT_CTRL);
      delay(50);
      m_keyboard.press(KEY_LEFT_ALT);
      delay(50);
      m_keyboard.press(KEY_DELETE);
      delay(100);
      m_keyboard.releaseAll();
    }

    delay(500);
    m_keyboard.write(KEY_RETURN);

    delay(500);
    for (const char* c = Config::c_pin; *c != '\0'; ++c)
    {
      delay(50);
      m_keyboard.write(*c);
    }

    Serial.println("Logging triggered!");
  }
}