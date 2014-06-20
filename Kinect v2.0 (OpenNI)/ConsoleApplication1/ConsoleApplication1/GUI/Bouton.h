#pragma once
//#include "Connection.h"
#include "CallbackCaller.h"

#define connecNum 10 //Number of connections

class Bouton: public CallbackCaller {
private:
	float posX, posY;
	int width, height;
	bool isPressed ;
	sf::Font font;
	sf::Text text;
	sf::RectangleShape shape;
	sf::Color color;
	sf::Texture texture;
	bool isActiveMod; // reste actif si pressé avec ce mod
	bool activateModOn;
	void buildBouton();
public:
	Bouton();
	Bouton(sf::Vector2f position, sf::Vector2i dimensions, std::string texte);
	sf::RectangleShape getGraphic();
	sf::Text getText();
	void setColor(sf::Color color);
	void action(void);
	void onAction(void);
	sf::Vector2f getPos();
	sf::Vector2i getDim();
	bool getIsActive(){return (this->activateModOn&&this->isActiveMod);}
	void userFeedbackAnim();
	void callbackOnly();
	void callbackMult();
	void setActiveMod(bool b);
	sf::Font getFont() { return this->font;}
	~Bouton();
};