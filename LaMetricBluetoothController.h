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
  private:
    std::optional<bool> m_shouldBeEnabled;
  };
}