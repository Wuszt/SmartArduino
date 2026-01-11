#include "UpdateManager.h"
#include <memory>

namespace SA
{
  class LaMetricFrameRequestHandle;
  class LaMetricAirQualityProvider : public IUpdatable
  {
  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 30u * 60u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_frameHandle;
  };
}