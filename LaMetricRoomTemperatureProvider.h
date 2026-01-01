#pragma once
#include "SmartThingsWorkerInterface.h"

namespace SA
{
  class LaMetricRoomTemperatureProvider : public ISmartThingsWorker
  {
    virtual void Update(NetworkClientSecure& client, const char* token) override;
  };
}