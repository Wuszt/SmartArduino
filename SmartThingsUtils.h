#include "StringDict.h"

class NetworkClientSecure;
namespace SA::Utils
{
  String GetToken(NetworkClientSecure& client);

  void RefreshDevice(NetworkClientSecure& client, const char* token, const char* deviceID);
  StringDict GetDeviceStatus(NetworkClientSecure& client, const char* token, const char* deviceID, bool forceDataRefresh);

  template<class T>
  std::optional<T> GetDeviceValue(NetworkClientSecure& client, const char* token, const char* deviceID, const char* key, bool forceDataRefresh)
  {
    return GetDeviceStatus(client, token, deviceID, forceDataRefresh).GetValue<T>(key);
  }
  
  bool IsSwitchEnabled(NetworkClientSecure& client, const char* token, const char* deviceID, bool forceDataRefresh);

  void SendCommand(NetworkClientSecure& client, const char* token, const char* deviceID, const char* component, const char* capability, const char* command);
  void SetSwitchValue(NetworkClientSecure& client, const char* token, const char* deviceID, bool value);
}