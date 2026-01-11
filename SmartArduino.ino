#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include "TVModeDetector.h"
#include "Utils.h"
#include "Config.h"
#include "Logger.h"
#include "SteamBootstrapper.h"
#include "SABleKeyboard.h"
#include "KeyboardTracker.h"
#include "LaMetricRoomTemperatureProvider.h"
#include "LaMetricBluetoothController.h"
#include "UpdateManager.h"
#include "SmartThingsManager.h"
#include "LaMetricManager.h"
#include "RequestsServer.h"
#include "OvertimeTracker.h"
#include "LaMetricAirQualityProvider.h"

WiFiMulti WiFiMulti;

SA::TVModeDetector g_tvModeDetector;
SA::LaMetricRoomTemperatureProvider g_laMetricRoomTemperatureProvider;
SA::LaMetricBluetoothController g_laMetricBluetoothController;

SA::SABleKeyboard g_keyboard{"SmartArduino", "SmartArduino", 100};
SA::Logger g_logger{g_keyboard};
SA::SteamBootstrapper g_steamBootStrapper{g_keyboard};
SA::KeyboardTracker g_keyboardTracker{g_keyboard};

SA::RequestsServer g_server;
SA::OvertimeTracker g_overtimeTracker(g_server);

SA::LaMetricAirQualityProvider g_airQualityProvider;

void setup() 
{
  SA::Utils::InitializeSerial();
  WiFiMulti = SA::Utils::InitializeWiFi();
  SA::Utils::InitializeClock();

  g_keyboard.begin();
  g_server.Init();
}

void loop() 
{
  SA::UpdateManager::Get().Update();
  SA::SmartThingsManager::Get().PostUpdate();
  SA::LaMetricManager::Get().PostUpdate();
  //Serial.printf("Remaining memory=%u, Largest memory block=%u\n", ESP.getFreeHeap(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  const unsigned long nextUpdateTimestamp = SA::UpdateManager::Get().GetNextUpdateTimestamp();
  if (nextUpdateTimestamp > millis())
  {
    const unsigned long delayDuration =  nextUpdateTimestamp - millis();
    Serial.printf("Sleeping for %fs...\n", delayDuration / 1000.0f);
    delay(delayDuration);
  }
}
