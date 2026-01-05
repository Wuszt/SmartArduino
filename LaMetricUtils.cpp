#include "LaMetricUtils.h"
#include <HTTPClient.h>
#include "base64.h"
#include <NetworkClient.h>
#include <String>
#include "Config.h"
#include "Utils.h"

namespace SA::Utils
{
  void SendToLaMetric(const char* subUrl, std::function<void(HTTPClient&)> func)
  {
    NetworkClient laMetricClient;
    HTTPClient https;

    char urlBuffer[256];
    snprintf(urlBuffer, sizeof(urlBuffer), "%s/%s", Config::c_laMetricURL, subUrl);
    if (https.begin(laMetricClient, urlBuffer))
    {
      ON_SCOPE_EXIT( https.end(); );

      String authBase64;
      {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "dev:%s", SA::Config::c_laMetricToken);
        authBase64 = base64::encode(buffer);
      }

      https.addHeader("Authorization", "Basic " + authBase64);
      https.addHeader("Content-Type", "application/json");

      func(https);
    }
  }

  String PostToLaMetric(const char* subUrl, const char* body)
  {
    String result;
    SendToLaMetric(subUrl, [&](HTTPClient& https)
    {
      const int httpCode = https.POST(body);
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] POST failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
      result = https.getString();
    });

    return result;
  }

  String GetToLaMetric(const char* subUrl)
  {
    String result;
    SendToLaMetric(subUrl, [&](HTTPClient& https)
    {
      const int httpCode = https.GET();
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] GET failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
      result = https.getString();
    });

    return result;
  }

  String PutToLaMetric(const char* subUrl, const char* body)
  {
    String result;
    SendToLaMetric(subUrl, [&](HTTPClient& https)
    {
      const int httpCode = https.PUT(body);
      if (httpCode != HTTP_CODE_OK)
      {
        Serial.printf("[%hs] PUT failed, error: %d | %s | %s\n", __FUNCTION__, httpCode, https.errorToString(httpCode).c_str(), https.getString().c_str());
      }
      result = https.getString();
    });

    return result;
  }
}