#pragma once

#include <memory>
class NetworkClientSecure;

namespace SA
{
  class SABleKeyboard;
  class SmartKeyboard
  {
    public:
      SmartKeyboard(std::shared_ptr<SABleKeyboard> keyboard, const char* triggeringDeviceID);
      void Update(NetworkClientSecure& client, const char* token);

    protected:
      virtual void OnTriggered() = 0;
      std::shared_ptr<SABleKeyboard> m_keyboard;
      const char* m_triggeringDeviceID = nullptr;
  };
}