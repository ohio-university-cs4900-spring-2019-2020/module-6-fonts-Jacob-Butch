<<<<<<< HEAD
#include "GUI.h"

#include <iostream>

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

WOMacroDeclaration(GUI, WOGUI);

GUI* GUI::New(WOGUI* parentWOGUI, float width, float height, std::string panelTex) {
	GUI* gui = new GUI();
	gui->onCreate(width, height, panelTex);
	return gui;
}

GUI::GUI() : IFace(this), WOGUI(nullptr) {}

void GUI::onMouseDown(const SDL_MouseButtonEvent& e) { 
	if (this->getLabel() != "Progress Bar (Bar)" && this->getLabel() != "Progress Bar (Progress)") {
		this->HasFocus = !this->hasFocus();
	}
}

bool GUI::pixelResidesInBoundingRect(int x, int y) { 
	return WOGUI::pixelResidesInBoundingRect(x, y); 
}

=======
#include "GUI.h"

#include <iostream>

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

WOMacroDeclaration(GUI, WOGUI);

GUI* GUI::New(WOGUI* parentWOGUI, float width, float height, std::string panelTex) {
	GUI* gui = new GUI();
	gui->onCreate(width, height, panelTex);
	return gui;
}

GUI::GUI() : IFace(this), WOGUI(nullptr) {}

void GUI::onMouseDown(const SDL_MouseButtonEvent& e) { 
	if (this->getLabel() != "Progress Bar (Bar)" && this->getLabel() != "Progress Bar (Progress)") {
		this->HasFocus = !this->hasFocus();
	}
}

bool GUI::pixelResidesInBoundingRect(int x, int y) { 
	return WOGUI::pixelResidesInBoundingRect(x, y); 
}

>>>>>>> 824740c3fc2ea164db81d1212f41b04fbc846812
#endif