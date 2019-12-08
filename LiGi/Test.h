#pragma once
#ifndef TEST_H
#define TEST_H
#include "Assertions.h"
#include "LinkedList.h"
#include <string>

namespace Li
{

struct TestCase : public Li::LLNode<TestCase>
{
  std::string name;
  std::string descr;
  std::string errorDesc;
  std::string category {"default"};
  bool (*func)(TestCase *);
  bool run {false};
  bool success {false};
  short error;

  void setFunc(bool(*function)(TestCase *))
  {
    func = function;
  }

  bool exec()
  {
    return func(this);
  }
};

//@todo: Test suite!
// It should be able to run every test, and produce a result-screen out of it
// nothing heavy, probably something with the Assertion.h
class Test : public Li::LinkedList<TestCase>
{

public:
  inline void exec()
  {
    if(head() == tail())
    {
      head()->exec();
    }
    for(auto node = head(); node != nullptr; node=node->next())
    {
      bool ret = node->exec();
      if(ret)
        node->success = true;
      else
        node->success = false;
    }
  }

  inline void mark_runned(TestCase *m)
  {
    m->success = true;
    m->run = true;
  }

  std::string errors();
  bool on_all(bool(*foo)(TestCase *p));
};


std::string Test::errors()
{
  std::string ret {""};
  std::string sizestr = std::to_string(size());
  size_t failed = 0;

  for(auto b: *this)
  {
    if(!b.success)
    {
      ++failed;
      ret.append("TEST CASE: ");
      ret.append(b.name);
      ret.append("\nError Code: ");
      ret.append(std::to_string(b.error));
      ret.append(", Description: ");
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
  for( auto it = begin(); it != end(); ++it)
  {
    foo(it.cur());
  }
  return true;
}

}
#endif // TEST_H
