#pragma once

#include "Utils.h"
#include <string_view>
#include <vector>
#include <string>
#include "Arduino.h"

namespace SA
{
  class StringDict
  {
    public:
      StringDict() = default;
      StringDict(std::string_view str, std::string_view keyCommonPart = "");

      template<class T>
      std::optional<T> GetValue(const char* key) const
      {
          const std::size_t keyHash = std::hash<std::string_view>{}(key);
          for (const Entry& entry : m_keys)
          {
            if (entry.m_keyHash == static_cast<int32_t>(keyHash))
            {
              return SA::Utils::ParseValue<T>(m_values[entry.m_valueIndex].c_str());
            }
          }

          Serial.println("no key");
          return {};
      }

    private:
      struct Entry
      {
        int32_t m_keyHash;
        int32_t m_valueIndex;
      };

      std::vector<Entry> m_keys;
      std::vector<std::string> m_values;
  };
}