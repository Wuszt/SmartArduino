#pragma once

#include <memory>
#include "SmartKeyboard.h"

class BleKeyboard;

namespace SA
{
  class SteamBootstrapper : public SmartKeyboard
  {
    public:
      SteamBootstrapper(std::shared_ptr<BleKeyboard> keyboard);

    protected:
      virtual void OnTriggered() override;
  };
}