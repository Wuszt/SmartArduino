#pragma once
#include "UpdateManager.h"

namespace SA
{
  class LaMetricBrightnessController : public IUpdatable
  {
  public:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 60u * 1000u;
    }

    virtual const char* GetName() const override
    {
      return "LaMetricBrightnessController";
    }
  };
}
