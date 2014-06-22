#pragma once
#include "GUIObject.h"

class Label : public GUIObject
{
public:
	Label(void);
	Label(std::string pStr, sf::Vector2f pPosition);
	~Label(void);

private:
	sf::Font m_font;
};

