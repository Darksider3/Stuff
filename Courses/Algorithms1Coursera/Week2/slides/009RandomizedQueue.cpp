#include <iostream>
#include <exception>
#include <bsd/stdlib.h>

#ifdef DEBUG
size_t allocCounter = 0;
size_t deallocCounter = 0;
#endif
 //g++ -Wall -g 009RandomizedQueue.cpp -o 009RandomizedQueue -lbsd
template <typename Item>
class RandomizedQueue
{
private:
//Somehow the first entry never gets filled
  Item *queue;
  size_t size;
  size_t index;
  
public:
  RandomizedQueue(Item f)
  {
    size = 1;
    index = 0;
    queue = new Item[1];
#ifdef DEBUG
    allocCounter++;
#endif
    enqueue(f);
  }
  
  bool isEmpty()
  {
    return size == 0;
  }
  
  size_t getsize()
  {
    return size;
  }
  
  void enqueue(Item item)
  {
    if(index >= size)
      addSize();
    queue[index] = item;
    index++;
  }
  
  Item get()
  {
    int ran = arc4random_uniform(index);
    Item ret = queue[ran];
    queue[ran] = queue[--index];
    queue[index] = 0;
    if(index < size / 4)
      removeSize();
    return ret;
  }

  void removeSize()
  {
    size /= 2;
    Item *newQueue = new Item[size];
    for(size_t i = 0; i < index; i++)
      newQueue[i] = queue[i];
    delete[] queue;
#ifdef DEBUG
    deallocCounter++;
#endif
    queue = newQueue;
  }
  void addSize()
  {
    size_t newsize = size * 2;
    Item *newQueue = new Item[newsize];
#ifdef DEBUG
    allocCounter++;
#endif
    for(size_t i = 0; i < index; i++)
    {
      newQueue[i] = queue[i];
    }
    size = newsize;
    delete[] queue;
#ifdef DEBUG
    deallocCounter++;
#endif
    queue = newQueue;
  }
  ~RandomizedQueue()
  {
    delete[] queue;
#ifdef DEBUG
    deallocCounter++;
#endif
  }
};

int main()
{
  RandomizedQueue<int> Test(5);
  Test.enqueue(1);
  Test.enqueue(2);  
  Test.enqueue(3);
  Test.enqueue(4);  
  Test.enqueue(6);
  Test.enqueue(7);
  Test.enqueue(8);
  Test.enqueue(9);
  Test.enqueue(10);
  Test.enqueue(11);
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;  
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << "hi." << std::endl;
#ifdef DEBUG
  std::cout << "Deallocations: " << deallocCounter++ << "; Allocations:  "<< allocCounter << std::endl;
#endif
}
