#include "UpdateManager.h"
#include <memory>
#include <optional>

namespace SA
{
  class LaMetricFrameRequestHandle;
  class Printer3DProvider : public IUpdatable
  {
  public:
    Printer3DProvider();
    ~Printer3DProvider();

    virtual const char* GetName() const override
    {
      return "Printer3DProvider";
    }

  private:
    virtual void Update() override;
    void ConnectToPrinter();

    virtual unsigned long GetInterval() const override
    {
      return 15u * 1000u;
    }

    std::unique_ptr<LaMetricFrameRequestHandle> m_frameHandle;
    std::optional<unsigned long> m_lastInProgressTime;
  };
}
