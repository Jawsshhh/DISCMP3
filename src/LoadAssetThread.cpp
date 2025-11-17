#include "LoadAssetThread.h"

LoadAssetThread::LoadAssetThread(int _id, IExecutionEvent* callback)
{
	this->id = _id;
	this->OnFinished = callback;
}



void LoadAssetThread::OnStartTask()
{
	TextureManager::getInstance()->loadSingleStreamAsset(this->id);
	this->OnFinished->OnFinishedExecution();
}