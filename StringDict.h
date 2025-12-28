#pragma once

#include "Utils.h"
#include <string_view>
#include <vector>
#include <string>

namespace SA
{
  class StringDict
  {
    public:
      StringDict(std::string_view str, std::string_view keyCommonPart = "");

      template<class T>
      std::optional<T> GetValue(const char* key) const
      {
          const std::size_t keyHash = std::hash<std::string_view>{}(key);
          for (const Entry& entry : m_values)
          {
            if (entry.m_key == keyHash)
            {
              return SA::Utils::ParseValue<T>(entry.m_value.c_str());
            }
          }

          return {};
      }

    private:
      struct Entry
      {
        std::string m_value;
        std::size_t m_key;
      };

      std::vector<Entry> m_values;
  };
}