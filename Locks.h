#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "Asserts.h"

#define CONCAT(X, Y) CONCAT_INNER(X, Y)
#define CONCAT_INNER(X,Y) X##Y

namespace SA
{
  class SpinLock
  {
    public:
      void Lock()
      {
        portENTER_CRITICAL(&m_lock);
      }

      void Unlock()
      {
        portEXIT_CRITICAL(&m_lock);
      }

    private:
      portMUX_TYPE m_lock = portMUX_INITIALIZER_UNLOCKED;
  };

  class Mutex
  {
    public:
      Mutex()
        : m_mutex(xSemaphoreCreateMutex())
      {}

      void Lock()
      {
         VERIFY(xSemaphoreTake(m_mutex, portMAX_DELAY));
      }

      void Unlock()
      {
        xSemaphoreGive(m_mutex);
      }

    private:
      SemaphoreHandle_t m_mutex;
  };
}

namespace SA::Internal
{
  template<class T>
  class ScopeLock
  {
    public:
      ScopeLock(T& instance)
        : m_instance(instance)
      {
        m_instance.Lock();
      }

      ~ScopeLock()
      {
        m_instance.Unlock();
      }

    private:
      T& m_instance;
  };
}

#define SCOPE_LOCK(lock) SA::Internal::ScopeLock<std::remove_reference_t<decltype( lock )>> CONCAT( _scopeLock, __COUNTER__ ) ( lock )
