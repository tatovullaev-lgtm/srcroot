#ifndef BMNABSTRACTTHREAD_H
#define BMNABSTRACTTHREAD_H 1

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>

// #include "Rtypes.h"
// #include "TStopwatch.h"

#pragma GCC system_header

using CClock = std::chrono::steady_clock;
using CPoint = std::chrono::time_point<CClock>;
using CDuration = std::chrono::duration<double>;

class BmnAbstractThread
{
  public:
    void Run()
    {
        if (calcStatistics) {
            std::lock_guard<std::mutex> statLock(statVarAccess);
            curThreadCount++;
        }
        std::unique_lock<std::mutex> dataLock(dataAccess);
        while (true) {
            if (calcStatistics) {
                std::lock_guard<std::mutex> statLock(statVarAccess);
                curThreadWaiting++;
            }
            if (measureTime) {   // timer->Start();
                ctime = CClock::now();
            }
            proceed.wait(dataLock, [this] { return dataReady; });
            if (calcStatistics) {
                std::lock_guard<std::mutex> statLock(statVarAccess);
                curThreadWaiting--;
            }
            if (measureTime) {
                //                timer->Stop();
                //                time1 += timer->RealTime();
                ctime1 += CClock::now() - ctime;
            }
            if (taskId == -1) {
                dataReady = false;
                break;
            }
            if (measureTime) {   // timer->Start();
                ctime = CClock::now();
            }

            Calculate();

            if (measureTime) {
                //                timer->Stop();
                //                time2 += timer->RealTime();
                ctime2 += CClock::now() - ctime;
            }
            // adding this thread back to the queue
            dataReady = false;
            std::lock_guard<std::mutex> queueLock(*queueAccess);
            threadQueue->push(threadId);
            queueWait->notify_one();
        }
        if (calcStatistics) {
            std::lock_guard<std::mutex> statLock(statVarAccess);
            curThreadCount--;
        }
    }

    int32_t GetRunningThreads()
    {
        if (calcStatistics)
            return curThreadCount;
        else
            return -1;
    }

    int32_t GetWaitingThreads()
    {
        if (calcStatistics)
            return curThreadWaiting;
        else
            return -1;
    }

    //    double GetWaitingTime(){
    //        if (measureTime) return time1;
    //        else return -1;
    //    }
    //
    //    double GetCalculationTime(){
    //        if (measureTime) return time2;
    //        else return -1;
    //    }

    CDuration GetWaitingCTime() { return ctime1; }

    CDuration GetCalculationCTime() { return ctime2; }

    int32_t GetId() { return threadId; }

    virtual void Terminate()
    {
        std::cout << "Abstract method is called!\n";
    }   // only call if the thread is not calculating

    void Execute()
    {
        std::lock_guard<std::mutex> dataLock(dataAccess);
        dataReady = true;
        proceed.notify_one();
    }

    void CancelReady()
    {
        std::lock_guard<std::mutex> queueLock(*queueAccess);
        threadQueue->push(threadId);
        queueWait->notify_one();
    }

    virtual void SetInitData(...) { std::cout << "Abstract method is called!\n"; }
    virtual void SetData(...) { std::cout << "Abstract method is called!\n"; }

  protected:
    BmnAbstractThread(std::queue<int32_t>* threadQueue,
                      std::mutex* queueAccess,
                      std::condition_variable* queueWait,
                      int32_t id,
                      bool measureTime = false)
        : threadQueue(threadQueue)
        , queueAccess(queueAccess)
        , queueWait(queueWait)
        , dataReady(false)
        , threadId(id)
        , taskId(-2)
        , measureTime(measureTime)   //, time1(0.0), time2(0.0)
    {
        //        timer = new TStopwatch();
    }

    ~BmnAbstractThread()
    {
        //        delete timer;
    }

    int32_t threadId;   // this thread id
    int32_t taskId;     // current task id

    virtual void Calculate() { std::cout << "Abstract method is called!\n"; }

  private:
    // variables for keeping track of current threads
    static const bool calcStatistics;
    static std::mutex statVarAccess;
    static int32_t curThreadWaiting;
    static int32_t curThreadCount;

    const bool measureTime;   // tells whether to use the timer
    //    TStopwatch* timer; //timer for evaluating performance
    CPoint ctime;
    //    double time1, time2; //variables to keep track of the performance 1-waiting time, 2-calculation time
    CDuration ctime1, ctime2;   // variables to keep track of the performance 1-waiting time, 2-calculation time

    std::condition_variable* queueWait;   // makes the ThreadManager class wait in case the queue is empty
    std::queue<int32_t>* threadQueue;     // queue from the ThreadManager class
    std::mutex* queueAccess;              // access to read/write into the queue

    std::condition_variable proceed;   // makes the thread wait untill its data is set
    std::mutex dataAccess;             // mutex for accessing the data of this thread
    bool dataReady;

    //    ClassDef(BmnAbstractThread, 1);
};

#endif