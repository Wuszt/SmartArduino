#include "StringDict.h"
#include <MapCollector.hpp>
#include "Arduino.h"

namespace SA
{
  int32_t FindIndexOfValue(const std::vector<std::string>& values, const char* value)
  {
    for (int index = 0; index < values.size(); ++index)
    {
      if (values[index] == value)
      {
        return index;
      }
    }

    return -1;
  }

  StringDict::StringDict(std::string_view str, std::string_view keyCommonPart)
  {
    {
      auto func = [&](std::string key, std::string value)
      {
        const size_t idx = keyCommonPart.empty() ? 0 : key.find(keyCommonPart.begin(), 0, keyCommonPart.length());
        if (idx != std::string::npos)
        {
          const std::string_view keyStr(key.begin() + idx + keyCommonPart.length(), key.end());
          const std::size_t keyHash = std::hash<std::string_view>{}(keyStr);

          int32_t index = FindIndexOfValue(m_values, value.c_str());
          if (index == -1)
          {
            index = m_values.size();
            m_values.emplace_back(std::move(value));
          }
          m_keys.push_back({static_cast<int32_t>(keyHash), index});
        }
        return false;
      };

      MapCollector parser(func);
      for (char c : str)
      {
        parser.parse(c);
      }
    }
    m_values.shrink_to_fit();
  }
}