#pragma once
#include <memory>
#include "UpdateManager.h"

namespace SA
{
  class SABleKeyboard;
  class KeyboardTrigger : public IUpdatable
  {
    public:
      KeyboardTrigger(SABleKeyboard& keyboard, const char* triggeringDeviceID);
      virtual void Update() override;

    protected:
      virtual void OnTriggered() = 0;
      SABleKeyboard& m_keyboard;
      const char* m_triggeringDeviceID = nullptr;
  };
}