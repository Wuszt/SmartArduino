#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include "TVModeDetector.h"
#include "Utils.h"
#include "Config.h"

WiFiMulti WiFiMulti;

void setup() 
{
  SA::Utils::InitializeSerial();
  WiFiMulti = SA::Utils::InitializeWiFi();
  SA::Utils::InitializeClock();
}

SA::TVModeDetector tvModeDetector;

void loop() 
{
  const uint32_t startTime = micros();
  NetworkClientSecure client;
  client.setCACert(SA::Config::c_smartThingsRootCA);
  {
    tvModeDetector.Update(client);
  }

  // esp_sleep_enable_timer_wakeup(5 * 1000000ULL);
  // esp_light_sleep_start();

  const uint32_t endTime = micros();
  const uint32_t elapsedTime = (endTime - startTime) / 1000u;
  const uint32_t targetDelay = 10000;

  uint32_t sleepingTime = 0u;
  if (elapsedTime < targetDelay)
  {
    sleepingTime = targetDelay - elapsedTime;
  }

  Serial.printf("Sleeping for %fs...\n", sleepingTime / 1000.0f);
  delay(sleepingTime);
}
