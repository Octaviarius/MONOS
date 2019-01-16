/*!
 * \file Semaphore.cpp
 * \brief Обёртка над Semaphore - объектом ОС
 */
#include "Semaphore.h"


namespace monos{




CSemaphore::CSemaphore(int n){
	acquires_max = n;
	acquires_count = 0;
}

CSemaphore::~CSemaphore() {

}




void CSemaphore::release(){
	CAtomic atomic(true);
	if(acquires_count > 0)
		acquires_count--;
}



int CSemaphore::available(){
	return acquires_count;
}

void CSemaphore::reset(int count){
	CAtomic atomic(true);
	acquires_count = CONSTRAIN(count, 0, acquires_max);
}


bool CSemaphore::acquire(){
	CAtomic atomic(true);
	if(acquires_count >= acquires_max)
		return false;
	else
		acquires_count++;
	return true;
}





};


