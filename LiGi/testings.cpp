#include <iostream>
#include "LinkedList.h"
#include "SinglyLinkedList.h"
#include "Test.h"
#include "stack.h"
#include <cstring>

constexpr int LISTSIZES = 50;

int lltester(int argc, char **argv);

struct ExampleSingly : Li::SingleNode<ExampleSingly>
{
  int data = 1;

  bool cmpr(ExampleSingly &other)
  {
    return (other.data != data);
  }

  bool bigger(ExampleSingly &other)
  {
    return (other.data < data);
  }
};

struct ExampleNode : Li::LLNode<ExampleNode>
{
  int data = 0;
};

bool testStack(Li::TestCase *mua)
{
  bool returner = true;
  mua->name = "StackTest";
  std::unique_ptr<ExampleNode[]> Nodes(new ExampleNode[LISTSIZES]);
  Li::Stack<ExampleNode> La{};

  for(size_t i = 0; i != LISTSIZES; ++i)
  {
    Nodes[i].data = static_cast<int>(i);
    La.pushd(&Nodes[i]);
  }
  if(La.height() != LISTSIZES)
  {
    mua->error = 2;
    mua->errorDesc = "Stack heights doesn't match! Height: " + std::to_string(La.height());
    returner = false;
  }

  ExampleNode B = *La.top();

  if(B.data != LISTSIZES-1)
  {
    mua->error = 4;
    mua->errorDesc = "Stacks inner doesn't match! Data: " + std::to_string(B.data);
    returner = false;
  }
  La.popd();
  B = *La.top();

  if(B.data != LISTSIZES-2)
  {
    mua->error = 4;
    mua->errorDesc = "Stacks inner doesn't match! Data: " + std::to_string(B.data);
    returner = false;
  }
  return returner;
}

bool testList(Li::TestCase *me)
{
  bool returner = true;
  me->name = "ListTest";
  std::unique_ptr<ExampleNode[]> Nodes(new ExampleNode[LISTSIZES]);
  Li::LinkedList<ExampleNode> L;
  for(size_t i = 0; i != LISTSIZES; ++i)
  {
    Nodes[i].data = static_cast<int>(i);
    L.append(&Nodes[i]);
  }

  if(L.size() != LISTSIZES)
  {
    me->error = 2;
    me->errorDesc = "Sizes don't match!";
    returner = false;
  }

  return returner;
}

bool testSingly(Li::TestCase *me)
{
  bool returner = true;
  me->name = "ListTest";
  std::unique_ptr<ExampleSingly[]> Nodes(new ExampleSingly[LISTSIZES]);
  Li::SinglyLinkedList<ExampleSingly> L;
  for(size_t i = 0; i != LISTSIZES; ++i)
  {
    Nodes[i].data = static_cast<int>(i);
    L.append(&Nodes[i]);
  }

  if(L.size() != LISTSIZES)
  {
    me->error = 2;
    me->errorDesc = "Sizes don't match!";
    returner = false;
  }

  return returner;
}

bool testTheTest(Li::TestCase *me)
{
  me->name = "Tests the tests. Fails intentionally";
  me->error = 2;
  me->errorDesc = "Just a fucking example";
  return false;
}

bool testFrankenstein(Li::TestCase *me)
{
  me->name = std::string("Frankensteins monster! Fails intentionally");
  if(std::strcmp(me->category.c_str(), "hi") == 0)
  {
    me->error = 3;
    me->errorDesc = "Well, frankensteins monster just ate you!";
    return false;
  }
  return true;
}

int main(int argc, char** argv)
{
#ifdef TEST
  std::cout << "THIS MUST BE RUN WITHOUT OPTIMIZATION FLAGS! \n";
  Li::Test *hu = Li::Test::instance();
  std::shared_ptr<Li::TestCase> test1 = std::make_shared<Li::TestCase>();
  std::shared_ptr<Li::TestCase> test2 = std::make_shared<Li::TestCase>();
  std::shared_ptr<Li::TestCase> test3 = std::make_shared<Li::TestCase>();
  std::shared_ptr<Li::TestCase> test4 = std::make_shared<Li::TestCase>();
  std::shared_ptr<Li::TestCase> test5 = std::make_shared<Li::TestCase>();
  test1->func = *testTheTest;
  test2->func = *testList;
  test3->func = *testFrankenstein;
  test4->func = *testSingly;
  test5->func = *testStack;
  hu->append(test1.get());
  hu->append(test4.get());
  hu->append(test2.get());
#ifdef FRANKENSTEIN
  test3->category = "hi";
#endif
  hu->append(test3.get());
  hu->append(test5.get());
  hu->exec();
  std::cout << hu->errors();
#endif
  lltester(argc, argv);
}


int lltester(int argc, char **argv)
{
  size_t sizer;
  if(argc > 1)
  {
    sizer = std::stoul(argv[1]);
    std::cout << "GOT SIZE: " << sizer << "!!\n";
  }
  else
    sizer = LISTSIZES;


  std::unique_ptr<ExampleNode> Tester = std::make_unique<ExampleNode>();
  Li::LinkedList<ExampleNode> b;
  b.append(Tester.get());
  std::unique_ptr<ExampleNode[]> Inserter(new ExampleNode[sizer]);

  for(size_t i = 0; i != sizer; ++i)
  {
    Inserter[i].data = static_cast<int>(i);
    b.append(&Inserter[i]);
  }

  for(ExampleNode *t = b.head(); t; t = t->next())
  {
    std::cout << t->data << ", ";
  }

  std::cout << "Size: " << b.size() << "\n";
  for( auto it = b.begin(); it != b.end(); ++it)
  {
    std::cout << it->data << ", ";
  }
  //@TODO: Commit: LinkedList.h -> "Working iterator!!". main.cpp -> "implement tests"
  return 0;
}
