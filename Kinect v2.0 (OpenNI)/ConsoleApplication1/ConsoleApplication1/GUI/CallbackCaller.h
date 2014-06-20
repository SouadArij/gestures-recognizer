#ifndef CALLBACKINTERFACE_H
#define CALLBACKINTERFACE_H
#include "CallbackInterface.h"
#endif

//Caller

class CallbackCaller{

private:

protected:
	CallbackInterface* m_cb;
	std::vector<CallbackInterface*>m_multiplecb;

public:
	void connect(CallbackInterface *cb){
		this->m_cb = cb;
	}
	void addMulConnection(CallbackInterface *cb){
		this->m_multiplecb.push_back(cb);
	}
	CallbackInterface* get_cb(){return this->m_cb;}
	std::vector<CallbackInterface*> getMulCb(){return this->m_multiplecb;}

};