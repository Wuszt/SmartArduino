#pragma once
#include <WString.h>

class NetworkClientSecure;
class HTTPClient;

namespace SA
{
  class TVModeDetector
  {
    public:
      void Update(NetworkClientSecure& client);
    private:
      void UpdateTokenIfNeeded(NetworkClientSecure& client);
      void UpdateState(NetworkClientSecure& client);
      String m_authToken;
  };
}