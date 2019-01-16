/*!
 * \file Hwi.hpp
 * \brief Обёртка над HWI - объектом ОС
 */

#pragma once


#include "monos_types.h"
#include "Atomic.h"
#include <list>


namespace monos{


class CMonosCore;


class CHwi{
private:

	struct TInterruptContext {
		CHwi *hwi_obj;
		uint64_t calls;
	};

	static std::list<CHwi*> hwis;
	static TInterruptContext interrupts[128];
	static uint64_t undef_counts;

	static void init();
	static void deinit();

	static CStaticInvoker<&CHwi::init, &CHwi::deinit> invoker;

	static interrupt void main_interrupt();
	static int undef_interrupt_handler(void *arg);

	TCallbackContext callback_context;
	int intNum;

public:


	CHwi(TCallbackFunctor meth, void *arg, int intNum);

	~CHwi();

	void setEnabled(bool en = true);

	void selfPost(void *arg = NULL);

	TCallbackContext CallbackContext();

	//! Включение глобальных прерываний
	static void setEnabledGlobalInterrupts(bool en);
	//! Выключение глобальных прерываний
	static bool disableGlobalInterrupts();
	//! Генерация заданного номера прерывания
	static void post(int intNum);

	friend class CMonosCore;

};


};
