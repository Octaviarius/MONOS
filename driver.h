#pragma once


#include <stdint.h>
#include <stddef.h>
#include "Module.hpp"





namespace monos{

class CDriver : public CModule {
public:

	virtual int read(uint32_t address, void *data, size_t size) = 0;
	virtual int write(uint32_t address, const void *data, size_t size) = 0;
	virtual int command(uint32_t cmd, const void *arg) = 0;


};

};

