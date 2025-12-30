#pragma once

#include <memory>
#include "SmartKeyboard.h"

namespace SA
{
  class SABleKeyboard;
  class SteamBootstrapper : public SmartKeyboard
  {
    public:
      SteamBootstrapper(std::shared_ptr<SABleKeyboard> keyboard);

    protected:
      virtual void OnTriggered() override;
  };
}