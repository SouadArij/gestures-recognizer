#include "Engine.h"

void renderingWindowTh(Engine* eng);

Engine::Engine(sf::RenderWindow* pframe) : running(false){
	// création de la fenêtre
	this->mainFrame = pframe;
	this->windowhandle = this->mainFrame->getSystemHandle();
    window_init(this->mainFrame);
	this->mainFrame->setActive(false);
	this->mainGUI = new GUI();
	this->initGUI();
	this->running = true;
}

void Engine::mainLoop()
{
	//Création des threads
	sf::Thread t_ui(&renderingWindowTh,this);
	t_ui.launch();
	//sf::Thread t_con(&checkBConnectionsTh,buttons);
	//t_con.launch();
    // la boucle principale
	this->mainGUI->imgRefresh();
    while (this->running)
    {
		for(int i=0; i < this->mainGUI->getButtons().size();i++)
		{
			if(this->mainGUI->getButtons().at(i)->getIsActive()) this->mainGUI->getButtons().at(i)->callbackOnly();
		}
        // gestion des évènements
        sf::Event event;
        while (this->mainFrame->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // on stoppe le programme
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // on ajuste le viewport lorsque la fenêtre est redimensionnée
                glViewport(0, 0, event.size.width, event.size.height);
            }
			else if (event.type == sf::Event::MouseButtonPressed){
				//pression de la souris - check si bouton détecté en dessous
				this->mainGUI->mouseAction(sf::Mouse::getPosition(*this->mainFrame).x,sf::Mouse::getPosition(*this->mainFrame).y);
			}
			else if (event.type == sf::Event::TextEntered){
				//Il faut un textArea en focus
				if(event.text.unicode == '\b') this->mainGUI->deleteInput();
				else this->mainGUI->keyboardInput(event.text.unicode);
				
			}
			

        }

        // effacement les tampons de couleur/profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // dessin...

        // termine la trame courante (en interne, échange les deux tampons de rendu)
        //main_window.display();
    }

    // libération des ressources...
	return;
}

void Engine::window_init(sf::RenderWindow *frame)
{
	frame->setVerticalSyncEnabled(true);
}


GUI* Engine::getGUI(){
	return this->mainGUI;
}
sf::RenderWindow* Engine::getFrame(){
	return this->mainFrame;
}

//   Thread d'actualisation de la fenêtre \\

void renderingWindowTh(Engine* eng)
{
	sf::RenderWindow* frame = eng->getFrame();
	GUI* UI = eng->getGUI();
	frame->setActive(true);
	frame->clear(sf::Color::Black);
	// boucle principal du thread
	while(frame->isOpen()){
	//update OpenGL

	//glBegin(GL_POLYGON);
	//	glColor3ub(255,0,0);    glVertex2d(-0.75,-0.75);
	//	glColor3ub(0,255,0);    glVertex2d(0,0.75);
	//	glColor3ub(0,0,255);    glVertex2d(0.75,-0.75);
	//	glColor3ub(125,125,125);glVertex2d(0,0);
	//glEnd();
	//glFlush();

	frame->pushGLStates();
	//update sfml
	frame->setActive(false);
	UI->drawGUI(frame);
	frame->setActive(true);
	//display
	frame->popGLStates();
	frame->display();
	}

}

void Engine::checkBConnectionsTh(std::vector<Bouton*>buttons){
	while(true){

	}
}

void Engine::initGUI(){
	int  w = MAINWINDOW_WIDTH;
	int  h = MAINWINDOW_HEIGHT;
	int  p = 10; //padding
	int b_w = w/6 - p*2; // button width
	int sender_w = (w/6 - p*2)/2;
	int b_h = h/8 - p*2;
	int init_pos = w - 2*b_w - 2*p;
	int txtbox_w = b_w + 0.5*b_w;
	int txtbox_h = b_h/2;
	int sender_h = txtbox_h;
	sf::Font font;
	if(!font.loadFromFile("resources/sansation.ttf")) write_text_to_log_file("echec font load");
	//Création des objets connectés
	Bouton *test  = new Bouton(sf::Vector2f(init_pos,p)            ,sf::Vector2i(b_w ,b_h),"LeftHand");
	Bouton *test2 = new Bouton(sf::Vector2f(init_pos +b_w+p, p)      ,sf::Vector2i(b_w ,b_h),"RightHand");
	Bouton *test3 = new Bouton(sf::Vector2f(init_pos, h/8 )      ,sf::Vector2i(b_w ,b_h),"Display K-hull");
	Bouton *test4 = new Bouton(sf::Vector2f(init_pos +b_w+p, h/8) ,sf::Vector2i(b_w ,b_h),"Display more");

	Bouton *text_sender  = new Bouton(sf::Vector2f(init_pos,h/8 + h/8+(h/16)*0),sf::Vector2i(sender_w,sender_h),"Send");
	Bouton *text_sender2 = new Bouton(sf::Vector2f(init_pos,h/8 + h/8+(h/16)*1),sf::Vector2i(sender_w,sender_h),"Send");
	Bouton *text_sender3 = new Bouton(sf::Vector2f(init_pos,h/8 + h/8+(h/16)*2),sf::Vector2i(sender_w,sender_h),"Send");
	Bouton *text_sender4 = new Bouton(sf::Vector2f(init_pos,h/8 + h/8+(h/16)*3),sf::Vector2i(sender_w,sender_h),"Send");
	Bouton *text_senderAll = new Bouton(sf::Vector2f(init_pos,h/8 + h/8+(h/16)*4),sf::Vector2i(sender_w*4 + p,sender_h),"SendAll");

	TextField *textField = new TextField(init_pos +b_w/2 +p,h/8 + h/8 ,txtbox_w,txtbox_h);
	TextField *textField2= new TextField(init_pos +b_w/2 +p,h/8 + h/8 + (h/16)*1 ,txtbox_w,txtbox_h);
	TextField *textField3= new TextField(init_pos +b_w/2 +p,h/8 + h/8 + (h/16)*2 ,txtbox_w,txtbox_h);
	TextField *textField4= new TextField(init_pos +b_w/2 +p,h/8 + h/8 + (h/16)*3 ,txtbox_w,txtbox_h);

	//sf::Text *labelRecord = new sf::Text() ;
	//labelRecord->setString("RECORD: a ; b ; c ; d");
	//labelRecord->setFont(font);
	//labelRecord->setColor(sf::Color::White);
	//labelRecord->setCharacterSize(20);
	//labelRecord->setPosition(400,400);


	test2->setActiveMod(true);

	this->mainGUI->addButton(test);
	this->mainGUI->addButton(test2);
	this->mainGUI->addButton(test3);
	this->mainGUI->addButton(test4);
	this->mainGUI->addButton(text_sender);
	this->mainGUI->addButton(text_sender2);
	this->mainGUI->addButton(text_sender3);
	this->mainGUI->addButton(text_sender4);
	this->mainGUI->addButton(text_senderAll);
	this->mainGUI->addTextField(textField);
	this->mainGUI->addTextField(textField2);
	this->mainGUI->addTextField(textField3);
	this->mainGUI->addTextField(textField4);
	/*this->mainGUI->addLabel(labelRecord);*/

	CallbackCallee* test_cb = new CallbackCallee();
	test->connect(test_cb);
	test2->connect(test_cb);

	text_sender->connect(textField);
	text_sender2->connect(textField2);
	text_sender3->connect(textField3);
	text_sender4->connect(textField4);

	text_senderAll->addMulConnection(textField);
	text_senderAll->addMulConnection(textField2);
	text_senderAll->addMulConnection(textField3);
	text_senderAll->addMulConnection(textField4);


}
