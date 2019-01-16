#pragma once

#include "monos_types.h"



namespace monos {



class CEvent {
private:

public:

	CEvent();
	~CEvent();

	//! Post event to system
	void post(uint32_t event);
	//! Wait for events combination
	bool wait();

};



} /* namespace os */





