#include <memory>
#include "KeyboardTrigger.h"

namespace SA
{
  class SABleKeyboard;
  class Logger : public KeyboardTrigger
  {
    public:
      Logger(std::shared_ptr<SABleKeyboard> keyboard);
      
    protected:
      virtual void OnTriggered() override;
  };
}