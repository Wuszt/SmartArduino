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

    private:
      SABleKeyboard& m_keyboard;
      std::optional<bool> m_isAuthenticated;
  };
}