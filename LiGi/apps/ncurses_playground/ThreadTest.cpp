#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <deque>

namespace Li
{
namespace FuncModifiers
{
namespace Thread
{
  enum __ShareThread{} ShareThread; // Selector
  enum __HoldThread{} HoldThread;
}
}
}

class ThreadWrapper
{
private:
  std::deque<std::function<void()>> funcQueue;
  std::mutex queueLock, statelock;
  std::atomic_bool &globalStop;
  enum anonym
  {
    idle,
    running
  } mState = idle;

  void threadingFunction()
  {
    while(!this->globalStop)
    {
      // @todo additional scope for scoped_lock
      {
        std::scoped_lock<std::mutex>(this->queueLock);
        if(!this->funcQueue.empty())
        {
          runFunc();
        }
        else
        {
          std::cout << "idling" << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
      }
    }
  }
protected:
public:

  explicit ThreadWrapper(std::atomic_bool &GlobalStopVar) : globalStop(GlobalStopVar)
  {}

  template<typename FUNC, typename ... Arguments>
  bool insert(FUNC f, Arguments&&... args)
  {
    {
      std::scoped_lock<std::mutex> l(this->queueLock);
      if(this->funcQueue.size() > 0)
        return false; // just one element allowed
    }
    std::function<void()> Next = std::function<void()>(
                                   std::bind(f, std::forward<Arguments>(args)...));
    {
      std::scoped_lock<std::mutex>(this->queueLock);
      this->funcQueue.emplace_back(std::move(Next));
    }
    return true;
  }



  std::shared_ptr<std::thread> RunThread(Li::FuncModifiers::Thread::__ShareThread&)
  {
    // std::ref(*this): We want the Thread to have our data here...
    std::shared_ptr<std::thread> &&MovingThread = std::make_shared<std::thread>(&ThreadWrapper::threadingFunction, std::ref(*this));
    return std::move(MovingThread);
  }

  bool runFunc()
  {
    std::function<void()> RunFunc;
    {
      std::scoped_lock<std::mutex> (this->queueLock);
      if(this->funcQueue.size() < 1)
        return false;
      RunFunc = this->funcQueue.back();
    }
    {
      std::scoped_lock<std::mutex> (this->statelock);
      mState = running;
    }

    RunFunc();

    {
      std::scoped_lock<std::mutex> (this->statelock);
      mState = idle;
    }

    {
      std::scoped_lock<std::mutex> (this->queueLock);
      this->funcQueue.pop_back();
    }
    return true;
  }

  void printTypeLol()
  {
    for(auto &b: this->funcQueue)
      std::cout << typeid(b).name() << std::endl;
  }

  bool is_running()
  {
    std::scoped_lock<std::mutex>(this->statelock);
    return (this->mState == running);
  }

  ~ThreadWrapper()
  {
  }
};


int main()
{
  auto blafunc = [](){
    for(size_t i = 10; i != 0; --i)
    {
      std::cout << "hello world" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "exiting func" << std::endl;
  };

  std::atomic_bool Global = false;
  ThreadWrapper ThreaderWrapper(Global);
  std::shared_ptr<std::thread> ThreadCopy = ThreaderWrapper.RunThread(Li::FuncModifiers::Thread::ShareThread);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ThreaderWrapper.insert<void()>(blafunc);
  std::string asw = "";
  while(asw != "q")
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    if(!ThreaderWrapper.is_running())
    {
      ThreaderWrapper.insert<void()>(blafunc);
    }
  }
  Global = true;
  ThreadCopy->join();
}
