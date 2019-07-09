#include <iostream>

class QuickUnionLazy
{
private:
  int *id;
  int *size;
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
    size = new int[N];
    for(size_t i = 0; i < N; i++)
    {
      id[i] = i; // set up our array
      size[i] = 1;
    }
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
    if(i == j) return; // if root is the same, dont do anything
    //if the tree of P is smaller than the Q one, add Q to Ps tree
    if(size[i] < size[j]) {id[i] = j; size[j] += size[i]; } 
    //else add P to Qs tree
    else {id[j] = i; size[i] += size[j];}
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
  else
    std::cout << "yay, 15, 10 are not connected!" << std::endl;
  return 0;
}
