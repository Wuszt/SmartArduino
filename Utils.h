#pragma once

#include <utility>
#include <optional>

#define CONCAT(X, Y) CONCAT_INNER(X, Y)
#define CONCAT_INNER(X,Y) X##Y

#define ON_SCOPE_EXIT( Action ) SA::Internal::ActionOnScopeExit CONCAT( _scopeExit, __COUNTER__ ) ( [&]() { Action } )

#define ASSERT(cond) \
  do { \
    if (!(cond)) { \
      Serial.print("ASSERT FAILED: "); \
      Serial.println(#cond); \
      while (true) { delay(100); } \
    } \
  } while (0)

class String;
class NetworkClientSecure;

namespace SA::Internal
{
	template< class T >
	class ActionOnScopeExit
	{
	public:
		ActionOnScopeExit( T func )
			: m_func( std::move( func ) )
		{}

		~ActionOnScopeExit()
		{
			m_func();
		}

	private:
		T m_func;
	};
}

class WiFiMulti;
namespace SA::Utils
{
  void InitializeSerial();

  WiFiMulti InitializeWiFi();

  void InitializeClock();

	bool StrCmpIgnoreCase(const char* l, const char* r);

	template<class T>
  std::optional<T> ParseValue(const char* str);

  template<>
  std::optional<const char*> ParseValue<const char*>(const char* str);

  template<>
  std::optional<int> ParseValue<int>(const char* str);

  template<>
  std::optional<bool> ParseValue<bool>(const char* str);

  String GetToken(NetworkClientSecure& client);

  void RefreshDevice(NetworkClientSecure& client, const char* token, const char* deviceID);
}