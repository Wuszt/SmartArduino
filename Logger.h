#include <memory>
class NetworkClientSecure;
class BleKeyboard;

namespace SA
{
  class Logger
  {
    public:
      Logger();
      ~Logger();
      void Update(NetworkClientSecure& client, const char* token);
    private:
      std::unique_ptr<BleKeyboard> m_keyboard;
  };
}