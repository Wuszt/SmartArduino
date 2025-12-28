#include "StringDict.h"
#include <MapCollector.hpp>

namespace SA
{
  StringDict::StringDict(std::string_view str, std::string_view keyCommonPart)
  {
    MapCollector parser;
    for (char c : str)
    {
      parser.parse(c);
    }

    for (auto& it : parser.getValues())
    {
      const size_t idx = keyCommonPart.empty() ? 0 : it.first.find(keyCommonPart.begin(), 0, keyCommonPart.length());
      if (idx != std::string::npos)
      {
        const std::string_view keyStr(it.first.begin() + idx + keyCommonPart.length(), it.first.end());
        const std::size_t keyHash = std::hash<std::string_view>{}(keyStr);
        m_values.push_back(Entry{std::move(it.second), keyHash});
      }
    }

    m_values.shrink_to_fit();
  }
}