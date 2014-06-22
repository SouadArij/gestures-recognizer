#include "Bouton.h"

Bouton::Bouton() : GUIObject(NULL, sf::Vector2f(5,5))
{
	this->height = 50;
	this->width = 50;
	this->isPressed = false;
	this->color = sf::Color(161,40,153);
	this->isActiveMod = false;
	buildBouton();
}

Bouton::Bouton(sf::Vector2f position, sf::Vector2i dimensions, std::string texte, Main* pMain, void(Main::*pPtr)(Bouton)): GUIObject(texte, position)
{
	m_main = pMain;
	m_ptr = pPtr;
	this->width = dimensions.x;
	this->height = dimensions.y;
	this->isPressed = false;

	if(!this->font.loadFromFile("resources/sansation.ttf")) printf("echec font load\n");
	m_texte.setFont(font);
	this->color = sf::Color(161,40,153);
	this->isActiveMod = false;
	buildBouton();
}

void Bouton::action(){
	return;
}

void Bouton::onAction(){

	if(this->isActiveMod){
		this->activateModOn = !this->activateModOn;
		if(this->activateModOn) this->color = sf::Color::Green;
		else this->color = sf::Color(161,40,153);
	}
	else userFeedbackAnim();
	(*m_main.*m_ptr)(*this);
	buildBouton();
	return;
}

sf::RectangleShape Bouton::getGraphic(){
	return this->shape ;
}

void Bouton::buildBouton(){
	this->shape.setSize(sf::Vector2f(this->width,this->height));
	this->shape.setPosition(m_position.x, m_position.y);
	this->shape.setFillColor(this->color);
	//txt
	std::string tmp = m_texte.getString();
	int size = tmp.size();
	int charsize = this->height/4 > 10 ? this->height/4 : 10;
	m_texte.setCharacterSize(charsize);

	float x = m_position.x +this->width/2 - m_texte.getCharacterSize()*0.25 *size;
	float y = m_position.y +this->height/2- m_texte.getCharacterSize()*0.5;
	m_texte.setPosition(x,y);
	m_texte.setColor(sf::Color::White);
}

void Bouton::setColor(sf::Color col){
	this->color = col;
}

sf::Vector2i Bouton::getDim(){
	return sf::Vector2i(this->width,this->height);
}

void Bouton::userFeedbackAnim(){
	this->color = sf::Color::Green;
	buildBouton();
	sf::sleep(sf::milliseconds(50));
	this->color = sf::Color(161,40,153);
	buildBouton();
}

void Bouton::setActiveMod(bool b){
	this->isActiveMod = b;
	if (b) activateModOn = false;
}

Bouton::~Bouton()
{
}