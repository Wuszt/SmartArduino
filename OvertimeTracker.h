#include "UpdateManager.h"
#include <memory>
#include <optional>

namespace SA
{
  class RequestsServer;
  class RequestsServerListenerHandle;
  class LaMetricFrameRequestHandle;
  
  class OvertimeTracker : public IUpdatable
  {
  public:
    OvertimeTracker(RequestsServer& server);
    ~OvertimeTracker();
    virtual void Update() override;
    virtual unsigned long GetInterval() const override
    {
      return 10u * 1000u;
    }

  private:
    std::unique_ptr<RequestsServerListenerHandle> m_serverRequestHandle;
    std::unique_ptr<LaMetricFrameRequestHandle> m_frameRequestHandle;
    RequestsServer& m_server;
    std::optional<int> m_remainingTime = 0;
  };
}