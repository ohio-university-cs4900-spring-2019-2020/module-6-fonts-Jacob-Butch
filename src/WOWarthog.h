#pragma once

#include <irrKlang.h>
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"
#include "WOText.h"
#include "NetMsgWarthog.h"
#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMessengerClient;
	class IPhysicsManager;

	class WOWarthog : public WO {
		public:
			WOMacroDeclaration(WOWarthog, WO);

			static WOWarthog* New(IPhysicsManager* physics);
			virtual ~WOWarthog();
			virtual void onCreate(const std::string& modelFileName, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);

			//User Keyboard Input Specific
			//-----------------------------------
			virtual void onKeyDown(const SDL_KeyboardEvent& key);
			virtual void onKeyUp(const SDL_KeyboardEvent& key);
			virtual void onMouseUp(const SDL_MouseButtonEvent& e);
			virtual void onMouseMove(const SDL_MouseMotionEvent& e);

			void setSpeed(float newSpeed) { this->speed = newSpeed; }
			float getSpeed() { return this->speed; }

			virtual Mat4 getDisplayMatrix() { return WO::getDisplayMatrix(); }
			virtual void setDisplayMatrix(Mat4 mat) { WO::getModel()->setDisplayMatrix(mat); }
			virtual void setLookDirection(Vector dir) { WO::getModel()->setLookDirection(dir); }

			Mat4 getTextLabelMatrix() { return textLabel->getDisplayMatrix(); }

			// Moves the warthog forward or backwards by the distance
			void drive(float distance = 0.1f);
			void reverse(float distance = 0.1f);
			// Proccess the keys pressed
			void update();
			void updateLabel();
			bool isMoving();
			bool hasDriver() { return driver != nullptr; };
			void setDriver(Camera* newDriver);
			Camera* getDriver() { return driver; }

			float toRads(float deg);
			float toDeg(float rad);

			void fromNetMsg(NetMsgWarthog* netMsg);
			physx::PxRigidActor* actor;
			WOText* textLabel;
			irrklang::ISound* drivingSound;
			irrklang::ISound* horn;

		protected:
			Camera* driver;
			IPhysicsManager* physics;
			std::set<SDL_Keycode> keysPressed;
			float speed;
			float distanceFromWarthog = 32.28f;
			float angleAround = 0;
			float pitch = 16.1892f;
			bool honking;

			WOWarthog(IPhysicsManager* physics);
			bool isMovementKey(SDL_Keycode key);
			
	};

}

#endif

