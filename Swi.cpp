/*!
 * \file Swi.cpp
 * \brief Обёртка над Swi-потоком - объектом ОС
 */

#include "Swi.h"
#include "Atomic.h"


namespace monos{


std::list<CSwi*> CSwi::swis;
bool CSwi::global_enabled = true;
bool CSwi::inprocess = false;
bool CSwi::swi_exist = false;






CSwi::CSwi(TCallbackFunctor method, void *arg){
	CAtomic atom(true);
	swis.push_back(this);

	callback_context.result = 0;
	callback_context.arg = arg;
	callback_context.meth = method;

	ctrl.all = 0;
}



CSwi::~CSwi(){
	CAtomic atom(true);
	swis.remove(this);
}




void CSwi::post(void *arg){
	CAtomic atom(true);
	callback_context.arg = arg;
	ctrl.bit.triggered = true;
	swi_exist = true;
}


TCallbackContext CSwi::CallbackContext(){
	return callback_context;
}





void CSwi::disableGlobalSoftInterrupts(){
	global_enabled = false;
}


void CSwi::enableGlobalSoftInterrupts(){
	global_enabled = true;
}





int CSwi::stepModule(){
	if(!global_enabled)
		return 1;

	CAtomic::assign(inprocess, true);

	std::list<CSwi*>::iterator swi;

	for(int i=swis.size(); i>0; i--){
		CSwi *s = *swi;

		if(s->ctrl.bit.triggered){
			s->callback_context.result = s->callback_context.meth(s->callback_context.arg);
			s->ctrl.bit.triggered = false;
		}

		swi++;
	}

	CAtomic::assign(swi_exist, false);
	CAtomic::assign(inprocess, false);
	return 0;
}





};

