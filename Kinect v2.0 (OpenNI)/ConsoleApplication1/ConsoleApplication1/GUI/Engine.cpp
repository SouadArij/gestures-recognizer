#include "Engine.h"
#include "../Main.h"

void renderingWindowTh(Engine* eng);

Engine::Engine(sf::RenderWindow* pframe, Main* pMain) : running(false){
	// création de la fenêtre
	m_main = pMain;
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
	//this->mainGUI->imgRefresh();
    while (this->running)
    {
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
	frame->clear(sf::Color::White);
	// boucle principal du thread
	while(frame->isOpen()){
		//update OpenGL
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


void Engine::initGUI(){
	int  w = MAINWINDOW_WIDTH;
	int  h = MAINWINDOW_HEIGHT;
	int  p = 10; //padding
	int init_pos = 970 +p;

	int b_w = w - init_pos - p*2; // button width
	int b_h = h/10 - p*2;

	int txtbox_w = b_w/2;
	int txtbox_h = b_h/2;

	int sender_w = b_w/2;
	int sender_h = txtbox_h;
	
	
	sf::Font font;
	if(!font.loadFromFile("resources/sansation.ttf")) printf("echec font load\n");
	
	//Création des objets connectés
	void(Main::*ptr)(Bouton) = &Main::OnRecordGestureButton;
	Bouton *test  = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*0),sf::Vector2i(b_w ,b_h),"Record Gesture", m_main, ptr);

	ptr = &Main::OnRecordPositionButton;
	Bouton *test2 = new Bouton(sf::Vector2f(init_pos,  p+(p+b_h)*1),sf::Vector2i(b_w ,b_h),"Record Position", m_main, ptr);

	ptr = &Main::OnRecordHandPositionButton;
	Bouton *test3 = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*2 ),sf::Vector2i(b_w ,b_h),"Record Hand Position", m_main, ptr);

	ptr = &Main::OnGetRecordInfoButton;
	Bouton *test4 = new Bouton(sf::Vector2f(init_pos,  p+(p+b_h)*3 ),sf::Vector2i(b_w ,b_h),"Record Infos", m_main, ptr);

	ptr = &Main::OnStopRecordButton;
	Bouton *stopRecord = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*4),sf::Vector2i(b_w ,b_h),"Stop Recording", m_main, ptr);

	ptr = &Main::OnSaveSampleButton;
	Bouton *saveSample = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*5),sf::Vector2i(b_w ,b_h),"Save Sample", m_main, ptr);

	ptr = &Main::OnChangeMainFrameButton;
	Bouton *changeMainFrame = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*6),sf::Vector2i(b_w ,b_h),"Change view", m_main, ptr);

	ptr = &Main::OnSendClassTextButton;
	Bouton *text_sender  = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*7),sf::Vector2i(sender_w,sender_h),"Set Classe", m_main, ptr);
	TextField *textField = new TextField(init_pos +sender_w +p, text_sender->getPosition().y ,txtbox_w,txtbox_h);

	ptr = &Main::OnSendNameTextButton;
	Bouton *text_sender2 = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*8),sf::Vector2i(sender_w,sender_h),"Set Name", m_main, ptr);
	TextField *textField2= new TextField(init_pos +sender_w +p, text_sender2->getPosition().y ,txtbox_w,txtbox_h);

	ptr = &Main::OnSendTimeoutButton;
	Bouton *text_sender3 = new Bouton(sf::Vector2f(init_pos, p+(p+b_h)*9),sf::Vector2i(sender_w,sender_h),"Set Timeout", m_main, ptr);
	TextField *textField3= new TextField(init_pos +sender_w +p, text_sender3->getPosition().y ,txtbox_w,txtbox_h);
	
	Label *labelPosition = new Label("POSITION :", sf::Vector2f(20, 500));
	Label *labelGeste = new Label("GESTE :", sf::Vector2f(20, 530));
	Label *labelHand = new Label("MAIN :", sf::Vector2f(20, 560));

	mainGUI->addButton(test);
	mainGUI->addButton(test2);
	mainGUI->addButton(test3);
	mainGUI->addButton(test4);
	mainGUI->addButton(saveSample);
	mainGUI->addButton(stopRecord);
	mainGUI->addButton(changeMainFrame);
	mainGUI->addButton(text_sender);
	mainGUI->addButton(text_sender2);
	mainGUI->addButton(text_sender3);
	mainGUI->addTextField(textField);
	mainGUI->addTextField(textField2);
	mainGUI->addTextField(textField3);
	mainGUI->addLabel(labelPosition);
	mainGUI->addLabel(labelGeste);
	mainGUI->addLabel(labelHand);

	text_sender->Attach(textField);
	text_sender2->Attach(textField2);
	text_sender3->Attach(textField3);

}
