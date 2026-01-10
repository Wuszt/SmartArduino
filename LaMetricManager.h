#include <WString.h>
#include <vector>

namespace SA
{
  struct LaMetricFrameRequestHandle
  {
    LaMetricFrameRequestHandle()
    {
      static unsigned long m_prev = 0;
      m_value == ++m_prev;
    }

    bool operator==(const LaMetricFrameRequestHandle&) const = default;

  private:
    unsigned long m_value = 0;
  };

  class LaMetricManager
  {
    public:
      using RequestHandle = LaMetricFrameRequestHandle;

      static LaMetricManager& Get()
      {
        static LaMetricManager s_instance;
        return s_instance;
      }

      [[nodiscard]] RequestHandle AddFrame(const char* text, int icon);
      bool UpdateFrame(RequestHandle handle, const char* text, int icon);
      bool RemoveFrame(RequestHandle handle);

      void PostUpdate();

    private:
      void SetFrames();

      struct Frame
      {
        String m_text;
        int m_icon;
        RequestHandle m_handle;
      };

      std::vector<Frame> m_frames;
      bool m_areFramesDirty = false;
  };
}