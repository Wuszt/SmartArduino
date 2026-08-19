#include "UpdateManager.h"
#include <memory>

namespace SA
{
  class LaMetricFrameRequestHandle;
  class LaMetricAirQualityProvider : public IUpdatable
  {
  public:
    LaMetricAirQualityProvider();
    ~LaMetricAirQualityProvider();

    virtual const char* GetName() const override
    {
      return "LaMetricAirQualityProvider";
    }

  private:
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 30u * 60u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_frameHandle;
  };
}