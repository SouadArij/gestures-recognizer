
#ifndef CALLBACKCALLEE_H
#define CALLBACKCALLEE_H
#include "CallbackCallee.h"
#endif
#include "GUI.h"


#define MAINWINDOW_WIDTH 800
#define MAINWINDOW_HEIGHT 600

class Engine{
private:
	GUI *mainGUI;
	sf::RenderWindow* mainFrame;
	sf::WindowHandle windowhandle;
	bool running;
public:
	void window_init(sf::RenderWindow* frame);
	void checkBConnectionsTh(std::vector<Bouton*>buttons);
	void initGUI();
	void mainLoop();
	bool IsRunning() { return running; }
	Engine(sf::RenderWindow* pframe);
	GUI* getGUI();
	sf::RenderWindow* getFrame();
	~Engine();
};