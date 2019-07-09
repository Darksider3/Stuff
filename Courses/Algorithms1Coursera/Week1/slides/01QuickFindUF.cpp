#include <iostream>

//gogo
class QuickFindUF
{
private:
  int *id;
  size_t length = 0;

public:
  QuickFindUF(size_t N) : length{N}
  {
    // Fill the array
    id = new int[N];
    for(size_t i = 0; i < N; i++)
    {
      id[i] = i;
    }
  }
  bool connected(int p, int q)
  {
    // Are the ids equal?
    return (id[p] == id[q]);
  }
  
  void Union(int p, int q)
  {
    int pid = id[p];
    int qid = id[q];
    for (int i = 0; i < length; i++)
    {
      if (id[i] ==  pid) id[i] = qid;
    }
  }
  ~QuickFindUF()
  {
    delete[] id;
  }
};


int main()
{
  QuickFindUF test(15);
  test.Union(1, 2);
  test.Union(2, 3);
  test.Union(4, 5);
  test.Union(6, 7);
  test.Union(7, 8);
  test.Union(8, 9);
  test.Union(9, 10);
  if (test.connected(1, 2))
    std::cout <<  "1, 2 connected!" <<  std::endl;
  if (test.connected(2, 3))
    std::cout <<  "2, 3 connected!" <<  std::endl;
  if (test.connected(4, 5))
    std::cout <<  "4, 5 connected!" <<  std::endl;
  if (test.connected(6, 7))
    std::cout <<  "6, 7 connected!" <<  std::endl;
  if (test.connected(8, 9))
    std::cout <<  "8, 9 connected!" <<  std::endl;
  if (test.connected(7, 10))
    std::cout <<  "7, 10 connected!" <<  std::endl;
  return 0;
}
