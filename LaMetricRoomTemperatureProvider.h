#pragma once
#include "UpdateManager.h"
#include <memory>

class NetworkClientSecure;
namespace SA
{
  struct LaMetricFrameRequestHandle;
  class LaMetricRoomTemperatureProvider : public IUpdatable
  {
  public:
      LaMetricRoomTemperatureProvider();
      
  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 5u * 60u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_requestHandle;
  };
}