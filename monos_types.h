#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <useful_header.h>
#include <DSP28x_Project.h>



namespace monos{


union int128_t{
	int16_t word[8];
	int32_t dword[4];
	int64_t qword[2];
};

union uint128_t{
	uint16_t word[8];
	uint32_t dword[4];
	uint64_t qword[2];
};



typedef int (*TCallbackFunction)(void *);
typedef CFunctor1<void*, int> TCallbackFunctor;


struct TCallbackContext{
	TCallbackFunctor meth;
	void *arg;
	int result;
};



template<void (*init)(), void (*deinit)()>
class CStaticInvoker{
public:

	typedef void (*TInvokeFunction)();

	CStaticInvoker()	{init();}
	~CStaticInvoker()	{deinit();}
};





};//namespace os
