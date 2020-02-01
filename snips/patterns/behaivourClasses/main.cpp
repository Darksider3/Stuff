#include <iostream>

class DuckMoveBase
{
protected:
public:
  virtual float Move(){return 0;}
  virtual ~DuckMoveBase(){};
};

class DuckSoundBase
{
public:
  virtual void Sound(){}
  virtual ~DuckSoundBase(){};
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
  Duck(DuckMoveBase *m, DuckSoundBase *s)
  {
    move = m;
    sound = s;
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
  void Sound()
  {
    std::cout << "Wääähhwääähhwäääh" << std::endl;
  }
};

class MallardMove : public DuckMoveBase
{
public:
  float Move()
  {
    return 0.01; // its SLOW
  }
};

class Mallard : public Duck, MallardMove, MallardSound
{
public:
  Mallard(){}
  void action()
  {
    this->Sound();
    std::cout << "Moved exactly " << this->Move() << " meters! \n";
  }
};

int main()
{
  Mallard First;
  First.action();
}
