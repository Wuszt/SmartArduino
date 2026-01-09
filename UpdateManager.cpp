#include <algorithm>
#include "UpdateManager.h"

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
    for (IUpdatable* updatable : m_updatables)
    {
      updatable->Update();
    }
  }

    void UpdateManager::Register(IUpdatable& updatable)
    {
      m_updatables.push_back(&updatable);
    }

    void UpdateManager::Unregister(IUpdatable& updatable)
    {
      m_updatables.erase(std::find(m_updatables.begin(), m_updatables.end(), &updatable));
    }
}