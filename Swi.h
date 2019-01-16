/*!
 * \file Swi.hpp
 * \brief Обёртка над Swi-потоком - объектом ОС
 */
#pragma once


#include "monos_types.h"
#include <list>





namespace monos{

class CMonosCore;
class CHwi;



class CSwi{
private:

	static std::list<CSwi*> swis;
	static bool global_enabled;
	static bool inprocess;
	static bool swi_exist;


	static int stepModule();

	TCallbackContext callback_context;

	union{
		struct{
			bool triggered : 1;
		}bit;
		uint16_t all;
	}ctrl;

public:

	//! КОнструктор с инициализацией
	CSwi(TCallbackFunctor method, void *arg);

	//! Деструктор
	~CSwi();

	//! Принудительный запуск программного прерывания
	void post(void *arg = NULL);

	TCallbackContext CallbackContext();

	//! Глобальное выключение программных прерываний
	static void disableGlobalSoftInterrupts();
	//! Глобальное включение программных прерываний
	static void enableGlobalSoftInterrupts();

	friend class CMonosCore;
	friend class CHwi;
};



}
