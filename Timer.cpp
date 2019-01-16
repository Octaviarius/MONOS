#include "Timer.h"
#include "MonosCore.h"
#include "Atomic.h"



namespace monos {


std::list<CTimer*> CTimer::timers;





CTimer::CTimer(TCallbackFunctor method, void *arg, float period){
	CAtomic atom(true);
	timers.push_back(this);

	timestamp = 0;

	this->period = period;
	callback_context.result = 0;
	callback_context.arg = arg;
	callback_context.meth = method;

	ctrl.all = 0;
}

CTimer::~CTimer(){
	timers.remove(this);
}

TCallbackContext CTimer::CallbackContext(){
	return callback_context;
}



float CTimer::Period(){
	return period;
}


void CTimer::setPeriod(float value){
	period = value;
}



void CTimer::start(bool oneshot, bool constantly){
	if(ctrl.bit.started)	return;

	CAtomic atom(true);
	uint64_t ts;
	CMonosCore::getTimestamp(ts);
	timestamp = ts + uint64_t(CMonosCore::Freq() * period);
	ctrl.bit.started = true;
	ctrl.bit.oneshot = oneshot;
	ctrl.bit.constantly = constantly;
}


void CTimer::stop(){
	ctrl.bit.started = false;
}

bool CTimer::isStarted(){
	return ctrl.bit.started;
}

bool CTimer::isOneshot(){
	return ctrl.bit.oneshot;
}

bool CTimer::isConstantly(){
	return ctrl.bit.constantly;
}



int CTimer::stepModule(){
	std::list<CTimer*>::iterator timer = timers.begin();

	uint64_t ts, adder;
	CMonosCore::getTimestamp(ts);

	for(int i=timers.size(); i>0; i--){
		CTimer *tmr = *timer;

		if( (tmr->ctrl.bit.started) && (tmr->timestamp < ts) ){
			tmr->callback_context.result = tmr->callback_context.meth(tmr->callback_context.arg);
			if(tmr->ctrl.bit.oneshot)
				tmr->ctrl.bit.started = false;
			else{
				adder = uint64_t(CMonosCore::Freq() * tmr->period);
				if(tmr->ctrl.bit.constantly){
					tmr->timestamp += adder;
					if(tmr->timestamp <= ts)
						tmr->timestamp = ts + adder;
				}else
					tmr->timestamp = ts + adder;
			}
		}

		timer++;
	}

	return 0;
}






} /* namespace monos */
