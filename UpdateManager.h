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
      virtual unsigned long GetInterval() const = 0;
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

    unsigned long GetNextUpdateTimestamp() const;

  private:
    struct Entry
    {
      IUpdatable* m_updatable;
      unsigned long m_nextUpdateTimestamp = 0u;
    };

    std::vector<Entry> m_updatables;
  };
}