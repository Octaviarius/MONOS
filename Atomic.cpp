/*!
 * \file Mutex.hpp
 * \brief Обёртка над Mutex - объектом ОС
 */
#pragma once


#include "Atomic.h"
#include "Hwi.h"


namespace monos{



void CAtomic::lock() {
	if(locked == false){
		lastInt = CHwi::disableGlobalInterrupts();
		locked = true;
	}
}


void CAtomic::unlock() {
	if(locked){
		locked = false;
		CHwi::setEnabledGlobalInterrupts(lastInt);
	}
}



};//namespace monos
