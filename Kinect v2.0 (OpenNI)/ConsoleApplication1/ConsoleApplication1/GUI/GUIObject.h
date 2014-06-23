#pragma once
#define SFML_STATIC
#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include <vector>

class GUIObject
{
public:
	GUIObject(void);
	GUIObject(std::string pStr, sf::Vector2f pPosition);
	~GUIObject(void);

	void Attach(GUIObject* object);

	std::vector<GUIObject*> getAttachedObject() { return m_AttachedObject; }
	sf::Text getTexte() { return m_texte; }
	sf::Vector2f getPosition() { return m_position; }

	void setText(std::string pText) { 
		m_texte.setString(pText); 
	}

protected :
	sf::Text m_texte;
	std::vector<GUIObject*> m_AttachedObject;
	sf::Vector2f m_position;
};

