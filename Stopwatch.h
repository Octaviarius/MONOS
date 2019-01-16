/*!
 * \file Hwi.hpp
 * \brief Обёртка над HWI - объектом ОС
 */

#pragma once


#include "monos_types.h"





namespace monos{



class CStopwatch{
private:
	uint64_t fixed_time;
public:

	CStopwatch();
	~CStopwatch();

	void stopWatch();
	float Time() const;

	static float64 globalTime();

};


};
