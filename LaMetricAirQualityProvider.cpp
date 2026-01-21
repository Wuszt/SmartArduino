#include "LaMetricAirQualityProvider.h"
#include "Config.h"
#include "LaMetricManager.h"
#include <HTTPClient.h>
#include <NetworkClient.h>
#include "Asserts.h"
#include "Utils.h"
#include "StringDict.h"

namespace SA
{
  enum class Quality
  {
    Good,
    Moderate,
    Bad,
    VeryBad
  };

  Quality GetQuality(int aqi)
  {
    if (aqi < 50)
    {
      return Quality::Good;
    }
    else if (aqi < 100)
    {
      return Quality::Moderate;
    }
    else if (aqi < 150)
    {
      return Quality::Bad;
    }
    else
    {
      return Quality::VeryBad;
    }
  }

  LaMetricAirQualityProvider::LaMetricAirQualityProvider() = default;
  LaMetricAirQualityProvider::~LaMetricAirQualityProvider() = default;

  void LaMetricAirQualityProvider::Update()
  {
    NetworkClient client;
    HTTPClient https;

    if (https.begin(client, Config::c_airQualityURL))
    {
      ON_SCOPE_EXIT( https.end(); );
      https.addHeader("Content-Type", "application/json");

      const int httpCode = https.GET();
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] GET failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
      const StringDict dict{https.getString().c_str(), "/root/"};
      if (auto aqi = dict.GetValue<int>("data/aqi"))
      {
        const Quality quality = GetQuality(*aqi);
        LaMetricManager::Frame frame;
        switch(quality)
        {
          case Quality::Good:
            if (m_frameHandle)
            {
              LaMetricManager::Get().RemoveFrame(*m_frameHandle);
              m_frameHandle = nullptr;
            }
            return;
          case Quality::Moderate:
            frame.m_text = "Srednie";
            frame.m_icon = 66331;
            break;
          case Quality::Bad:
            frame.m_text = "Złe";
            frame.m_icon = 66333;
            break;
          case Quality::VeryBad:
            frame.m_text = "B. Złe";
            frame.m_icon = 62260;
            break;
          default:
            ASSERT(false);
        }

        if (m_frameHandle)
        {
          LaMetricManager::Get().UpdateFrame(*m_frameHandle, std::move(frame));
        }
        else
        {
          m_frameHandle = std::make_unique<LaMetricFrameRequestHandle>(LaMetricManager::Get().AddFrame(std::move(frame)));
        }
      }
      else
      {
        Serial.printf("[%hs] Failed to fetch AQI\n", __FUNCTION__);
      }
    }
  }
}