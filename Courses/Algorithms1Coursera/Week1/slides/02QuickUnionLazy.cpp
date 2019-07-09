#include <iostream>

class QuickUnionLazy
{
private:
  int *id;
  size_t length;
  
  size_t root(size_t N)
  {
    while(N != id[N]) N = id[N];  // chase parent pointers until reaching the root, means follow every number till we found the exact same. When id(N)==N, we found our root! :)
    return N;
  }
  
public:
  QuickUnionLazy(size_t N)
  {
    length = N;
    id = new int[N];
    for(size_t i = 0; i < N; i++) id[i] = i; // set up our array
  }
  
  bool connected(size_t p, size_t q)
  {
    return root(p) == root(q); //check if the root is the same. Basically, if a root matches to different objects, it must be connected
  }
  
  void Union(size_t p, size_t q)
  {
      
    size_t i = root(p);
    size_t j = root(q);
    std::cout << "root(p) = " << i << ", root(q) = " << j << std::endl;
    id[j] = i; // Union the roots of p and q, means: Set the root of p to q's root, so p is a subtree of q now
  }
    
  void dump()
  {
    for(size_t i = 0; i < length; i++)
    {
      std::cout << "id[" << i << "] = " << id[i] << std::endl;
    }
  }
  ~QuickUnionLazy()
  {
    delete[] id;
  }
};


int main()
{
  QuickUnionLazy test(15);
  test.Union(1, 2);
  test.Union(2, 3);
  test.Union(4, 5);
  test.Union(6, 7);
  test.Union(7, 8);
  test.Union(8, 9);
  test.Union(9, 10);
    test.dump();
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
  if (test.connected(15, 10))
    std::cout <<  "15, 10 connected!" <<  std::endl;
  return 0;
}
