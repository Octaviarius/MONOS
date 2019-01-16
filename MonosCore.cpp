#include "MonosCore.h"
#include "Hwi.h"
#include "Swi.h"
#include "Task.h"
#include "Timer.h"


#define NUM_CORES	1
#define EXTERN_CLOCK	(20*1000000UL)
#define SYSTEM_CPU_MULT	15






namespace monos{




float CMonosCore::multiplicity = 0;
float CMonosCore::input_clock = EXTERN_CLOCK;
float CMonosCore::cpu_freq = 0;
float CMonosCore::cpu_rate = 0;
float CMonosCore:: hs_mult = 0;
float CMonosCore::ls_mult = 0;
CMonosCore::TStaticCtrl CMonosCore::static_ctrl = {0};
uint128_t CMonosCore::counter;


int CMonosCore::timer1_handler(void *arg){
	CAtomic atom(true);
	counter.dword[0] = ~CpuTimer1Regs.TIM.all;
	counter.dword[1]++;
	CpuTimer1Regs.TCR.bit.TIF = 1;
	return 0;
}










CMonosCore::CMonosCore() :
		timer1(TCallbackFunctor(timer1_handler), NULL, 13)
{
	counter.qword[0] = 0;
	counter.qword[1] = 0;
}


CMonosCore::~CMonosCore(){

}



int CMonosCore::Cores(){
	return NUM_CORES;
}


uint16_t CMonosCore::PartID(){
	return DevEmuRegs.PARTID.all;
}

float CMonosCore::Freq(){
	return cpu_freq;
}


float CMonosCore::Rate(){
	return cpu_rate;
}


float CMonosCore::InputClock(){
	return EXTERN_CLOCK;
}


float CMonosCore::HSBFreq(){
	return hs_mult * cpu_freq;
}


float CMonosCore::LSBFreq(){
	return ls_mult * cpu_freq;
}


void CMonosCore::setHSBMultiplicity(float value){
	if(value < 1.0 / 62.0 || value > 1.0)
		return ;

	EALLOW;
	if(value == 1.0)
		SysCtrlRegs.HISPCP.bit.HSPCLK = 0;
	else
		SysCtrlRegs.HISPCP.bit.HSPCLK = 0.5 / value;
	EDIS;

	if(SysCtrlRegs.HISPCP.bit.HSPCLK == 0)
		hs_mult = 1;
	else
		hs_mult = 0.5 / (float)SysCtrlRegs.HISPCP.bit.HSPCLK;

}


float CMonosCore::HSBMultiplicity(){
	return hs_mult;
}


void CMonosCore::setLSBMultiplicity(float value){
	if(value < 1.0 / 14.0 || value > 1.0)
		return;

	EALLOW;
	if(value == 1.0)
		SysCtrlRegs.LOSPCP.bit.LSPCLK = 0;
	else
		SysCtrlRegs.LOSPCP.bit.LSPCLK = 0.5 / value;
	EDIS;

	if(SysCtrlRegs.LOSPCP.bit.LSPCLK == 0)
		ls_mult = 1;
	else
		ls_mult = 0.5 / (float)SysCtrlRegs.LOSPCP.bit.LSPCLK;

}


float CMonosCore::LSBMultiplicity(){
	return ls_mult;
}

/*!
 * \brief Запускаю CPU PLL (на максимальную частоту).
 * Вызывается с параметром 15 .
 */
void CMonosCore::setMultiplicity(float value){
	float last_mult = multiplicity;

	if(value < 0.125 || value > 31)
		return;
	else if(value >= 0.125 && value < 4){
		InitPll(8 * value - 1, 0);
		multiplicity = (int)(value * 8) / 8.0;
	}else if(value >= 4 && value < 8){
		InitPll(4 * value - 1, 1);
		multiplicity = (int)(value * 4) / 4.0;
	}else if(value >= 8 && value < 16){
		InitPll(2 * value - 1, 2);       // <=== (29, 2)
		multiplicity = (int)(value * 2) / 2.0;
	}else if(value >= 16 && value < 32){
		InitPll(value, 3);
		multiplicity = (int)(value * 1) / 1.0;
	}

	CpuTimer0Regs.PRD.all = uint32_t(float(CpuTimer0Regs.PRD.all) * (multiplicity / last_mult));
	cpu_freq = input_clock * multiplicity;
	cpu_rate = 1.0 / cpu_freq;
}


float CMonosCore::Multiplicity(){
	return multiplicity;
}






void CMonosCore::getTimestamp(uint64_t &stamp){
	CAtomic atom(true);

	if(CpuTimer1Regs.TCR.bit.TIF == 0){
		((uint32_t*)&stamp)[0] = ~CpuTimer1Regs.TIM.all;
		if(CpuTimer1Regs.TCR.bit.TIF == 0){
			((uint32_t*)&stamp)[1] = counter.dword[1];
		}else{
			((uint32_t*)&stamp)[0] = ~CpuTimer1Regs.TIM.all;
			((uint32_t*)&stamp)[1] = ++counter.dword[1];
			CpuTimer1Regs.TCR.bit.TIF = 1;
		}
	}else{
		((uint32_t*)&stamp)[0] = ~CpuTimer1Regs.TIM.all;
		((uint32_t*)&stamp)[1] = ++counter.dword[1];
		CpuTimer1Regs.TCR.bit.TIF = 1;
	};
	
}





void CMonosCore::init(){
	if(static_ctrl.bit.inited)
		return;

	DisableDog();
	InitPieCtrl();

	multiplicity = float(SysCtrlRegs.PLLCR.bit.DIV + 1) / float(1<<(3 - SysCtrlRegs.PLLSTS.bit.DIVSEL)) ;
	cpu_freq = input_clock * multiplicity;
	cpu_rate = 1.0 / cpu_freq;
	hs_mult = (SysCtrlRegs.HISPCP.bit.HSPCLK == 0) ? 1.0 : 1.0 / (float)(SysCtrlRegs.HISPCP.bit.HSPCLK * 2);
	ls_mult = (SysCtrlRegs.LOSPCP.bit.LSPCLK == 0) ? 1.0 : 1.0 / (float)(SysCtrlRegs.LOSPCP.bit.LSPCLK * 2);

	counter.qword[0] = 0;
	counter.qword[1] = 0;

	CpuTimer1Regs.PRD.all = (uint32_t)-1;
	CpuTimer1Regs.TCR.bit.TSS = 0;
	CpuTimer1Regs.TCR.bit.TIE = 1;

	EALLOW;
	SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1;
	IER |= M_INT13;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
	EDIS;

	timer1.setEnabled();
	static_ctrl.bit.inited = true;
}





void CMonosCore::deinit(){
	if(static_ctrl.bit.inited){
		DRTM;
		DINT;
	}
}







void CMonosCore::start(){
	static_ctrl.bit.started = true;

	CHwi::setEnabledGlobalInterrupts(true);
	ERTM;

	//main cycle
	while(static_ctrl.bit.started){
		CSwi::stepModule();
		CTask::stepModule();
		CTimer::stepModule();
	}
	CHwi::setEnabledGlobalInterrupts(false);
}


void CMonosCore::stop(){
	static_ctrl.bit.started = false;
}








};//namespace monos




