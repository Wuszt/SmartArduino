#pragma once
#include "SmartThingsWorkerInterface.h"
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class LaMetricBluetoothController : public ISmartThingsWorker
  {
  public:
    virtual void Update(NetworkClientSecure& client, const char* token) override;
  private:
    std::optional<bool> m_shouldBeEnabled;
  };
}