#include <memory>
#include "KeyboardTrigger.h"

namespace SA
{
  class SABleKeyboard;
  class Logger : public KeyboardTrigger
  {
    public:
      Logger(SABleKeyboard& keyboard);
      
    protected:
      virtual void OnTriggered() override;
  };
}