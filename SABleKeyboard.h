#include <NimBleKeyboard.h>

namespace SA
{
  class SABleKeyboard : public BleKeyboard
  {
    public:
      using BleKeyboard::BleKeyboard;

    bool IsAuthenticated() const
    {
      return m_isAuthenticated;
    }    

    protected:
      virtual void onAuthenticationComplete(NimBLEConnInfo& connInfo) override
      {
        BleKeyboard::onAuthenticationComplete(connInfo);
        m_isAuthenticated = true;
      }

      virtual void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo &connInfo, int reason) override
      {
        BleKeyboard::onDisconnect(pServer, connInfo, reason);
        m_isAuthenticated = false;
      }
      
    private:
      bool m_isAuthenticated = false;
  };
}