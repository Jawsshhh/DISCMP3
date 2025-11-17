#include "ThreadPool.h"

ThreadPool::ThreadPool(int _workerCount)
    : workerCount(_workerCount), isRunning(false)
{
    workers.resize(workerCount);

    // Create and start all worker threads
    for (int i = 0; i < workerCount; i++)
    {
        workers[i] = new WorkerThread(i, this);
        availableWorkerIds.push(i);
    }
}

ThreadPool::~ThreadPool()
{
    StopScheduling();

    for (auto worker : workers)
    {
        delete worker;
    }
}

void ThreadPool::StartScheduling()
{
    isRunning = true;

    // Start all worker threads
    for (auto worker : workers)
    {
        worker->Start();
    }
}

void ThreadPool::StopScheduling()
{
    isRunning = false;

    // Stop all worker threads
    for (auto worker : workers)
    {
        worker->Stop();
    }
}

void ThreadPool::ScheduleTask(IWorkerAction* _task)
{
    {
        std::lock_guard<std::mutex> lock(poolMutex);
        pendingTasks.push(_task);
    }

    processPendingTasks();
}

void ThreadPool::processPendingTasks()
{
    std::lock_guard<std::mutex> lock(poolMutex);

    // Assign tasks to available workers
    while (!pendingTasks.empty() && !availableWorkerIds.empty())
    {
        int workerId = availableWorkerIds.front();
        availableWorkerIds.pop();

        IWorkerAction* task = pendingTasks.front();
        pendingTasks.pop();

        workers[workerId]->AssignTask(task);
    }
}

void ThreadPool::OnFinishedTask(int id)
{
    {
        std::lock_guard<std::mutex> lock(poolMutex);
        availableWorkerIds.push(id);
    }

    // Try to process more pending tasks
    processPendingTasks();
}