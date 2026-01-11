#pragma once
#include "UpdateManager.h"
#include <memory>
#include <vector>
#include <WString.h>
#include <functional>
#include <Locks.h>

class AsyncWebServer;
namespace SA
{
    class RequestsServerListenerHandle
    {
    public:
      RequestsServerListenerHandle()
      {
        static unsigned long s_prev = 0;
        m_value = ++s_prev;
      }

      bool operator==(const RequestsServerListenerHandle&) const = default;

    private:
      unsigned long m_value = 0;
    };

  class RequestsServer
  {
  public:
    using ListenerHandle = RequestsServerListenerHandle;

    RequestsServer();
    ~RequestsServer();
    
    void Init();

    [[nodiscard]] ListenerHandle RegisterListener(String name, std::function<void(const char*)> callback);
    bool UnregisterListener(ListenerHandle handle);
  
  private:
    struct Listener
    {
      String m_name;
      std::function<void(const char*)> m_callback;
      ListenerHandle m_handle;
    };

    std::unique_ptr<AsyncWebServer> m_server;

    SA::Mutex m_listenersMutex;
    std::vector<Listener> m_listeners;
  };
}