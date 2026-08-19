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

      virtual const char* GetName() const override
      {
        return "TVModeDetector";
      }
    private:
      std::optional<bool> m_isEnabled;
  };
}