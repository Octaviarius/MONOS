#pragma once


#include "monos_types.h"
#include <list>


namespace monos{


class CMonosCore;



class CTask {
private:

	static std::list<CTask*> tasks;
	static CTask *current_task;
	static bool global_enabled;
	static uint32_t global_duty;

	static int stepModule();

	TCallbackContext callback_context;
	uint64_t timestamp;

	float duty;

	union{
		struct{
			uint16_t suspend : 1;
			uint16_t sleep : 1;
			uint16_t timer : 1;
			uint16_t semaphore : 1;
		}bit;
		uint16_t all;
	} mute;


public:

	enum ETaskStates{
		taskReady,
		taskExecution,
		taskWait
	};

	CTask(TCallbackFunctor method, void *arg = NULL, bool started = false, float duty = 50);
	//! Деструктор
	~CTask();


	//! Установка приоритета задачи
	void setDuty(float value);
	//! Приоритет задачи
	float Duty();

	//! Возвращает результат выполнения функции
	TCallbackContext CallbackContext();

	void suspend();
	void resume();
	void sleep(float secs);

	//! Включение планировщика задач
	static void enableSheduler();
	//! Выключение планировщика задач
	static void disableSheduler();

	//! Засыпание на время (мс)
	static void sleepTask(float secs);

	static void suspendTask();
	static void resumeTask();

	static CTask *currentTask();

	friend class CMonosCore;

private:

	ETaskStates task_state;

};




};

