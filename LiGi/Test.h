#pragma once
#ifndef TEST_H
#define TEST_H
#include "Assertions.h"
#include "LinkedList.h"
#include <string>
#include <memory>

struct TestCase : public Li::LLNode<TestCase>
{
  void (*foo)();
  std::string descr;
  std::string category = "default";
  short success = false; // true, false, 2->forced true
  bool run = false;
};

//@todo: Test suite!
// It should be able to run every test, and produce a result-screen out of it
// nothing heavy, probably something with the Assertion.h
class Tests : Li::LinkedList<TestCase>
{
  void run_all(){}
  void mark_runned(TestCase *m)
  {
    m->success = 2;
    m->run = true;
  }

  bool on_all(bool(*foo)(TestCase *p))
  {
    for( auto it = begin(); it != end(); ++it)
    {
      bool ret = foo(it.current);
      if(!ret)
      {
        return false;
      }
    }
    return true;
  }
};

#endif // TEST_H
