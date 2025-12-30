#include <memory>
#include "SmartKeyboard.h"

namespace SA
{
  class SABleKeyboard;
  class Logger : public SmartKeyboard
  {
    public:
      Logger(std::shared_ptr<SABleKeyboard> keyboard);
      
    protected:
      virtual void OnTriggered() override;
  };
}