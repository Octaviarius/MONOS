#include "Stopwatch.h"
#include "MonosCore.h"




namespace monos{



CStopwatch::CStopwatch(){
	fixed_time = 0;
}


CStopwatch::~CStopwatch(){

}



void CStopwatch::stopWatch(){
	CMonosCore::getTimestamp(fixed_time);
}


float CStopwatch::Time() const{
	uint64_t ts;
	CMonosCore::getTimestamp(ts);
	return float(ts - fixed_time) * CMonosCore::Freq();
}



float64 CStopwatch::globalTime(){
	uint64_t time;
	CMonosCore::getTimestamp(time);
	return float64(time) * CMonosCore::Freq();
}





};
