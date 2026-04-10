#ifndef BMNTHREADMANAGER_H
#define BMNTHREADMANAGER_H 1

#include <condition_variable>
#include <cstdarg>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
// BmnRoot
#include "BmnAbstractThread.h"
#include "BmnConverterThread.h"

#pragma GCC system_header

template<typename T>
class BmnThreadManager
{
  public:
    virtual ~BmnThreadManager() {}

    T* Add(Int_t threadType = 0)
    {
        BmnAbstractThread* tmp;
        switch (threadType) {
            case 0:
                tmp = new T(&threadQueue, &queueAccess, &queueWait, threads.size(), kTRUE);
                break;
        }
        threadQueue.push(threads.size());
        threads.push_back(tmp);
        std::thread t(&T::Run, tmp);
        t.detach();
        return (T*)tmp;
    }

    void Finish()
    {
        std::unique_lock<std::mutex> lk(queueAccess);
        queueWait.wait(lk, [this] { return threadQueue.size() == threads.size(); });
    }

    void Terminate()
    {
        for (int i = 0; i < threads.size(); i++)
            GetWaitingThread()->Terminate();
    }

    T* GetWaitingThread()
    {
        std::unique_lock<std::mutex> lk(queueAccess);
        queueWait.wait(lk, [this] { return !threadQueue.empty(); });
        Int_t threadId = threadQueue.front();
        threadQueue.pop();
        lk.unlock();
        return (T*)threads[threadId];
    }

    T* GetThread(int index) { return (T*)threads[index]; }

  private:
    vector<BmnAbstractThread*> threads;
    std::mutex queueAccess;
    std::condition_variable queueWait;
    queue<Int_t> threadQueue;

    ClassDef(BmnThreadManager<T>, 1);
};

#endif
