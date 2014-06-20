#include "Bouton.h"

Bouton::Bouton()
{
	this->isPressed = false;
	this->height = 50;
	this->width = 50;
	this->posX = 5;
	this->posY = 5;
	this->color = sf::Color::Yellow;
	this->isActiveMod = false;
	buildBouton();
}

Bouton::Bouton(sf::Vector2f position, sf::Vector2i dimensions, std::string texte)
{
	this->posX = position.x;
	this->posY = position.y;
	this->width = dimensions.x;
	this->height = dimensions.y;
	this->isPressed = false;
	this->text.setString( texte );
	if(!this->font.loadFromFile("resources/sansation.ttf")) write_text_to_log_file("echec font load");
	this->text.setFont(font);
	this->color = sf::Color::Yellow;
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
		else this->color = sf::Color::Yellow;
	}
	else userFeedbackAnim() ;
	if(this->get_cb() != NULL)
	{
		this->get_cb()->cb_func();
	}
	else 	if(this->getMulCb().size() > 1){
		for(int i = 0;i < this->getMulCb().size();i++) this->getMulCb().at(i)->cb_func();
	}
	buildBouton();
	return;
}

void Bouton::callbackOnly(){
	if(this->get_cb() != NULL)
	{
		this->get_cb()->cb_func();
	}

}

void Bouton::callbackMult(){
	if(this->getMulCb().size() > 1){
		for(int i = 0;i < this->getMulCb().size();i++) this->getMulCb().at(i)->cb_func();
	}
}

sf::RectangleShape Bouton::getGraphic(){
	return this->shape ;
}

sf::Text Bouton::getText(){
	return this->text;
}

void Bouton::buildBouton(){
	this->shape.setSize(sf::Vector2f(this->width,this->height));
	this->shape.setPosition(this->posX, this->posY);
	this->shape.setFillColor(this->color);
	//txt
	std::string tmp = this->text.getString();
	int size = tmp.size();
	int charsize = this->height/4 > 10 ? this->height/4 : 10;
	this->text.setCharacterSize(charsize);

	float x = this->posX +this->width/2 - this->text.getCharacterSize()*0.25 *size;
	float y = this->posY +this->height/2- this->text.getCharacterSize()*0.5;
	this->text.setPosition(x,y);
	this->text.setColor(sf::Color::Black);
}

void Bouton::setColor(sf::Color col){
	this->color = col;
}

sf::Vector2f Bouton::getPos(){
	return sf::Vector2f(this->posX,this->posY);

}

sf::Vector2i Bouton::getDim(){
	return sf::Vector2i(this->width,this->height);
}

void Bouton::userFeedbackAnim(){
	this->color = sf::Color::Green;
	buildBouton();
	sf::sleep(sf::milliseconds(50));
	this->color = sf::Color::Yellow;
	buildBouton();
}

void Bouton::setActiveMod(bool b){
	this->isActiveMod = b;
	if (b) activateModOn = false;
}

Bouton::~Bouton()
{
}