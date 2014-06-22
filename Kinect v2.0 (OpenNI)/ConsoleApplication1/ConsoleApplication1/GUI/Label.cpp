#include "Label.h"

Label::Label(void)
{
}

Label::~Label(void)
{
}

Label::Label(std::string pStr, sf::Vector2f pPosition) : GUIObject(pStr, pPosition)
{
	if(!m_font.loadFromFile("resources/sansation.ttf")) printf("echec font load\n");
	m_texte.setFont(m_font);
	m_texte.setPosition(pPosition);
	m_texte.setColor(sf::Color::Black);
}