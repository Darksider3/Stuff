#include <iostream>
#include <exception>

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
    if(index > size)
      addSize();
    queue[++index] = item;
  }
  
  Item get()
  {
    return queue[index--];
  }
  
  void addSize()
  {
    size_t newsize = size * 2;
    Item *newQueue = new Item[newsize];
    for(size_t i = 0; i < index; i++)
    {
      newQueue[i] = queue[i];
    }
    size = newsize;
    queue = newQueue;
  }
  ~RandomizedQueue()
  {
    delete[] queue;
  }
};

int main()
{
  RandomizedQueue<int> Test(0);
  Test.enqueue(1);
  Test.enqueue(2);
  std::cout <<  std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << "hi." << std::endl;
}
