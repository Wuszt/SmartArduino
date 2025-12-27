#pragma once

#include "Utils.h"
#include <string_view>
#include <map>
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
          auto it = m_map.find(std::string(key));
          if (it != m_map.end())
          {
              return SA::Utils::ParseValue<T>(it->second.c_str());
          }

          return {};
      }

    private:
      std::map<std::string, std::string> m_map;
  };
}