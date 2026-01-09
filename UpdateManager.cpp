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
    for (Entry& entry : m_updatables)
    {
      if (millis() < entry.m_nextUpdateTimestamp)
      {
        break;
      }

      entry.m_updatable->Update();
      entry.m_nextUpdateTimestamp = millis() + entry.m_updatable->GetInterval();
    }

    std::sort(m_updatables.begin(), m_updatables.end(), [](const Entry& a, const Entry& b) 
    {
        return a.m_nextUpdateTimestamp < b.m_nextUpdateTimestamp;
    });
  }

  void UpdateManager::Register(IUpdatable& updatable)
  {
    m_updatables.push_back({&updatable, 0u});
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