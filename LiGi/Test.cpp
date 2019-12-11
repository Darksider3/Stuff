#include "Test.h"

#include <iostream>

namespace Li
{
void Test::exec()
{
  if(head() == tail())
  {
    head()->func(head());
  }
  for(auto &node: *this)
  {
    bool ret = node.func(&node);
    node.success = ret;
  }
}

std::string Test::errors()
{
  std::string ret;
  std::string sizestr = std::to_string(size());
  size_t failed = 0;

  for(auto const &b: *this)
  {
    if(!b.success)
    {
      ++failed;
      ret.append("TEST CASE: ");
      ret.append(b.name);
      ret.append("\nError Code: ");
      ret.append(std::to_string(b.error));
      ret.append(";\nDescription: ");
      ret.append(b.errorDesc);
      ret.append("\n-----\n\n");
    }
  }

  if(failed > 0)
  {
    ret = "Results: " + std::to_string(failed) + "/" + sizestr + " failed\n" + ret;
  }
  else if (failed == 0)
  {
    ret = sizestr + "/" + sizestr + " succeeded, no errors!\n\n";
  }

  return ret;
}

bool Test::on_all(bool(*foo)(TestCase *p))
{
  if(head() == tail())
  {
    foo(head());
  }
  for(auto &p: *this)
  {
    foo(&p);
  }
  return true;
}

}
