#pragma once


#include "monos_types.h"
#include <list>






namespace monos {


class CMonosCore;



class CTimer {
private:

	static std::list<CTimer*> timers;

	static int stepModule();

	TCallbackContext callback_context;

	uint64_t timestamp;
	float period;

	union{
		struct{
			bool started : 1;
			bool oneshot : 1;
			bool constantly : 1;
		}bit;
		uint16_t all;
	}ctrl;


public:

	CTimer(TCallbackFunctor method, void *arg, float period);
	~CTimer();

	TCallbackContext CallbackContext();

	float Period();
	void setPeriod(float value);

	void start(bool oneshot = false, bool constantly = false);
	void stop();

	bool isStarted();
	bool isOneshot();
	bool isConstantly();

	friend class CMonosCore;
};



} /* namespace monos */

