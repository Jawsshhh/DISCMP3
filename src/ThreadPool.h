#pragma once

#include "WorkerThread.h"
#include "IWorkerAction.h"

#include <queue>
#include <vector>
#include <mutex>

class ThreadPool : public IFinishedTask {
public:
    ThreadPool(int _workerCount);
    ~ThreadPool();

    void StartScheduling();
    void StopScheduling();

    void ScheduleTask(IWorkerAction* _task);

private:
    void OnFinishedTask(int id) override;

    int workerCount;
    std::vector<WorkerThread*> workers;
    std::queue<int> availableWorkerIds;
    std::queue<IWorkerAction*> pendingTasks;

    std::mutex poolMutex;
    bool isRunning;

    void processPendingTasks();
};