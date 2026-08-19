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

      virtual const char* GetName() const override
      {
        return "SteamBootstrapper";
      }

    protected:
      virtual void OnTriggered() override;
  };
}