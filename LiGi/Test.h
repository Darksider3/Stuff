#pragma once
#ifndef TEST_H
#define TEST_H
#include "Assertions.h"
#include "LinkedList.h"
#include <string>
#include <memory>
#include <iostream>

namespace Li
{

struct ErrorCase
{
  int errorCode = 0; // 0 -> not yet done
  std::string errorDesc;
};

struct TestCase : public Li::LLNode<TestCase>
{
  bool (*foo)(ErrorCase *);
  std::string descr;
  std::string category {"default"};
  bool success {false}; // true, false, 2->forced true
  bool run {false};
  ErrorCase error;
};

//@todo: Test suite!
// It should be able to run every test, and produce a result-screen out of it
// nothing heavy, probably something with the Assertion.h
class Tests : public Li::LinkedList<TestCase>
{

public:
  void exec()
  {
    if(head() == tail())
    {
      head()->foo(&head()->error);
    }
    for(auto it = begin(); it != end(); ++it)
    {
      bool ret = it.current->foo(&it.current->error);
      if(ret)
        it.current->success = true;
      else
        it.current->success = false;
    }
  }
  void mark_runned(TestCase *m)
  {
    m->success = 2;
    m->run = true;
  }

  bool on_all(bool(*foo)(TestCase *p))
  {
    if(head() == tail())
    {
      foo(head());
    }
    for( auto it = begin(); it != end(); ++it)
    {
      foo(it.current);
    }
    return true;
  }
};
}
#endif // TEST_H
