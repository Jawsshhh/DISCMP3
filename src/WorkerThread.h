#pragma once

#include "IWorkerAction.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class IFinishedTask {
public:
    virtual void OnFinishedTask(int id) = 0;
};

class WorkerThread {
public:
    WorkerThread(int _id, IFinishedTask* _onDone);
    ~WorkerThread();

    void Start();  // Start the worker thread (call once)
    void AssignTask(IWorkerAction* _task);
    void Stop();   // Stop the worker thread
    int GetId() { return id; }

private:
    void run();

    int id;
    IFinishedTask* onDone;
    IWorkerAction* task;

    std::thread workerThread;
    std::mutex taskMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::atomic<bool> hasTask;
};