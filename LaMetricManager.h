#include <WString.h>
#include <vector>
#include <optional>

namespace SA
{
  class LaMetricFrameRequestHandle
  {
  public:
    LaMetricFrameRequestHandle()
    {
      static unsigned long s_prev = 0;
      m_value = ++s_prev;
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

      struct Frame
      {
        struct GoalData
        {
          int m_start = 0;
          int m_current = 0;
          int m_end = 0;
        };

        String m_text;
        int m_icon = 0;
        std::optional<GoalData> m_goalData;
        int m_duration = 3000;
      };

      [[nodiscard]] RequestHandle AddFrame(Frame frame);
      bool UpdateFrame(RequestHandle handle, Frame frame);
      bool RemoveFrame(RequestHandle handle);

      void PostUpdate();

    private:
      void SetFrames();

      struct RegisteredFrame
      {
        Frame m_frame;
        RequestHandle m_handle;
      };

      std::vector<RegisteredFrame> m_frames;
      bool m_areFramesDirty = false;
  };
}