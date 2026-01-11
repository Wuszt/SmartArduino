#include <stdio.h>
#include "OvertimeTracker.h"
#include "RequestsServer.h"
#include "Utils.h"
#include "Arduino.h"
#include "LaMetricManager.h"
#include "SmartThingsManager.h"
#include "Config.h"

namespace SA
{
  OvertimeTracker::OvertimeTracker(RequestsServer& server)
    : m_server(server)
  {
    m_serverRequestHandle = std::make_unique<RequestsServerListenerHandle>(server.RegisterListener("OvertimeTracker", [&](const char* payload)
    {
      if (auto remainingTime = Utils::ParseValue<int>(payload))
      {
        m_remainingTime = *remainingTime;
      }
      else
      {
        Serial.printf("[%hs] Failed to parse the request. \n", __FUNCTION__);
      }
    }));
  }

  OvertimeTracker::~OvertimeTracker()
  {
    m_server.UnregisterListener(*m_serverRequestHandle);
  }

  void OvertimeTracker::Update()
  {
    LaMetricManager::Frame frame;

    if (m_remainingTime.has_value())
    {
      if (!SmartThingsManager::Get().IsSwitchEnabled(Config::c_overtimeActivatorDevice, false))
      {
        if (m_frameRequestHandle)
        {
          LaMetricManager::Get().RemoveFrame(*m_frameRequestHandle);
        }

        m_remainingTime.reset();
        return;
      }

      if (*m_remainingTime > 0)
      {
        const int remainingHours = static_cast<int>(*m_remainingTime / 3600);
        const int remainingMinutes = static_cast<int>(*m_remainingTime / 60) - remainingHours * 60;
        const int maxTime = 8 * 60 * 60;
        char textBuffer[64];
        snprintf(textBuffer, sizeof(textBuffer), "%dh %dm", remainingHours, remainingMinutes);
        frame.m_text = textBuffer;
        frame.m_icon = 51055;
        frame.m_goalData = {.m_start = 0, .m_current = *m_remainingTime, .m_end = maxTime};
      }
      else
      {
        frame.m_text = "OVER";
        frame.m_icon = 681;
      }

      if (m_frameRequestHandle == nullptr)
      {
        m_frameRequestHandle = std::make_unique<LaMetricFrameRequestHandle>(LaMetricManager::Get().AddFrame(std::move(frame)));
      }
      else
      {
        LaMetricManager::Get().UpdateFrame(*m_frameRequestHandle, std::move(frame));
      }
    }
  }
}