#include "CallbackCallee.h"
CallbackCallee::CallbackCallee(){}

CallbackCallee::~CallbackCallee(){}

void CallbackCallee::cb_func(){
		write_text_to_log_file("GENERIC CALLBACK");
	}