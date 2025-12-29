#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include "TVModeDetector.h"
#include "Utils.h"
#include "Config.h"
#include "Logger.h"
#include "SteamBootstrapper.h"
#include <NimBleKeyboard.h>

WiFiMulti WiFiMulti;
SA::TVModeDetector tvModeDetector;
std::unique_ptr<SA::Logger> logger;
std::unique_ptr<SA::SteamBootstrapper> steamBootstrapper;

void setup() 
{
  SA::Utils::InitializeSerial();
  WiFiMulti = SA::Utils::InitializeWiFi();
  SA::Utils::InitializeClock();

  std::shared_ptr<BleKeyboard> keyboard = std::make_shared<BleKeyboard>("SmartArduino", "SmartArduino", 100);
  keyboard->begin();
  logger = std::make_unique<SA::Logger>(keyboard);
  steamBootstrapper = std::make_unique<SA::SteamBootstrapper>(keyboard);
}

void loop() 
{
  const uint32_t startTime = micros();
  {
    NetworkClientSecure client;
    client.setCACert(SA::Config::c_smartThingsRootCA);

    String authToken = SA::Utils::GetToken(client);
    tvModeDetector.Update(client, authToken.c_str());
    logger->Update(client, authToken.c_str());
    steamBootstrapper->Update(client, authToken.c_str());

    // esp_sleep_enable_timer_wakeup(5 * 1000000ULL);
    // esp_light_sleep_start();
  }
  Serial.printf("Remaining memory=%u, Largest memory block=%u\n", ESP.getFreeHeap(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

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
