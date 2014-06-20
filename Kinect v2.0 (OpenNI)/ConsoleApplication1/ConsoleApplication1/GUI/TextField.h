#ifndef CALLBACKCALLEE_H
#define CALLBACKCALLEE_H
#include "CallbackCallee.h"
#endif

#define MAX_SIZE 22
#define PADDING_RIGHT  2

//TODO : FIX LE TEXT QUI AFFICHE DE LA MERDE avec le DELETE
//TODO : POSITIONNEMENT DU TXT DANS LA TXTBOX



class TextField : public CallbackCallee {
private:
	bool hasFocus;  // textArea is focused
	sf::String str; // str to work with
	sf::Text txt;  // text to display
	sf::Font font; // Police d'écriture
	sf::RectangleShape textArea; // Area where the text displays

public:
	TextField();
	TextField(float px,float py,float pwidth, float pheight);
	void cb_func(); //will send his string
	void clearStr(); //clear the string
	void addChar(std::string toAdd); //add a char to the string
	void removeLastchar(); // remove last char when deleted
	void update(); // update appearance of textArea
	void setFocus(bool b);
	void fixPosition(); // give a good position to the text in the textArea
	sf::Text getText(){return this->txt;}
	bool getFocus() {return this->hasFocus;}

	sf::RectangleShape getTextArea();
	~TextField();
};