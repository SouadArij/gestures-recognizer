#pragma once
#include "GUIObject.h"
#define SFML_STATIC
#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
class Main;

#define connecNum 10 //Number of connections

class Bouton: public GUIObject {
private:
	int width, height;
	bool isPressed ;
	sf::Font font;
	sf::RectangleShape shape;
	sf::Color color;
	sf::Texture texture;
	Main* m_main;
	void(Main::*m_ptr)(Bouton);
	bool isActiveMod; // reste actif si pressé avec ce mod
	bool activateModOn;
	void buildBouton();
public:
	Bouton();
	Bouton(sf::Vector2f position, sf::Vector2i dimensions, std::string texte, Main* pMain, void(Main::*pPtr)(Bouton));
	sf::RectangleShape getGraphic();
	void setColor(sf::Color color);
	void action(void);
	void onAction(void);
	sf::Vector2i getDim();
	bool getIsActive(){return (this->activateModOn&&this->isActiveMod);}
	void userFeedbackAnim();
	void callbackOnly();
	void callbackMult();
	void setActiveMod(bool b);
	sf::Font getFont() { return this->font;}
	~Bouton();
};