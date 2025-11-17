#pragma once

#include <iostream>
#include "IETThread.h"
#include "IExecutionEvent.h"
#include "TextureManager.h"
#include "IWorkerAction.h"

class LoadAssetThread : public IWorkerAction {
public:
	LoadAssetThread() {};
	~LoadAssetThread() {};

	LoadAssetThread(int _id, IExecutionEvent* callback);

private:
	int id = 0;
	//void run() override;
	void OnStartTask() override;
	IExecutionEvent* OnFinished;

};