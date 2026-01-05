#include "SmartThingsWorkerInterface.h"
#include <memory>
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class SABleKeyboard;
  class KeyboardTracker : public ISmartThingsWorker
  {
    public:
      KeyboardTracker(std::shared_ptr<SABleKeyboard> keyboard);
      virtual void Update(NetworkClientSecure& client, const char* token) override;

    private:
      std::shared_ptr<SABleKeyboard> m_keyboard;
      std::optional<bool> m_isAuthenticated;
  };
}