#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include "TVModeDetector.h"
#include "Utils.h"
#include "Config.h"
#include "Logger.h"
#include "SteamBootstrapper.h"
#include "SABleKeyboard.h"
#include "KeyboardTracker.h"
#include "SmartThingsWorkerInterface.h"
#include "LaMetricRoomTemperatureProvider.h"
#include "LaMetricBluetoothController.h"
#include "UpdateManager.h"
#include "SmartThingsManager.h"

WiFiMulti WiFiMulti;

SA::TVModeDetector g_tvModeDetector;
SA::LaMetricRoomTemperatureProvider g_laMetricRoomTemperatureProvider;
SA::LaMetricBluetoothController g_laMetricBluetoothController;

SA::SABleKeyboard g_keyboard{"SmartArduino", "SmartArduino", 100};
SA::Logger g_logger{g_keyboard};
SA::SteamBootstrapper g_steamBootStrapper{g_keyboard};
SA::KeyboardTracker g_keyboardTracker{g_keyboard};

void setup() 
{
  SA::Utils::InitializeSerial();
  WiFiMulti = SA::Utils::InitializeWiFi();
  SA::Utils::InitializeClock();

  g_keyboard.begin();
}

void loop() 
{
  const uint32_t startTime = micros();
  {
    SA::UpdateManager::Get().Update();
    SA::SmartThingsManager::Get().PostUpdate();
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

  if (sleepingTime > 0)
  {
    Serial.printf("Sleeping for %fs...\n", sleepingTime / 1000.0f);
    delay(sleepingTime);
  }
}
