#include "Arduino.h"
namespace SA::Internal
{
  void PrintAssert(const char* reason)
  {
    Serial.printf("ASSERT FAILED: %s \n", reason);
  }
}