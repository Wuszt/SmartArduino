namespace SA::Internal
{
  void PrintAssert(const char* reason);
}

#define ASSERT(cond) \
  do { \
    if (!(cond)) { \
      SA::Internal::PrintAssert( #cond ); \
      while (true) {} \
    } \
  } while (0)

#define VERIFY(cond) ASSERT(cond)