#pragma once
#include "GUI.h"


#define MAINWINDOW_WIDTH 1600
#define MAINWINDOW_HEIGHT 600

class Engine{
private:
	GUI *mainGUI;
	sf::RenderWindow* mainFrame;
	sf::WindowHandle windowhandle;
	Main *m_main;
	bool running;
public:
	void window_init(sf::RenderWindow* frame);
	void initGUI();
	void mainLoop();
	bool IsRunning() { return running; }
	Engine(sf::RenderWindow* pframe, Main* pMain);
	GUI* getGUI();
	sf::RenderWindow* getFrame();
	~Engine();
};