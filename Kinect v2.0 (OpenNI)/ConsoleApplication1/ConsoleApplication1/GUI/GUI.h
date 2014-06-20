#pragma once
#pragma warning(disable: 4996)

#define SFML_STATIC

#include <vector>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>

#include "Bouton.h"
#include "TextField.h"



#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 240

/*  --------------------------------- *\
	CLASS : GUI
	Rôle  : Mises en place et affichage
	des différents éléments graphiques
\*  --------------------------------- */
class GUI {

private:
	std::vector<Bouton*> buttons;
	std::vector<TextField*> textfields;
	std::vector<sf::Text*> labels;
	sf::Image sf_img;
	sf::Texture* img_tex;
	sf::RectangleShape* videoFrame;
	cv::Mat raw_img;

public:
	GUI();
	void addButton(Bouton* toAdd);
	void addTextField(TextField* toAdd);
	void addLabel(sf::Text* toAdd);
	void drawGUI(sf::RenderWindow *frame);
	void mouseAction(float x,float y);
	void keyboardInput(sf::String input);
	void deleteInput();
	void setRawImage(cv::Mat rawImage);
	void imgRefresh(); // prendra un flux de data en param
	std::vector<Bouton*> getButtons(){return this->buttons;}
	~GUI();

};