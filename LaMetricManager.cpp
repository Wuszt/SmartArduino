#include "LaMetricManager.h"
#include "Config.h"
#include "LaMetricUtils.h"
#include <cstdio>
#include <algorithm>
#include <Arduino.h>

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
      const Frame& frame = m_frames[frameIndex].m_frame;

      char goalDataBuffer[128] = "";
      if (frame.m_goalData.has_value())
      {
        snprintf(goalDataBuffer, sizeof(goalDataBuffer), R"rl(, "goalData": { "start": %d, "current": %d, "end": %d })rl", 
          frame.m_goalData->m_start,
          frame.m_goalData->m_current,
          frame.m_goalData->m_end);
      }

      char durationBuffer[32] = "";
      if (frame.m_duration > 0)
      {
        snprintf(goalDataBuffer, sizeof(goalDataBuffer), R"rl(, "duration": %d)rl", frame.m_duration); 
      }

      char frameBuffer[128];
      snprintf(frameBuffer, sizeof(frameBuffer), R"rl(%s{ "text": "%s", "icon": %d%s%s }%s)rl", frameIndex == 0 ? "" : ", ", 
      frame.m_text.c_str(), 
      frame.m_icon, 
      goalDataBuffer,
      durationBuffer,
      "%s");

      snprintf(otherBuff, bufferSize, currentBuff, frameBuffer);
      std::swap(currentBuff, otherBuff);
    }

    snprintf(otherBuff, bufferSize, currentBuff, "");
    snprintf(currentBuff, bufferSize, "%s/actions", Config::c_laMetricWidgetID);
    Serial.println(otherBuff);
    Utils::PostToLaMetric(currentBuff, otherBuff);
  }

  LaMetricManager::RequestHandle LaMetricManager::AddFrame(Frame frame)
  {
    m_frames.push_back({std::move(frame)});
    m_areFramesDirty = true;
    return m_frames.back().m_handle;
  }

  bool LaMetricManager::UpdateFrame(RequestHandle handle, Frame frame)
  {
    auto it = std::find_if(m_frames.begin(), m_frames.end(), [&](const RegisteredFrame& frame) { return frame.m_handle == handle; });
    if (it != m_frames.end())
    {
      it->m_frame = std::move(frame);
      m_areFramesDirty = true;
      return true;
    }

    return false;
  }

  bool LaMetricManager::RemoveFrame(RequestHandle handle)
  {
    auto it = std::find_if(m_frames.begin(), m_frames.end(), [&](const RegisteredFrame& frame) { return frame.m_handle == handle; });
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