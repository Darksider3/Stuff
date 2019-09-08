/*
 * @TODO: GUI with 1 box to enter the question, 4 buttons: Ask, clear(the box), play again, quit
 */
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
    answers.emplace_back("The feelings are overcoming me! *sleeps*");
    answers.emplace_back("There is no posssible outcome that would be good!");
    answers.emplace_back("You will find your greatest love! DO IT!");
    answers.emplace_back("Dont let your dreams be memes!");
    answers.emplace_back("0w0");
    answers.emplace_back("I see something bad happening!");
    answers.emplace_back("Possibly, but maybe you should check your dog first!");
    answers.emplace_back("NO! Or... i mean... maybe?");
    answers.emplace_back("Think more about that.");
    answers.emplace_back("UwU");
    answers.emplace_back("Nice try! But doesnt work.");
    answers.emplace_back("Nah.");
    answers.emplace_back("Meh.");
    answers.emplace_back("Yay");
    answers.emplace_back("Yepp");
    answers.emplace_back("Nay");
    answers.emplace_back("Hereby i shall call out my gut feelings: We're done!");
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
