#include "GUIObject.h"


GUIObject::GUIObject(void)
{
}

GUIObject::~GUIObject(void)
{
}

GUIObject::GUIObject(std::string pStr, sf::Vector2f pPosition)
{
	m_position = pPosition;
	m_texte.setString(pStr);
}

void GUIObject::Attach(GUIObject* object)
{
	m_AttachedObject.push_back(object);
}
