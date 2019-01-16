/*!
 * \file Semaphore.hpp
 * \brief Обёртка над Semaphore - объектом ОС
 */

#pragma once



#include "monos_types.h"
#include "Atomic.h"



namespace monos{



/*!
 * @brief Семафор
 */
class CSemaphore {
private:
	int acquires_count;
	int acquires_max;

public:

	//! Конструктор
	CSemaphore(int n = 1);
	//! Деструктор
	~CSemaphore();

	//! Отпускание семафора
	void release();
	//! Сброс семафора
	void reset(int count = 0);

	//! текущее количество захватов
	int available();

	//! Захват
	bool acquire();


};








}
