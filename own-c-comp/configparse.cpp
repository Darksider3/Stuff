#include <iostream>
#include <memory>

void tre()
{
  char *bla = new char[5];
  bla[0] = 'H';
  bla[1] = 'a';
  bla[2] = 'l';
  bla[3] = 'l';
  bla[4] = 'o';
  std::cout << bla;
}

class Config
{
  public:
  std::unique_ptr<std::unique_ptr<int[]>[]> table;
  void test()
  {
    table = std::make_unique<std::unique_ptr<int[]>[]>(3);
    for(int i = 0; i < 3; i++)
    {
      std::unique_ptr<int[]> tmp = std::make_unique<int[]>(5);
      tmp[0]=0;
      tmp[1]=1;
      tmp[2]=2;
      tmp[3]=3;
      tmp[4]=4;
      table[i]=std::move(tmp);
    }

    for(int i=0; i < 3; i++)
    {
      for(int j=0; j < 5; j++)
      {
        std::cout << table[i][j] << " ";
      }
      std::cout << '\n';
    }
  }
};

int main()
{
  Config bla;
  bla.test();
  tre();

  std::cout << "Hi! \n";
  return 0;
}
