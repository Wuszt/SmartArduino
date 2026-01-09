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
    private:
      std::optional<bool> m_isEnabled;
  };
}