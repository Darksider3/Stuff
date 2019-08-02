#include <iostream>
#include <bsd/stdlib.h> //arc4random
#include <vector> 
#include <chrono>
#include <thread> //sleep_for
//g++ -Wall -g magic_8_ball.cpp -o magic_8_ball -lbsd
class Answer
{
public:
  std::vector<std::string> answers;
  Answer()
  {
    answers.push_back("The feelings are overcoming me! *sleeps*");
    answers.push_back("There is no posssible outcome that would be good!");
    answers.push_back("You will find your greatest love! DO IT!");
    answers.push_back("Dont let your dreams be memes!");
    answers.push_back("0w0");
    answers.push_back("I see something bad happening!");
    answers.push_back("Possibly, but maybe you should check your dog first!");
    answers.push_back("NO! Or... i mean... maybe?");
    answers.push_back("Think more about that.");
    answers.push_back("UwU");
    answers.push_back("Nice try! But doesnt work.");
    answers.push_back("Nah.");
    answers.push_back("Meh.");
    answers.push_back("Yay");
    answers.push_back("Yepp");
    answers.push_back("Nay");
    answers.push_back("Hereby i shall call out my gut feelings: We're done!");
  }

  std::string get(size_t x)
  {
    return answers.at(x);
  }

  std::string get()
  {
    uint8_t Num = arc4random_uniform(answers.size());
    return answers.at(Num);
  }

  void think()
  {
    std::cout << "thinking about this matter.... \n";
    std::fflush(stdout);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  };

};


int main(int argc, char** argv)
{
  Answer Ans{};
  std::string inputbuf;
  bool exit=false;
  while(!exit)
  {
    std::cout << "Press q to quit, or enter your question!" << std::endl;
    std::cout << "Enter your Question: ";
    std::cin >> inputbuf;
    if(inputbuf == "q")
    {  
      exit = true;
      continue;
    }
    Ans.think();
    std::cout << Ans.get() << std::endl;
    inputbuf = "";
    std::cin.clear();
  }
}
