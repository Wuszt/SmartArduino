#include <memory>
#include "KeyboardTrigger.h"

namespace SA
{
  class SABleKeyboard;
  class Logger : public KeyboardTrigger
  {
    public:
      Logger(SABleKeyboard& keyboard);
      
      virtual const char* GetName() const override
      {
        return "Logger";
      }

    protected:
      virtual void OnTriggered() override;
  };
}