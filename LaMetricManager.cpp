#include "LaMetricManager.h"
#include "Config.h"
#include "LaMetricUtils.h"
#include <cstdio>
#include <algorithm>

namespace SA
{
  void LaMetricManager::SetFrames()
  {
    constexpr int bufferSize = 512;
    char buffer0[bufferSize] = R"rl({ "frames": [ %s ] })rl";
    char buffer1[bufferSize];
    char* currentBuff = buffer0;
    char* otherBuff = buffer1;

    for (int frameIndex = 0; frameIndex < m_frames.size(); ++frameIndex)
    {
      const Frame& frame = m_frames[frameIndex];
      char frameBuffer[128];
      snprintf(frameBuffer, sizeof(frameBuffer), R"rl(%s{ "text": "%s", "icon": %d }%s)rl", frameIndex == 0 ? "" : ", ", frame.m_text.c_str(), frame.m_icon, "%s");
      snprintf(otherBuff, bufferSize, currentBuff, frameBuffer);
      std::swap(currentBuff, otherBuff);
    }

    snprintf(otherBuff, bufferSize, currentBuff, "");
    snprintf(currentBuff, bufferSize, "%s/actions", Config::c_laMetricWidgetID);
    Utils::PostToLaMetric(currentBuff, otherBuff);
  }

  LaMetricManager::RequestHandle LaMetricManager::AddFrame(const char* text, int icon)
  {
    m_frames.push_back({text, icon});
    m_areFramesDirty = true;
    return m_frames.back().m_handle;
  }

  bool LaMetricManager::UpdateFrame(RequestHandle handle, const char* text, int icon)
  {
    auto it = std::find_if(m_frames.begin(), m_frames.end(), [&](const Frame& frame) { return frame.m_handle == handle; });
    if (it != m_frames.end())
    {
      it->m_text = text;
      it->m_icon = icon;
      m_areFramesDirty = true;
      return true;
    }

    return false;
  }

  bool LaMetricManager::RemoveFrame(RequestHandle handle)
  {
    auto it = std::find_if(m_frames.begin(), m_frames.end(), [&](const Frame& frame) { return frame.m_handle == handle; });
    if (it != m_frames.end())
    {
      m_frames.erase(it);
      m_areFramesDirty = true;
      return true;
    }

    return false;
  }

  void LaMetricManager::PostUpdate()
  {
    if (m_areFramesDirty)
    {
      SetFrames();
      m_areFramesDirty = false;
    }
  }
}