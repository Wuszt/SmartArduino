#pragma once
class NetworkClientSecure;

namespace SA
{
  class ISmartThingsWorker
  {
  public:
    virtual ~ISmartThingsWorker() = default;
    virtual void Update(NetworkClientSecure& client, const char* token) = 0;
  };
}