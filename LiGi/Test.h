#pragma once
#ifndef TEST_H
#define TEST_H
#include "Assertions.h"
#include "LinkedList.h"
#include <string>
#include "Singleton.h"

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


class Test : public Li::LinkedList<TestCase>, public Li::Singleton<Test>
{
  friend Li::Singleton<Test>;
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

  virtual bool on_all(bool(*foo)(TestCase *p));
  std::string errors();

private:
  // Singleton pattern: Disables any relevant constructor
  // already through  Singleton-Class
  //Test() {}
  // FIXME: Need this because i hesitate yet to declare it in there in singleton
  Test(const Test&);
  Test &operator=(const Test&);
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
  for( auto it = begin(); it != end(); ++it)
  {
    foo(&*it);
  }
  return true;
}

}
#endif // TEST_H
