#include "UpdateManager.h"
#include <memory>

namespace SA
{
  class LaMetricFrameRequestHandle;
  class CO2Alerter : public IUpdatable
  {
  public:
    CO2Alerter();
    ~CO2Alerter();

  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 60u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_frameHandle;
  };
}