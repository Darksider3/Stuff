#include <iostream>

class DuckMoveBase
{
protected:
public:
  virtual float Move(){return 0;}
  virtual ~DuckMoveBase()= default;;
};

class DuckSoundBase
{
public:
  virtual void Sound(){}
  virtual ~DuckSoundBase()= default;;
};

class Duck
{
protected:
  DuckMoveBase *move;
  DuckSoundBase *sound;
  bool responsible = false;

public:
  Duck()
  {
    move = new DuckMoveBase();
    sound = new DuckSoundBase();
    responsible = true;
  }
  Duck(DuckMoveBase &m, DuckSoundBase &s)
  {
    move = &m;
    sound = &s;
  }
  ~Duck()
  {
    if(responsible)
    {
      delete move;
      delete sound;
    }
  }
};

class MallardSound : public DuckSoundBase
{
public:
  void Sound() override
  {
    std::cout << "Wääähhwääähhwäääh" << std::endl;
  }
};

class MallardMove : public DuckMoveBase
{
public:
  float Move() override
  {
    return 0.01; // its SLOW
  }
};

class Mallard : public Duck, MallardMove, MallardSound
{
public:
  Mallard(MallardMove m = MallardMove(), MallardSound s = MallardSound()) : Duck(m, s)
  {}

  void action()
  {
    this->Sound();
    std::cout << "Moved exactly " << this->Move() << " meters! \n";
    std::cout << "Did we actually allocate? ";
    if(responsible)
      std::cout << "Yepp";
    else
      std::cout << "Nope.";
  }
};

int main()
{
  Mallard First;
  First.action();
}
