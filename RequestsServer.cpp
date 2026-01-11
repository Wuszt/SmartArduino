#include "RequestsServer.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Utils.h"

namespace SA
{
  RequestsServer::RequestsServer() = default;
  RequestsServer::~RequestsServer()
  {
    if (m_server)
    {
      m_server->end();
    }
  }
  
  void RequestsServer::Init()
  {
    m_server = std::make_unique<AsyncWebServer>(80);
    m_server->on(
      "/api",
      HTTP_POST,
      [](AsyncWebServerRequest *request){},
      nullptr,
      [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) 
      {
        if (index == 0)
        {
          request->_tempObject = new String();
        }

        String* buffer = static_cast<String*>(request->_tempObject);
        *buffer += String((char*)data).substring(0, len);

        if (index + len == total) 
        {
          {
            const std::string_view bufferView(buffer->c_str(), buffer->length());
            SCOPE_LOCK(m_listenersMutex);
            for (const auto& listener : m_listeners)
            {
              const int listenerNameLength = listener.m_name.length();
              if (bufferView.starts_with(listener.m_name.c_str()) && bufferView.size() > listenerNameLength && bufferView[listenerNameLength] == ':')
              {
                listener.m_callback(buffer->c_str() + listenerNameLength + 1);
              }
            }
          }
          request->send(200, "text/plain", "OK");
          request->_tempObject = nullptr;
          delete buffer;
        }
      }
    );

    m_server->begin();
  }

  [[nodiscard]] RequestsServer::ListenerHandle RequestsServer::RegisterListener(String name, std::function<void(const char*)> callback)
  {
    SCOPE_LOCK(m_listenersMutex);
    m_listeners.push_back({std::move(name), std::move(callback)});
    return m_listeners.back().m_handle;
  }

  bool RequestsServer::UnregisterListener(ListenerHandle handle)
  {
    SCOPE_LOCK(m_listenersMutex);
    auto it = std::find_if(m_listeners.begin(), m_listeners.end(), [&](const Listener& listener) { return listener.m_handle == handle; });
    if (it != m_listeners.end())
    {
      m_listeners.erase(it);
      return true;
    }

    return false;
  }
}