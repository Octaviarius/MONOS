/*!
 * \file Mutex.hpp
 * \brief Обёртка над Mutex - объектом ОС
 */
#pragma once


#include "monos_types.h"



namespace monos{




/*!
 * @brief Atomic control unit. Control mono access operations
 *
 */
class CAtomic{
private:
	bool lastInt : 1;
	bool locked : 1;

public:

	//! Constructor with autostart
	CAtomic(bool is_lock = false)	{ locked = false; if(is_lock) this->lock(); else locked = false;}
	//! Descturtor
	~CAtomic()						{ unlock(); }

	//! Start atomic block
	void lock();
	//! Stop atomic block
	void unlock();

	//! Atomic assign value to variable
	template<class T>
	static void assign(T &variable, const T &value);

	//! Atomic swap two variables
	template<class T>
	static void swap(T &variable1, T &variable2);

	//! Atomic get value
	template<class T>
	static T getVal(const T &variable);
};






template<class T>
void CAtomic::assign(T &variable, const T &value)	{
	CAtomic atom(true);
	variable = value;
}


template<class T>
void CAtomic::swap(T &variable1, T &variable2)	{
	CAtomic atom(true);
	T tmp = variable1;
	variable1 = variable2;
	variable2 = tmp;
}

template<class T>
T CAtomic::getVal(const T &variable) {
	CAtomic atom(true);
	T tmp = variable;
	return tmp;
}






};//namespace monos
