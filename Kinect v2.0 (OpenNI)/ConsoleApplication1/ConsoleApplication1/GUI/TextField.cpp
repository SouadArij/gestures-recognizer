#include "TextField.h"

TextField::TextField(){
}
TextField::TextField(float px,float py,float pwidth, float pheight){
	this->hasFocus = false;
	this->textArea.setPosition(px,py);
	this->textArea.setSize(sf::Vector2f(pwidth,pheight));
	this->textArea.setFillColor(sf::Color::White);
	this->str.clear();
	int charsize = pheight/4;
	if (charsize < 10) charsize = 10;
	m_texte.setCharacterSize(charsize);
	if(!this->font.loadFromFile("resources/sansation.ttf")) printf("echec font load\n");
	m_texte.setFont(font);
	float x = px + pwidth - PADDING_RIGHT;
	float y = py + pheight- m_texte.getCharacterSize()*0.25;
	fixPosition();
	m_texte.setColor(sf::Color::Black);
	m_texte.setString(this->str);
}

void TextField::clearStr(){
	this->str.clear();
}

void TextField::addChar(std::string toAdd){

	if(this->str.getSize() >= MAX_SIZE) return;
	std::string tmp;
	tmp = this->str;
	tmp.append(toAdd);
	this->str.clear();
	this->str = tmp;
	m_texte.setString(this->str);
	//mettre à jour la position du texte en X
	fixPosition();
	//printf(m_texte.getString().toAnsiString().c_str());
	return;
}

void TextField::removeLastchar(){

	if(this->str.getSize() <= 0) return;
	std::string tmp = this->str;
	this->str.clear();
	tmp.pop_back();
	this->str = tmp;
	m_texte.setString(this->str);
	fixPosition();
	return;
}

sf::RectangleShape TextField::getTextArea(){
	return this->textArea;
}

void TextField::update(){
	if(hasFocus){
		this->textArea.setOutlineColor(sf::Color::Green);
		this->textArea.setFillColor(sf::Color(196,244,193,255));
	}
	else{
		this->textArea.setOutlineColor(sf::Color::White);
		this->textArea.setFillColor(sf::Color::White);
	}
}

void TextField::setFocus(bool b){
	this->hasFocus = b;
}

void TextField::fixPosition(){
	size_t charsize = m_texte.getCharacterSize();
	sf::FloatRect lb = this->textArea.getLocalBounds();
	int s_size = m_texte.getString().getSize();
	/*size_t maxHeight,height;

	for(size_t i =0; i < this->str.getSize() ; ++i){
		sf::Uint32 character = this->txt.getString().toAnsiString().at(i);
		const sf::Glyph& CurrentGlyph = this->txt.getFont()->getGlyph(character,charsize,false);
		height = CurrentGlyph.bounds.height;
		if (maxHeight < height) maxHeight = height;
     }*/
	m_texte.setOrigin(lb.left + lb.width/2.0f, lb.top + lb.height /2.0f);
	m_texte.setPosition(sf::Vector2f(this->textArea.getPosition().x + lb.width/1.8f,
									this->textArea.getPosition().y + lb.height/1.2f));

}

TextField::~TextField(){
}

