#ifndef CALLBACKINTERFACE_H
#define CALLBACKINTERFACE_H
#include "CallbackInterface.h"
#endif

class CallbackCallee : public CallbackInterface{
private:

public:
	CallbackCallee();
	~CallbackCallee();
	void cb_func();

};