#include <limits>
#include <algorithm>
#include "UpdateManager.h"
#include <Arduino.h>

namespace SA
{
  IUpdatable::IUpdatable()
  {
    UpdateManager::Get().Register(*this);
  }

  IUpdatable::~IUpdatable()
  {
    UpdateManager::Get().Unregister(*this);
  }

  void UpdateManager::Update()
  {
    std::vector<const char*> updatedNames;
    updatedNames.reserve(m_updatables.size());

    for (Entry& entry : m_updatables)
    {
      if (millis() < entry.m_nextUpdateTimestamp)
      {
        break;
      }

      entry.m_updatable->Update();
      updatedNames.push_back(entry.m_updatable->GetName());
      entry.m_nextUpdateTimestamp = millis() + entry.m_updatable->GetInterval();
    }

    std::sort(m_updatables.begin(), m_updatables.end(), [](const Entry& a, const Entry& b) 
    {
        return a.m_nextUpdateTimestamp < b.m_nextUpdateTimestamp;
    });

    Serial.print("Updated updatables: ");
    if (updatedNames.empty())
    {
      Serial.println("none");
    }
    else
    {
      for (size_t index = 0; index < updatedNames.size(); ++index)
      {
        if (index > 0)
        {
          Serial.print(", ");
        }

        Serial.print(updatedNames[index]);
      }

      Serial.println();
    }
  }

  void UpdateManager::Register(IUpdatable& updatable)
  {
    m_updatables.push_back({&updatable, millis() + random(10u * 1000, 120u * 1000)});
  }

  void UpdateManager::Unregister(IUpdatable& updatable)
  {
    m_updatables.erase(std::find_if(m_updatables.begin(), m_updatables.end(), [&](const Entry& entry) { return entry.m_updatable == &updatable; }));
  }

  unsigned long UpdateManager::GetNextUpdateTimestamp() const
  {
    if (m_updatables.empty())
    {
      return std::numeric_limits<unsigned long>::max();
    }

    return m_updatables[0].m_nextUpdateTimestamp;
  }
}