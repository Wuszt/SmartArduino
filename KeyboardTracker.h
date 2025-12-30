#include <memory>

class NetworkClientSecure;

namespace SA
{
  class SABleKeyboard;
  class KeyboardTracker
  {
    public:
      KeyboardTracker(std::shared_ptr<SABleKeyboard> keyboard);
      void Update(NetworkClientSecure& client, const char* token);

    private:
      std::shared_ptr<SABleKeyboard> m_keyboard;
      bool m_isAuthenticated = false;
      bool m_hasValue = false;
  };
}