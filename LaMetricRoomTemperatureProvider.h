#pragma once
#include "UpdateManager.h"

class NetworkClientSecure;
namespace SA
{
  class LaMetricRoomTemperatureProvider : public IUpdatable
  {
  private:
    virtual void Update() override;
  };
}