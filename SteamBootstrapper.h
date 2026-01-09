#pragma once

#include <memory>
#include "KeyboardTrigger.h"

namespace SA
{
  class SABleKeyboard;
  class SteamBootstrapper : public KeyboardTrigger
  {
    public:
      SteamBootstrapper(SABleKeyboard& keyboard);

    protected:
      virtual void OnTriggered() override;
  };
}