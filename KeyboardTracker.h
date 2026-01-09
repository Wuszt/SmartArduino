#include "UpdateManager.h"
#include <memory>
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class SABleKeyboard;
  class KeyboardTracker : public IUpdatable
  {
    public:
      KeyboardTracker(SABleKeyboard& keyboard);
      virtual void Update() override;
      virtual unsigned long GetInterval() const override
      {
        return 10u * 1000u;
      }

    private:
      SABleKeyboard& m_keyboard;
      std::optional<bool> m_isAuthenticated;
  };
}