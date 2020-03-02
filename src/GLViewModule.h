#pragma once

#include "GLView.h"
#include "AftrAPI.h"
#include "GameModuleAPI.h"
#include "ISoundManager.h"
#include "PxPhysicsAPI.h"
#include <irrKlang.h>

namespace Aftr {
    class Camera;
    class NetMessengerClient;
    class IPhysicsManager;

    class GLViewModule : public GLView {
        public:
            static GLViewModule* New(const std::vector< std::string >& outArgs);
            virtual ~GLViewModule() { ISoundManager::drop(); };
            virtual void init(float gravityScalar, Vector gravityNormalizedVector, std::string confFileName, const PHYSICS_ENGINE_TYPE& physicsEngineType);
            virtual void updateWorld(); ///< Called once per frame
            virtual void loadMap(); ///< Called once at startup to build this module's scene
            virtual void createNewModuleWayPoints();
            virtual void onResizeWindow(GLsizei width, GLsizei height) { GLView::onResizeWindow(width, height); };
            virtual void onMouseDown(const SDL_MouseButtonEvent& e);
            virtual void onMouseUp(const SDL_MouseButtonEvent& e);
            virtual void onMouseMove(const SDL_MouseMotionEvent& e);
            virtual void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
            virtual void onKeyDown(const SDL_KeyboardEvent& key);
            virtual void onKeyUp(const SDL_KeyboardEvent& key);

            void processKeyPress(const SDL_Keycode& key);
            void makeToilet(Vector pos = Vector(0, 0, 0));

            WOText* worldText;
            WOWarthog* warthog;
            WOGUILabel* guiText;
            ProgressBar* progressBar;
            WO* track_sphere;

            std::map<WOPhysx*, int> toilets;
            int numToilets = 0;

        protected:
            GLViewModule(const std::vector< std::string >& args);
            virtual void onCreate();
            void updatePhysics();
            bool isDriving();

            irrklang::ISound* citySound;
            IPhysicsManager* physics;
            NetMessengerClient* client;
            std::string startingText;
    };
}

