#pragma once
#include "UpdateManager.h"

class NetworkClientSecure;
namespace SA
{
  class LaMetricRoomTemperatureProvider : public IUpdatable
  {
  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
      {
        return 5u * 60u * 1000u;
      }
  };
}