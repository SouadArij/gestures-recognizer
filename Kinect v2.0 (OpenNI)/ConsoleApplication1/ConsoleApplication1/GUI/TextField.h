#pragma once
#include "GUIObject.h"
#define SFML_STATIC
#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>

#define MAX_SIZE 22
#define PADDING_RIGHT  2



class TextField : public GUIObject {
private:
	bool hasFocus;  // textArea is focused
	sf::String str; // str to work with
	sf::Font font; // Police d'écriture
	sf::RectangleShape textArea; // Area where the text displays
public:
	TextField();
	TextField(float px,float py,float pwidth, float pheight);
	void clearStr(); //clear the string
	void addChar(std::string toAdd); //add a char to the string
	void removeLastchar(); // remove last char when deleted
	void update(); // update appearance of textArea
	void setFocus(bool b);
	void fixPosition(); // give a good position to the text in the textArea
	bool getFocus() {return this->hasFocus;}
	sf::RectangleShape getTextArea();
	~TextField();
};