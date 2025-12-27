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
        m_map.emplace(std::string(it.first, idx + keyCommonPart.length()), std::move(it.second));
      }
    }
  }
}