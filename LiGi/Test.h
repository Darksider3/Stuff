#pragma once
#ifndef TEST_H
#define TEST_H
#include "Assertions.h"
#include "LinkedList.h"
#include "Singleton.h"
#include <string>

namespace Li
{

struct TestCase : public Li::LLNode<TestCase>
{
  std::string name;
  std::string descr;
  std::string errorDesc;
  std::string category {"default"};
  bool(*func)(TestCase*);
  bool run {false};
  bool success {false};
  short error;

  void setFunc(bool(*function)(TestCase *))
  {
    func = *function;
  }
};


class Test : public Li::LinkedList<TestCase>, public Li::Singleton<Test>
{
  friend Li::Singleton<Test>; // REQUIRED
public:

  inline static void mark_runned(TestCase *m)
  {
    m->success = true;
    m->run = true;
  }

  void exec();
  virtual bool on_all(bool(*foo)(TestCase *p));
  virtual std::string errors();
private:
  // Singleton pattern: Disables any relevant constructor
  // already through  Singleton-Class
  //Test() {}
  // FIXME: Need this because i hesitate yet to declare it in there in singleton
  //Test(const Test&);
  //Test &operator=(const Test&);
};
}
#endif // TEST_H
