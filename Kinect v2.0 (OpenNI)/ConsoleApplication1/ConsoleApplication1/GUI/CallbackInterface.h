#pragma warning(disable: 4996)

#define SFML_STATIC

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>

#include "log.h"

class CallbackInterface{
private:

public:
	virtual void cb_func() = 0;

};