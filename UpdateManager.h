#pragma once
#include <vector>

namespace SA
{
  class IUpdatable
  {
    public:
      IUpdatable();
      virtual ~IUpdatable();
      virtual void Update() = 0;
  };

  class UpdateManager
  {
  public:
    static UpdateManager& Get()
    {
      static UpdateManager s_instance;
      return s_instance;
    }

    void Update();
    void Register(IUpdatable& updatable);
    void Unregister(IUpdatable& updatable);

  private:
    std::vector<IUpdatable*> m_updatables;
  };
}