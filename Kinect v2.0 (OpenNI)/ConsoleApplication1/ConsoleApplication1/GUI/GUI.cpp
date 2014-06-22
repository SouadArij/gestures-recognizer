#include "GUI.h"


GUI::GUI() : m_Textures(3), m_RectanglesShapes(3), m_ImageMat(3){
	m_RectanglesShapes[0] = new sf::RectangleShape(sf::Vector2f(320,240));
	m_RectanglesShapes[0]->setPosition(5,5);
	m_Textures[0] = new sf::Texture();
	m_Textures[0]->create(m_RectanglesShapes[0]->getSize().x,m_RectanglesShapes[0]->getSize().y);

	m_RectanglesShapes[1] = new sf::RectangleShape(sf::Vector2f(640,480));
	m_RectanglesShapes[1]->setPosition(330,5);
	m_Textures[1] = new sf::Texture();
	m_Textures[1]->create(m_RectanglesShapes[1]->getSize().x,m_RectanglesShapes[1]->getSize().y);

	m_RectanglesShapes[2] = new sf::RectangleShape(sf::Vector2f(320,240));
	m_RectanglesShapes[2]->setPosition(5,250);
	m_Textures[2] = new sf::Texture();
	m_Textures[2]->create(m_RectanglesShapes[2]->getSize().x,m_RectanglesShapes[2]->getSize().y);

	m_labelsMutex = CreateMutex(NULL, FALSE, NULL);
}

// Permet d'ajouter un bouton à l'interface
void GUI::addButton(Bouton* toAdd){

	this->buttons.push_back(toAdd);
}

// Permet d'ajouter un textfield à l'interface
void GUI::addTextField(TextField* toAdd){
	this->textfields.push_back(toAdd);
}

//Permet d'ajouter une chaine de caractères à l'interface
void GUI::addLabel(Label *toAdd){
	this->labels.push_back(toAdd);
}

// Permet de dessiner le GUI en général - prend la fenêtre de rendu en paramètre
void GUI::drawGUI(sf::RenderWindow *frame){

	frame->setActive(true);
	frame->clear(sf::Color(230,230,230,255));

	for( int iterator = 0; iterator < m_Textures.size(); iterator++)
	{
		imgRefresh(iterator);
	}


	for (int i = 0; i < this->buttons.size(); i++){
		frame->draw(this->buttons.at(i)->getGraphic());
		frame->draw(this->buttons.at(i)->getTexte());
	}

	for(int j = 0; j < this->textfields.size();j++){
		this->textfields.at(j)->update();
		frame->draw(this->textfields.at(j)->getTextArea());
		frame->draw(this->textfields.at(j)->getTexte());
	}

	WaitForSingleObject(m_labelsMutex, INFINITE);
	Label *tmp = NULL;
	for(int k=0; k < this->labels.size();k++){
		tmp = labels.at(k);
		frame->draw(tmp->getTexte());
	}
	ReleaseMutex(m_labelsMutex);


	for(int iterator = 0; iterator < m_Textures.size(); iterator++)
	{
		if(m_RectanglesShapes[iterator]->getTexture() != NULL) 
		{
			frame->draw(*m_RectanglesShapes[iterator]);
		}
	}

	
	frame->setActive(false);
	return;
}

// Détecte si un des widgets de l'interface a pris le focus sur un clic - interne à la classe
void GUI::mouseAction(float x,float y){
	float co_x, co_y;
	int	  co_w, co_h;
	for (int i=0; i < buttons.size(); i++){
		co_x = buttons.at(i)->getPosition().x;
		co_y = buttons.at(i)->getPosition().y;
		co_w = buttons.at(i)->getDim().x;
		co_h = buttons.at(i)->getDim().y;
		if( (x>co_x) && (y > co_y) ) {
			if((x<co_x + co_w) && (y<co_y + co_h) ){
				// bouton actionné
				buttons.at(i)->onAction();
				
			}
		}
	}
	for (int i=0; i < textfields.size(); i++){
		co_x = textfields.at(i)->getTextArea().getPosition().x;
		co_y = textfields.at(i)->getTextArea().getPosition().y;
		co_w = textfields.at(i)->getTextArea().getSize().x;
		co_h = textfields.at(i)->getTextArea().getSize().y;
		if( (x>co_x) && (y > co_y) ) {
			if((x<co_x + co_w) && (y<co_y + co_h) ){
				// bouton actionné
				textfields.at(i)->setFocus(true);
				printf("HAZ FOCUS BOY\n");
				
			}
			else{
			textfields.at(i)->setFocus(false);
			printf("NO MORE FOCUS\n");
		}
		}
		else{
			textfields.at(i)->setFocus(false);
			printf("NO MORE FOCUS\n");
		}

	}

}

// Fait le lien pour entrer un caractère dans un textfuield - interne à la classe
void GUI::keyboardInput(sf::String input){
	std::string tmp = input;
	for(int i =0;i<textfields.size();i++){
		if(textfields.at(i)->getFocus()){
			textfields.at(i)->addChar(input);
			break;
		}
	}
	return;
}

// Fait le lien pour supprimer un caractère dans un textfield - interne à la classe
void GUI::deleteInput(){
		for(int i =0;i<textfields.size();i++){
		if(textfields.at(i)->getFocus()){
			textfields.at(i)->removeLastchar();
			break;
		}
	}
	return;
}

// Permet de passer de cv::Mat à un affichage SFML - interne à la classe
void GUI::imgRefresh(int iterator){

	// création d'une matrice RGBA 32 bit artificielle
	cv::Mat image;
	if(m_ImageMat[iterator].data != NULL){
		image = m_ImageMat[iterator];
	}
	else{
	image = cv::imread("resources/blank.png",CV_LOAD_IMAGE_COLOR);
	 if(! image.data ) // Check for invalid input
    {
        printf("ERREUR: IMREAD\n");
    }
	}
	cv::cvtColor(image, image, CV_RGB2BGRA);
	
	int w = image.size().width;
	int h = image.size().height;
	this->sf_img.create(w,h,sf::Color::White);

    unsigned char *input = (unsigned char*)(image.data);
	m_Textures[iterator]->update(input,w,h,0,0);
	m_RectanglesShapes[iterator]->setTexture(m_Textures[iterator]);
	
}

// Passer en paramètre l'image qui va être afficher sur le GUI
void GUI::setRawImage(cv::Mat rawImage, int iterator){
	rawImage.copyTo(m_ImageMat[iterator]);
}

GUI::~GUI(){
}
