#pragma once

#include "monos_types.h"

#include "Hwi.h"



namespace monos{


class CMonosCore {
private:

	union TStaticCtrl{
		struct{
			bool inited : 1;
			bool started : 1;
		} bit;
		uint16_t all;
	};

	static float multiplicity;
	static float input_clock;
	static float cpu_freq;
	static float cpu_rate;
	static float hs_mult;
	static float ls_mult;

	static TStaticCtrl static_ctrl;

	static uint128_t counter;

	static int timer1_handler(void *arg);

	CHwi timer1;


public:


	CMonosCore();
	~CMonosCore();

	void init();
	void deinit();

	void start();
	void stop();


	static int Cores();

	static float Freq();
	static float Rate();
	static float InputClock();

	static float HSBFreq();
	static float LSBFreq();

	static float HSBMultiplicity();
	static float LSBMultiplicity();
	static float Multiplicity();

	static uint16_t PartID();

	static void setMultiplicity(float value);
	static void setLSBMultiplicity(float value);
	static void setHSBMultiplicity(float value);

	static void getTimestamp(uint64_t &stamp);


};





};//namespace monos






