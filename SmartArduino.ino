#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include "TVModeDetector.h"
#include "Utils.h"
#include "Config.h"
#include "Logger.h"
#include "SteamBootstrapper.h"
#include "SABleKeyboard.h"
#include "SmartThingsUtils.h"
#include "KeyboardTracker.h"
#include "SmartThingsWorkerInterface.h"

WiFiMulti WiFiMulti;
std::unique_ptr<SA::TVModeDetector> tvModeDetector;
std::unique_ptr<SA::Logger> logger;
std::unique_ptr<SA::SteamBootstrapper> steamBootstrapper;
std::unique_ptr<SA::KeyboardTracker> keyboardTracker;

std::vector<std::unique_ptr<SA::ISmartThingsWorker>> m_smartThingsWorkers;

void setup() 
{
  SA::Utils::InitializeSerial();
  WiFiMulti = SA::Utils::InitializeWiFi();
  SA::Utils::InitializeClock();

  m_smartThingsWorkers.push_back(std::make_unique<SA::TVModeDetector>());

  std::shared_ptr<SA::SABleKeyboard> keyboard = std::make_shared<SA::SABleKeyboard>("SmartArduino", "SmartArduino", 100);
  keyboard->begin();

  m_smartThingsWorkers.push_back(std::make_unique<SA::Logger>(keyboard));
  m_smartThingsWorkers.push_back(std::make_unique<SA::SteamBootstrapper>(keyboard));
  m_smartThingsWorkers.push_back(std::make_unique<SA::KeyboardTracker>(keyboard));
}

void loop() 
{
  const uint32_t startTime = micros();
  {
    NetworkClientSecure client;
    client.setCACert(SA::Config::c_smartThingsRootCA);

    String authToken = SA::Utils::GetToken(client);

    for (const auto& worker : m_smartThingsWorkers)
    {
      worker->Update(client, authToken.c_str());
    }

    // esp_sleep_enable_timer_wakeup(5 * 1000000ULL);
    // esp_light_sleep_start();
  }
  //Serial.printf("Remaining memory=%u, Largest memory block=%u\n", ESP.getFreeHeap(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

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
