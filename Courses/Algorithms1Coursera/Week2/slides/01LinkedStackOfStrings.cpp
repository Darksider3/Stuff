#include <iostream>

class LinkedStackOfStrings
{
protected:
  class Node
  {
  public:
    std::string item;
    Node *next;
  };
  Node *first = nullptr;
public:
  bool isEmpty()
  {
    return first == nullptr;
  }
  
  void push(std::string item)
  {
    Node *oldfirst = first;
    first = new Node();
    first->item = item;
    first->next = oldfirst;
  }
  
  std::string pop()
  {
    std::string item = first->item;
    Node *next = first->next;
    delete first;
    first = next;
    return item;
  }
  
  ~LinkedStackOfStrings()
  {
    struct Node *cur, *tmp;
    cur = this->first;
    while(cur != nullptr)
    {
      tmp = cur->next;
      delete (cur);
      cur = tmp;
      std::cout << "deallocate!" << std::endl;
    }
  }
};

int main()
{
  LinkedStackOfStrings stack;
  std::string s = "hallo welt";
  stack.push(s);
  stack.push("Nein!");
  std::cout << stack.pop() << std::endl;
}
