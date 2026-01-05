#pragma once
#include <functional>

class String;
class HTTPClient;
namespace SA::Utils
{
   void SendToLaMetric(const char* subUrl, std::function<void(HTTPClient&)> func);
   String PostToLaMetric(const char* subUrl, const char* body);
   String GetToLaMetric(const char* subUrl);
   String PutToLaMetric(const char* subUrl, const char* body);
}