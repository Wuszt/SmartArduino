#pragma once
#include "SmartThingsWorkerInterface.h"
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class TVModeDetector : public ISmartThingsWorker
  {
    public:
      void Update(NetworkClientSecure& client, const char* token) override;
    private:
      std::optional<bool> m_isEnabled;
  };
}