#include "../includes/Menu.hpp"

MenuState::MenuState( Engine & engine ){
	std::cout << "Menu constructed" << std::endl; // debug
	this->_engine = &engine;
	this->_MainMenuOptions[0] = "New game";
	this->_MainMenuOptions[1] = "Load game";
	this->_MainMenuOptions[2] = "Options";
	this->_MainMenuOptions[3] = "Credits";
	this->_MainMenuOptions[4] = "Quit";
	this->_menuSize = this->_MainMenuOptions.size();
	this->_menuIndex = 0;
}

MenuState::MenuState( void ){
	std::cout << "Menu constructed" << std::endl; // debug
}

MenuState::~MenuState( void ){
	std::cout << "Menu destructed" << std::endl; // debug
}

void MenuState::update( eControls key ){
	std::cout << "Menu update" << std::endl; // debug
	this->_changeSelection( key );
}

void MenuState::render( void ){
	this->_engine->render();
	// this->_engine->renderMenu( array of options, current index, background/layout );
	std::cout << "Menu render" << std::endl; // debug
}

void	MenuState::_changeSelection( eControls key){
	switch (key){
		case UP:
			this->_menuIndex--;
			break;
		case DOWN:
			this->_menuIndex++;
			break;
		case ENTER:
			this->_makeSelection(this->_menuIndex);
			break;
		default:
			break;
	};
	this->_menuIndex = std::abs(this->_menuIndex % 5);
	std::cout << "Menu index: " << this->_menuIndex << std::endl; // debug
}

void	MenuState::_makeSelection( int _menuIndex ){
	std::cout << "Selected " << this->_menuIndex << "! (" << this->_menuIndex << ")" << std::endl;
	
	// reset _menuIndex;
	// update state depending on selection
	if (this->_menuIndex == 2) // test
		this->_engine->state = PLAY;
}