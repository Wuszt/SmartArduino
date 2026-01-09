#pragma once
#include "UpdateManager.h"
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class LaMetricBluetoothController : public IUpdatable
  {
  public:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
      {
        return 60u * 1000u;
      }
  
  private:
    std::optional<bool> m_shouldBeEnabled;
  };
}