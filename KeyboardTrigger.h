#pragma once
#include "SmartThingsWorkerInterface.h"

#include <memory>
class NetworkClientSecure;

namespace SA
{
  class SABleKeyboard;
  class KeyboardTrigger : public ISmartThingsWorker
  {
    public:
      KeyboardTrigger(std::shared_ptr<SABleKeyboard> keyboard, const char* triggeringDeviceID);
      virtual void Update(NetworkClientSecure& client, const char* token) override;

    protected:
      virtual void OnTriggered() = 0;
      std::shared_ptr<SABleKeyboard> m_keyboard;
      const char* m_triggeringDeviceID = nullptr;
  };
}