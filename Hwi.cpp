/*!
 * \file Hwi.cpp
 * \brief Обёртка над HWI - объектом ОС
 */
#include "Hwi.h"
#include "Swi.h"
#include <algorithm>


namespace monos{


std::list<CHwi*> CHwi::hwis;
CHwi::TInterruptContext CHwi::interrupts[128];
CStaticInvoker<&CHwi::init, &CHwi::deinit> CHwi::invoker;
uint64_t CHwi::undef_counts = 0;



void CHwi::init(){
	EALLOW;
	for(int i=0; i<128; i++){
		(&FORCE_CAST(uint32_t, PieVectTable))[i] = (uint32_t)&main_interrupt;
		interrupts[i].calls = 0;
		interrupts[i].hwi_obj = NULL;
	}
	EDIS;
}

void CHwi::deinit(){

}



interrupt void CHwi::main_interrupt(){
	int intNum = (PieCtrlRegs.PIECTRL.bit.PIEVECT & 0x7f);
	CHwi &intr = *interrupts[intNum].hwi_obj;
	interrupts[intNum].calls++;

	intr.callback_context.result = intr.callback_context.meth(intr.callback_context.arg);
	if(intNum >= 32){
		intNum -=32;
		PieCtrlRegs.PIEACK.all |= BIT(intNum>>3);
	}

	CAtomic atom(true);

	if( !CSwi::inprocess && CSwi::swi_exist){
		CSwi::inprocess = true;
		atom.unlock();
		CSwi::stepModule();
	}else
		atom.unlock();

}


int CHwi::undef_interrupt_handler(void *arg){
	undef_counts++;
	return 0;
}





CHwi::CHwi(TCallbackFunctor method, void *arg, int intNum){
	CAtomic atom(true);
	interrupts[intNum].hwi_obj = this;
	callback_context.result = 0;
	callback_context.meth = method;
	hwis.push_back(this);

}




CHwi::~CHwi(){
	CAtomic atom(true);
	setEnabled();
	interrupts[intNum].hwi_obj = NULL;
	hwis.remove(this);
}




TCallbackContext CHwi::CallbackContext(){
	CAtomic atom(true);
	return callback_context;
}



void CHwi::setEnabled(bool en){
	CAtomic atom(true);

	if(en){
		if(intNum < 32){

		}else{
			intNum -= 32;
			uint16_t *reg = &((uint16_t*)&PieCtrlRegs.PIEIER1.all)[2*(intNum>>3)];
			EALLOW;
			*reg |= BIT(intNum & 0x7);
			IER |= BIT(intNum >> 3);
			EDIS;
		}
	}else{

		if(intNum < 32){

		}else{
			intNum -= 32;
			uint16_t *reg = &((uint16_t*)&PieCtrlRegs.PIEIER1.all)[2*(intNum>>3)];
			EALLOW;
			*reg &= ~BIT(intNum & 0x7);
			IER &= ~BIT(intNum >> 3);
			EDIS;
		}
	}
}



void CHwi::selfPost(void *arg){
	callback_context.arg = arg;
	post(intNum);
}





void CHwi::setEnabledGlobalInterrupts(bool en){
	if(en)	EINT;
	else	DINT;
}


bool CHwi::disableGlobalInterrupts(){
	asm(
		" PUSH	ST1\n"
		" POP 	AL\n"
		" ANDB 	AL, #0x01\n"
	);
}



void CHwi::post(int intNum){
	if(intNum < 32){

	}else{
		intNum -= 32;
		uint16_t *reg = &((uint16_t*)&PieCtrlRegs.PIEIFR1.all)[2*(intNum>>3)];
		EALLOW;
		*reg |= BIT(intNum & 0x7);
		EDIS;
	}
}





};
