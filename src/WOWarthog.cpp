#include "WOWarthog.h"

#include <complex> 
#include "ISoundManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

WOWarthog* WOWarthog::New(IPhysicsManager* physics) {
	WOWarthog* wo = new WOWarthog(physics);
	wo->onCreate("../mm/models/warthog/warthog.obj", Vector(4, 4, 4), MESH_SHADING_TYPE::mstSMOOTH);
	wo->setLabel("Warthog");
	wo->setPosition(Vector(13, -17, -4));
	return wo;
}

WOWarthog::WOWarthog(IPhysicsManager* physics) : IFace(this), WO() {
	this->speed = 1;
	this->driver = nullptr;
	this->drivingSound = ISoundManager::getEngine()->play3D("../mm/sounds/warthog/drive.wav", ISoundManager::toVec3df(Vector(0, -17, -4)), true, true, true);
	this->drivingSound->setMinDistance(30);
	this->horn = ISoundManager::getEngine()->play3D("../mm/sounds/warthog/horn.wav", ISoundManager::toVec3df(Vector(0, -17, -4)), true, true, true);
	this->horn->setMinDistance(30);
	this->honking = false;

	this->actor = nullptr;

	this->textLabel = WOText::New(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf", 24, "Warthog", "Warthog");
}

void WOWarthog::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
	float mat[16] = {
		0, -1, 0, 0,
		0, 0, 1, 0,
		-1, 0, 0, 0,
		0, 0, 0, 1
	};
	this->textLabel->setDisplayMatrix(Mat4(mat));
	this->textLabel->setScale(Vector(10.0f, 10.0f, 10.0f));
	//this->textLabel->setLookDirection(this->getLookDirection());
	//this->textLabel->setNormalDirection(this->getNormalDirection());
}

void WOWarthog::updateLabel() {
	this->textLabel->setPosition(this->getPosition() + Vector(0, 0, 7));
	this->textLabel->setNormalDirection(this->getLookDirection());
	this->textLabel->setLookDirection(this->getLookDirection());
	Vector norm = this->getNormalDirection();
	Vector look = this->getLookDirection();

	// w[0] = t[8]
	// w[1] = t[9]
	// -w[4] = t[0]
	// -w[5] = t[1]
	Mat4 w = this->getDisplayMatrix();
	float t[16] = {
		-w[4], -w[5], 0, 0,
		0, 0, 1, 0,
		w[0], w[1], 0, 0,
		0, 0, 0, 1
	};
	this->textLabel->setDisplayMatrix(Mat4(t));
}

/*
		0, -1, 0, 0,
		0, 0, -1, 0,
		1, 0, 0, 0,
		0, 0, 0, 1
*/

WOWarthog::~WOWarthog() {
	if (this->drivingSound != nullptr) {
		//this->drivingSound->drop();
	}
	if (this->horn != nullptr) {
		//this->horn->drop();
	}
}

void WOWarthog::setDriver(Camera* driver) {
	this->driver = driver;
	if (driver != nullptr) {
		this->driver->setParentWorldObject(this);
	} else {
		this->keysPressed.clear();
		if (this->drivingSound != nullptr && !this->drivingSound->getIsPaused()) {
			this->drivingSound->setIsPaused(true);
		}
		if (this->horn != nullptr && !this->horn->getIsPaused()) {
			this->horn->setIsPaused(true);
		}
	}
}

void WOWarthog::onMouseUp(const SDL_MouseButtonEvent& e) {}
void WOWarthog::onMouseMove(const SDL_MouseMotionEvent& e) {}

void WOWarthog::update() {
	for (std::set<SDL_Keycode>::iterator it = this->keysPressed.begin(); it != this->keysPressed.end(); ++it) {
		if (*it == SDLK_UP || *it == SDLK_w) {
			this->drive(this->speed);
		}
		if (*it == SDLK_LEFT || *it == SDLK_a) {
			WO::rotateAboutRelZ(0.1f);
		}
		if (*it == SDLK_DOWN || *it == SDLK_s) {
			this->reverse(this->speed);
		}
		if (*it == SDLK_RIGHT || *it == SDLK_d) {
			WO::rotateAboutRelZ(-0.1f);
		}
	}
	if (this->drivingSound != nullptr) {
		if (this->isMoving()) {
			if (this->drivingSound->getIsPaused()) {
				this->drivingSound->setIsPaused(false);
			}
		} else {
			if (!this->drivingSound->getIsPaused()) {
				this->drivingSound->setIsPaused(true);
			}
		}
		this->drivingSound->setPosition(ISoundManager::toVec3df(this->getPosition()));
	}
	if (this->horn != nullptr) {
		this->horn->setPosition(ISoundManager::toVec3df(this->getPosition()));
	}
	this->driver->setPosition(this->getPosition() + Vector(-31, 0, 9)); 
	this->textLabel->setPosition(this->getPosition() + Vector(0, 0, 7));
}

void WOWarthog::fromNetMsg(NetMsgWarthog* netMsg) {
	this->setPosition(netMsg->position);
	this->setLookDirection(netMsg->direction);
	this->textLabel->setDisplayMatrix(netMsg->textMat);
}


void WOWarthog::drive(float distance) {
	for (int i = 0; i < distance; ++i) this->moveRelative(this->getLookDirection()); 
}

void WOWarthog::reverse(float distance) {
	for (int i = 0; i < distance; ++i) this->moveRelative(this->getLookDirection() * -1); 
}

void WOWarthog::onKeyDown(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyDown = key.keysym.sym;
	if (keyDown == SDLK_PLUS || keyDown == SDLK_EQUALS) {
		if (this->speed < 10) {
			++this->speed;
		} else if (this->speed < 0 || this->speed > 10) {
			//reset speed in case it becomes too large or negative
			this->speed = 0;
		}
		//else do nothing for now
	}
	if (keyDown == SDLK_MINUS) {
		if (this->speed > 0) {
			--this->speed;
		} else if (this->speed < 0 || this->speed > 10) {
			//reset speed in case it becomes too large or negative
			this->speed = 0;
		}
		//else do nothing for now
	}
	if (isMovementKey(keyDown)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyDown);
		if (found == keysPressed.end()) {
			keysPressed.insert(keyDown);
		}
	}
	if (keyDown == SDLK_SPACE && this->horn != nullptr) {
		if (!this->honking) {
			this->horn->setIsPaused(false);
			this->honking = true;
		}
	}
}

void WOWarthog::onKeyUp(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyUp = key.keysym.sym;
	if (isMovementKey(keyUp)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyUp);
		if (found != keysPressed.end()) {
			keysPressed.erase(found);
		}
	}
	if (keyUp == SDLK_SPACE && this->horn != nullptr) {
		if (honking) {
			this->horn->setIsPaused(true);
			honking = false;
		}
	}
}

bool WOWarthog::isMovementKey(SDL_Keycode key) {
	return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT ||
		key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d);
}

bool WOWarthog::isMoving() {
	return this->keysPressed.find(SDLK_UP) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_w) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_DOWN) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_s) != this->keysPressed.end();
}

float WOWarthog::toRads(float deg) { return std::tan((deg * Aftr::PI) / 180); }
float WOWarthog::toDeg(float rad) { return rad * (180 / Aftr::PI); }


#endif