/*!
 * \file Thread.cpp
 * \brief Обёртка над Task-потоком - объектом ОС
 */
#include "Task.h"
#include "Atomic.h"
#include "MonosCore.h"


namespace monos{


std::list<CTask*> CTask::tasks;
CTask *CTask::current_task = NULL;
bool CTask::global_enabled = true;
uint32_t CTask::global_duty = 0;



CTask::CTask(TCallbackFunctor method, void *arg, bool started, float duty){
	CAtomic atom(true);

	tasks.push_back(this);

	timestamp = 0;

	mute.all = 0;

	callback_context.arg = arg;
	callback_context.result = 0;
	callback_context.meth = method;

	//this->duty = 0;

	setDuty(duty);

	mute.bit.suspend = !started;

	if(mute.all == 0)	task_state = taskReady;
	else				task_state = taskWait;

}


CTask::~CTask(){
	CAtomic atom(true);
	tasks.remove(this);
	global_duty -= duty;
}










void CTask::setDuty(float value){
	CAtomic atom(true);
	value = CONSTRAIN(value, 0.0, 100.0);
	duty = value;
}


float CTask::Duty(){
	return duty;
}






TCallbackContext CTask::CallbackContext(){
	return callback_context;
}






void CTask::suspend(){
	CAtomic atom(true);
	mute.bit.suspend = 1;
	if(mute.all == 0)	task_state = taskReady;
	else				task_state = taskWait;
}


void CTask::resume(){
	CAtomic atom(true);
	mute.bit.suspend = 0;
	if(mute.all == 0)	task_state = taskReady;
	else				task_state = taskWait;
}


void CTask::sleep(float secs){
	CAtomic atom(true);
	uint64_t ts;
	CMonosCore::getTimestamp(ts);
	timestamp = ts + uint64_t(CMonosCore::Freq() * secs);
	mute.bit.sleep = 1;
	if(mute.all == 0)	task_state = taskReady;
	else				task_state = taskWait;
}













void CTask::enableSheduler(){
	global_enabled = true;
}


void CTask::disableSheduler(){
	global_enabled = false;
}








void CTask::sleepTask(float secs){
	CAtomic atom(true);
	uint64_t ts;
	CMonosCore::getTimestamp(ts);
	current_task->timestamp = ts + uint64_t(CMonosCore::Freq() * secs);
	current_task->mute.bit.sleep = 1;
	if(current_task->mute.all == 0)	current_task->task_state = taskReady;
	else							current_task->task_state = taskWait;
}



void CTask::suspendTask(){
	CAtomic atom(true);
	current_task->mute.bit.suspend = 1;
	if(current_task->mute.all == 0)	current_task->task_state = taskReady;
	else							current_task->task_state = taskWait;
}

void CTask::resumeTask(){
	CAtomic atom(true);
	current_task->mute.bit.suspend = 0;
	if(current_task->mute.all == 0)	current_task->task_state = taskReady;
	else							current_task->task_state = taskWait;
}

CTask *CTask::currentTask(){
	return current_task;
}










int CTask::stepModule(){
	if(!global_enabled)
		return 1;

	//get front task
	current_task = tasks.front();

	float duty = float(rand()) * (100.0 / float(RAND_MAX));

	if(current_task->duty <= duty){
		tasks.pop_front();
		tasks.push_back(current_task);
	}

	//handle sleep
	uint64_t ts;
	CMonosCore::getTimestamp(ts);
	if(ts >= current_task->timestamp)
		current_task->mute.bit.sleep = 0;

	if(current_task->mute.all == 0)	current_task->task_state = taskReady;
	else							current_task->task_state = taskWait;

	//exec function
	if(current_task->task_state == taskReady){
		current_task->task_state = taskExecution;
		current_task->timestamp = ts;
		current_task->callback_context.result = current_task->callback_context.meth(current_task->callback_context.arg);

		if(current_task->mute.all == 0)	current_task->task_state = taskReady;
		else							current_task->task_state = taskWait;
	}else
		return 2;

	return 0;
}





};
