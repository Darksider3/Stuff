#include <iostream>
#include <exception>
#include <bsd/stdlib.h>
#include <iterator>



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
    allocCounter+=1;
#endif
    enqueue(f);
  }
  
  bool isEmpty()
  {
    return size == 0;
  }
  
  size_t Size()
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
    size_t ran = arc4random_uniform(index);
    Item ret = queue[ran];
    //Set value of queueu[ran] to last(valid) queue[index] value and decrement by 1 before. So we manage to shrink the array effectivly every time we return an result by 1. 
    //Given queue[1]=123 and queue[index]=59, return queue[1] and set it to 59, invalidate it in your head and deallocate when needed.
    queue[ran] = queue[--index];
    // DO NOT CLEAR THE MEMORY - due to type constraints we cannot set values to 0 or nullptr, cuz' the type could be for example std::string or int, and int cant be set to nullptr and std::string to 0....
    if(index < size / 4)
      removeSize();
    return ret;
  }

  void removeSize()
  {
    size /= 2;
    Item *newQueue = new Item[size];
    for(size_t i = 0; i < index; i++)
    {
      newQueue[i] = queue[i];
    }
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
  

  /*
   class iterator
   {
    size_t current = 0;
    Flow: Distance
    
    size_t begin()
    size_t end()
    bool operator==(rhs)
    
   }
   */
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
  Test.enqueue(12);
  Test.enqueue(13);
  Test.enqueue(14);
  Test.enqueue(15);
  Test.enqueue(16);
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;  
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;  
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;  
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
    std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;
  std::cout << Test.get() << std::endl;  
  std::cout << Test.get() << std::endl;
  std::cout << "hi." << std::endl;
  
  RandomizedQueue<std::__cxx11::basic_string<char>> Tra("ra");
  std::string bla("Hiallo");
  std::string bla2("Hi2");
  std::string bla3("Hi3");
  std::string bla4("Hi4");
        
  Tra.enqueue(bla);
  Tra.enqueue(bla2);
  Tra.enqueue(bla3);
  Tra.enqueue(bla4);
  std::cout << Tra.get() << std::endl;
  std::cout << Tra.get() << std::endl;
  std::cout << Tra.get() << std::endl;
  std::cout << Tra.get() << std::endl;
#ifdef DEBUG
  std::cout << "Deallocations: " << deallocCounter++ << "; Allocations:  "<< allocCounter << std::endl;
#endif
}
