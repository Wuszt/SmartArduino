#include "Printer3DProvider.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "LaMetricManager.h"
#include "Config.h"
#include "StringDict.h"
#include "Utils.h"

namespace SA
{
  Printer3DProvider::Printer3DProvider() = default;
  Printer3DProvider::~Printer3DProvider() = default;

  void Printer3DProvider::Update()
  {
    NetworkClientSecure networkClient;
    PubSubClient mqttClient(networkClient);
    networkClient.setInsecure();
    mqttClient.setServer(Config::c_printerIP, 8883);
    bool receivedReply = false;
    mqttClient.setCallback([&](char* topic, byte* payload, unsigned int length)
    {
      receivedReply = true;
      const StringDict dict{std::string_view(reinterpret_cast<const char*>(payload), length), "/print/"};

      const char* gcodeState = *dict.GetValue<const char*>("gcode_state");
      const int printPercent = *dict.GetValue<int>("mc_percent");

      std::optional<LaMetricManager::Frame> frame;
      if (strcmp(gcodeState, "PAUSE") == 0)
      {
        frame = LaMetricManager::Frame();
        frame->m_text = "PAUSED";
        frame->m_icon = 68593;
        frame->m_goalData = {.m_start = 0, .m_current = printPercent, .m_end = 100};
      }
      else if (strcmp(gcodeState, "FINISH") == 0 && m_lastInProgressTime && ((millis() - *m_lastInProgressTime) < 15u * 60u * 1000u))
      {
        frame = LaMetricManager::Frame();
        frame->m_text = "FINISHED";
        frame->m_icon = 75666;
      }
      else
      {
        const auto remainingTime = dict.GetValue<int>("mc_remaining_time");
        if (remainingTime && *remainingTime > 0)
        {
          const int remainingHours = *remainingTime / 60;
          const int remainingMinutes = *remainingTime - remainingHours * 60;

          frame = LaMetricManager::Frame();
          char valueBuffer[16];
          snprintf(valueBuffer, sizeof(valueBuffer), "%dh %dm", remainingHours, remainingMinutes);
          frame->m_text = valueBuffer;
          frame->m_icon = 55700;
          frame->m_goalData = {.m_start = 0, .m_current = printPercent, .m_end = 100};
          m_lastInProgressTime = millis();
        }
      }

      if (frame)
      {
        if (m_frameHandle)
        {
          LaMetricManager::Get().UpdateFrame(*m_frameHandle, std::move(*frame));
        }
        else
        {
          m_frameHandle = std::make_unique<LaMetricFrameRequestHandle>(LaMetricManager::Get().AddFrame(std::move(*frame)));
        }
      }
      else if (m_frameHandle)
      {
        LaMetricManager::Get().RemoveFrame(*m_frameHandle);
        m_frameHandle = nullptr;
      }
    });
    mqttClient.setBufferSize(16384);

    {
      char clientIDBuffer[256];
      snprintf(clientIDBuffer, sizeof(clientIDBuffer), "SmartArduino_%llx", static_cast<unsigned long long>(ESP.getEfuseMac()));
      mqttClient.connect(clientIDBuffer, "bblp", Config::c_printerAccessCode);
    }

    ON_SCOPE_EXIT( mqttClient.disconnect(); );

    if (mqttClient.connected())
    {
      {
        char reportTopicBuffer[256];
        snprintf(reportTopicBuffer, sizeof(reportTopicBuffer), "device/%s/report", Config::c_printerSerial);
        mqttClient.subscribe(reportTopicBuffer);
      }

      {
        const char body[] = R"({"pushing":{"sequence_id":"0","command":"pushall","version":1,"push_target":1}})";
        char requestTopicBuffer[256];
        snprintf(requestTopicBuffer, sizeof(requestTopicBuffer), "device/%s/request", Config::c_printerSerial);
        mqttClient.publish(requestTopicBuffer, reinterpret_cast<const uint8_t*>(body), sizeof(body) - 1);
      }

      unsigned long startTime = millis();
      constexpr unsigned long maxWaitingTime = 2000u;
      while (!receivedReply && ((millis() - startTime) < maxWaitingTime))
      {
        mqttClient.loop();
        delay(50);
      }

      if (receivedReply)
      {
        return;
      }
    }

      Serial.printf("[%hs] Failed to connect to the printer\n", __FUNCTION__);

      if (m_frameHandle)
      {
        LaMetricManager::Get().RemoveFrame(*m_frameHandle);
        m_frameHandle = nullptr;
      }
  }
}
