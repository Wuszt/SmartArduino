#include "UpdateManager.h"
#include <memory>

namespace SA
{
  class LaMetricFrameRequestHandle;
  class HumidityPlantAlerter : public IUpdatable
  {
  public:
    HumidityPlantAlerter();
    ~HumidityPlantAlerter();

  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 5u * 60u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_frameHandle;
  };
}