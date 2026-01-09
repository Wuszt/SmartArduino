#pragma once
#include "UpdateManager.h"
#include <optional>

class NetworkClientSecure;

namespace SA
{
  class TVModeDetector : public IUpdatable
  {
    public:
      void Update() override;
      virtual unsigned long GetInterval() const override
      {
        return 10 * 1000;
      }
    private:
      std::optional<bool> m_isEnabled;
  };
}