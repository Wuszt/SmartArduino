#pragma once
#include "SmartThingsWorkerInterface.h"

class NetworkClientSecure;
class HTTPClient;

namespace SA
{
  class TVModeDetector : public ISmartThingsWorker
  {
    public:
      void Update(NetworkClientSecure& client, const char* token) override;
    private:
      void UpdateState(NetworkClientSecure& client, const char* token);
  };
}