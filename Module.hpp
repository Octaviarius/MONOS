#pragma once


#include <useful_header.h>





class CModule {
public:

	enum Workstate{
		WorkstateInnactive = 0,		//!< Innactive state, object not working
		WorkstateActive,			//!< Active state, normal working object
		WorkstatePassive,			//!< Passive state ???
		WorkstateSuspended,			//!< Object working suspended
		WorkstateUserStateBegin = 16
	};


	enum Error{
		ErrorSuccess = 0,				//!< Success operation
		ErrorInitAlreadySet = -1,		//!< Init state already set, but previous access
		ErrorNotInitialized = -2,		//!< Object not initialized
		ErrorWorkstateAlreadySet = -3,	//!< Workstate is same
		ErrorWorkstateUnhandled = -4,	//!< Workstate isn't handled
		ErrorUndef = -255,				//!< Undef error code
		ErrorUserError = INT_MIN,		//!< User error code
	};

private:
	bool is_init;			//!< Flag indicate what object is initialized
	long last_error;		//!< Last error info
	Workstate work_state;	//!< Current Workstate
protected:
	//! Get last error, and value clear
	inline void setLastError(long err)				{last_error = err;}

	//! Init function handler of childrens
	virtual long _init()							{return ErrorSuccess;}
	//! Deinit function handler of childrens
	virtual long _deinit()							{return ErrorSuccess;}
	//! Init function handler of childrens
	virtual long _reinit()							{_deinit(); return _init();}
	//! Update function handler of childrens
	virtual long _update()							{return ErrorSuccess;}
	//! Reset function handler of childrens
	virtual long _reset()							{return ErrorSuccess;}
	//! setWorstate function handler of childrens
	virtual long _setWorkstate(Workstate state)	{return ErrorSuccess;}

public:

	//! Constructor
	CModule() {
		is_init = false;
		work_state = WorkstateInnactive;
		last_error = 0;
	}

	//! Destructor
	virtual ~CModule() {
		if(is_init == false)
			return;
		_setWorkstate(WorkstateInnactive);
		_deinit();
	}

	//! Init function. Basic of childrens. Handling state, initializating values, creating objects, etc...
	inline long init(){
		if(is_init == true)
			return last_error = ErrorInitAlreadySet;
		last_error = _init();
		if(last_error == ErrorSuccess)
			is_init = true;
		return last_error;
	}

	//! Deinit function. Basic of childrens. Handling state, deinitializating values, destruct objects, etc...
	inline long deinit(){
		if(is_init == false)
			return last_error = ErrorInitAlreadySet;
		if(work_state != WorkstateInnactive)
			_setWorkstate(WorkstateInnactive);
		last_error = _deinit();
		if(last_error == ErrorSuccess)
			is_init = false;
		return last_error;
	}

	inline long reinit(){
		return _reinit();
	}

	//! Update state handler
	long update(){
		if(is_init == false)
			return last_error = ErrorNotInitialized;
		return last_error = _update();
	};

	//! Reset state handler
	long reset(){
		if(is_init == false)
			return last_error = ErrorNotInitialized;
		return last_error = _reset();
	}

	//! Change inner state handler
	long setWorkstate(Workstate state){
		if(is_init == false)
			return last_error = ErrorNotInitialized;
		if(work_state == state)
			return last_error = ErrorWorkstateAlreadySet;
		last_error = _setWorkstate(state);
		if(last_error == ErrorSuccess)
			work_state = state;
		return last_error;
	}


	//! Get current state object
	inline Workstate workstate()	{return work_state;}
	//! Indicates that object initialized
	inline bool isInit()			{return is_init;}
	//! Get last error, and reset value
	inline long lastError()			{return last_error;}

};




