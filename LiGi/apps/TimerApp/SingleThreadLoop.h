#ifndef LIGI_SINGLE_THREAD_LOOP
#define LIGI_SINGLE_THREAD_LOOP
#include <atomic>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

namespace App {
struct ShareThread {
};
struct HoldThread {
};
}

class SingleThreadLoop {
private:
    std::deque<std::function<void()>> funcQueue;
    std::mutex queueLock, statelock;
    std::atomic_bool& globalStop;
    std::unique_ptr<std::thread> M_Hold;

    uint64_t WaitTime = 15;

    enum anonym {
        idle,
        running
    } mState
        = idle;

    void threadingFunction()
    {
        while (!this->globalStop) {
            bool ss = true;
            {
                std::scoped_lock<std::mutex>(this->queueLock);
                ss = this->funcQueue.empty();
            }
            // @todo additional scope for scoped_lock
            if (!ss) {
                runFunc();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
        }
    }

protected:
public:
    explicit SingleThreadLoop(std::atomic_bool& GlobalStopVar)
        : globalStop(GlobalStopVar)
    {
    }

    template<typename FUNC, typename... Arguments>
    bool insert(FUNC f, Arguments&&... args)
    {
        {
            std::scoped_lock<std::mutex> l(this->queueLock);
            if (this->funcQueue.size() > 0)
                return false; // just one element allowed
            std::function<void()> Next = std::function<void()>(
                std::bind(f, std::forward<Arguments>(args)...));
            this->funcQueue.emplace_back(std::move(Next));
        }
        return true;
    }

    std::shared_ptr<std::thread> RunThread(App::ShareThread)
    {
        // std::ref(*this): We want the Thread to have our data here...
        std::shared_ptr<std::thread>&& MovingThread = std::make_shared<std::thread>(&SingleThreadLoop::threadingFunction, std::ref(*this));
        return std::move(MovingThread);
    }

    bool RunThread(App::HoldThread)
    {
        if (M_Hold)
            return false;
        M_Hold = std::make_unique<std::thread>(&SingleThreadLoop::threadingFunction, std::ref(*this));
        return true;
    }

    bool runFunc()
    {
        std::function<void()> RunFunc;
        {
            std::scoped_lock<std::mutex, std::mutex>(this->statelock, this->queueLock);
            if (this->funcQueue.size() < 1)
                return false;
            RunFunc = this->funcQueue.back();
            mState = running;
        }

        RunFunc();

        {
            std::scoped_lock<std::mutex, std::mutex>(this->statelock, this->queueLock);
            mState = idle;
            this->funcQueue.pop_back();
        }
        return true;
    }

    void printTypeLol()
    {
        for (auto& b : this->funcQueue)
            std::cout << typeid(b).name() << std::endl;
    }

    bool is_running()
    {
        std::scoped_lock<std::mutex>(this->statelock);
        return (this->mState == running);
    }

    void Stop()
    {
        globalStop = true;
    }

    ~SingleThreadLoop()
    {
        if (M_Hold)
            M_Hold->join();
    }
};

namespace App {
template<typename ClockT>
void delayedInsertion(SingleThreadLoop& obj, std::function<void()>& func, uint64_t delay = 5)

{
    while (obj.is_running())
        std::this_thread::sleep_for(ClockT(delay));
    obj.insert(func);
    return;
}
}
#endif
