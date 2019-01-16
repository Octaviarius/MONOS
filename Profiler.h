#pragma once


#include "monos_types.h"
#include "MonosCore.h"


namespace monos{


class CProfiler{
private:

	uint64_t start_moment;
	uint64_t stop_moment;
	uint64_t last_period;
	bool started : 1;

public:

	CProfiler()		{
		last_period = start_moment = stop_moment = 0;
		started = false;
	}

	~CProfiler() {

	};

	void start(){
		started = true;
		CMonosCore::getTimestamp(start_moment);
	}

	void stop(){
		CMonosCore::getTimestamp(stop_moment);
		last_period = stop_moment - start_moment;
		started = false;
	}

	uint64_t Ticks(){
		if(started)
			return 0;
		return last_period;
	}

	float Period(){
		if(started)
			return 0.0;
		return float(last_period) * CMonosCore::Freq();
	}

	bool isStarted(){
		return started;
	}

};




};
