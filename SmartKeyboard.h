#pragma once

#include <memory>
class NetworkClientSecure;
class BleKeyboard;

namespace SA
{
  class SmartKeyboard
  {
    public:
      SmartKeyboard(std::shared_ptr<BleKeyboard> keyboard, const char* triggeringDeviceID);
      void Update(NetworkClientSecure& client, const char* token);

    protected:
      virtual void OnTriggered() = 0;
      std::shared_ptr<BleKeyboard> m_keyboard;
      const char* m_triggeringDeviceID = nullptr;
  };
}