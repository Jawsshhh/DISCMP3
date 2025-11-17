#include "WorkerThread.h"

WorkerThread::WorkerThread(int _id, IFinishedTask* _onDone)
    : id(_id), onDone(_onDone), task(nullptr), running(false), hasTask(false)
{
}

WorkerThread::~WorkerThread()
{
    Stop();
}

void WorkerThread::Start()
{
    running = true;
    workerThread = std::thread(&WorkerThread::run, this);
}

void WorkerThread::Stop()
{
    running = false;
    cv.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void WorkerThread::AssignTask(IWorkerAction* _task)
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        task = _task;
        hasTask = true;
    }
    cv.notify_one();
}

void WorkerThread::run()
{
    while (running)
    {
        IWorkerAction* currentTask = nullptr;

        {
            std::unique_lock<std::mutex> lock(taskMutex);
            cv.wait(lock, [this] { return hasTask.load() || !running.load(); });

            if (!running) break;

            if (hasTask) {
                currentTask = task;
                task = nullptr;
                hasTask = false;
            }
        }

        if (currentTask != nullptr)
        {
            currentTask->OnStartTask();
            delete currentTask;  // Clean up the task after execution

            if (onDone != nullptr)
            {
                onDone->OnFinishedTask(id);
            }
        }
    }
}