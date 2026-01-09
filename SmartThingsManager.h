#pragma once
#include <optional>
#include <WString.h>
#include <NetworkClientSecure.h>
#include "StringDict.h"

namespace SA
{
  class StringDict;
  class SmartThingsManager
  {
    public:
      static SmartThingsManager& Get()
      {
        static SmartThingsManager s_instance;
        return s_instance;
      }

      void PostUpdate()
      {
        m_cache.reset();
      }

      void RefreshDevice(const char* deviceID);
      StringDict GetDeviceStatus(const char* deviceID, bool forceDataRefresh);

      template<class T>
      std::optional<T> GetDeviceValue(const char* deviceID, const char* key, bool forceDataRefresh)
      {
        return GetDeviceStatus(deviceID, forceDataRefresh).GetValue<T>(key);
      }
      
      bool IsSwitchEnabled(const char* deviceID, bool forceDataRefresh);

      void SendCommand(const char* deviceID, const char* component, const char* capability, const char* command);
      void SetSwitchValue(const char* deviceID, bool value);

    private:
      struct Cache
      {
        String m_authToken;
        NetworkClientSecure m_client;
      };

      Cache& GetCache();

      NetworkClientSecure& GetClient();
      const char* GetToken();
      
      std::optional<Cache> m_cache;
  };
}