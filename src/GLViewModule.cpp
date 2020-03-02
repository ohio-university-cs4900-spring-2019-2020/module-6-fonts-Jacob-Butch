#include "GLViewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrGLRendererBase.h"

using namespace Aftr;
using namespace irrklang;
using namespace physx;

const std::string sharedMM = ManagerEnvironmentConfiguration::getSMM();

GLViewModule* GLViewModule::New( const std::vector< std::string >& args ){
    GLViewModule* glv = new GLViewModule( args );
    glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
    glv->onCreate();
    return glv;
}

void GLViewModule::init(float gravityScalar, Vector gravityNormalizedVector, std::string configFileName, const PHYSICS_ENGINE_TYPE& physicsEngineType) {
    ISoundManager::init();
    GLView::init(gravityScalar, gravityNormalizedVector, configFileName, physicsEngineType);
}

GLViewModule::GLViewModule( const std::vector< std::string >& args ) : GLView( args ){
    //Initialize any member variables that need to be used inside of LoadMap() here.
    //Note: At this point, the Managers are not yet initialized. The Engine initialization occurs immediately after this method returns 
    //(see GLViewNewModule::New() for reference). Then the engine invoke's GLView::loadMap() for this module.
    //After loadMap() returns, GLView::onCreate is finally invoked.
    //The order of execution of a module startup: GLView::New() is invoked: calls GLView::init() -> GLView::loadMap() -> GLView::onCreate()
    //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}

void GLViewModule::onCreate(){
    //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
    if( this->pe != NULL ){
        //optionally, change gravity direction and magnitude here. The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector( Vector( 0, 0,-1.0f ) );
        this->pe->setGravityScalar( Aftr::GRAVITY );
    }
    // setup physics
    this->physics = IPhysicsManager::New();

    this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode

    // add warthog
    this->warthog = WOWarthog::New(this->physics);
    worldLst->push_back(this->warthog);
    worldLst->push_back(this->warthog->textLabel);

    // setup background city noises
    this->citySound = ISoundManager::getEngine()->play2D("../mm/sounds/citysounds.ogg", true);  

    // setup world text
    this->startingText = "Hit the '/' key to type a message here";
    this->worldText = WOText::New(sharedMM + "/fonts/arial.ttf", 72, this->startingText, "World Text");
    this->worldText->setColor(aftrColor4f(1.0f, 0.0f, 0.0f, 1.0f));
    this->worldText->setSize(30, 10);
    this->worldText->setPosition(20, 0, 20);
    worldLst->push_back(this->worldText);

    // make gui
    this->guiText = WOGUILabel::New(nullptr);
    this->guiText->setText("Press 't' to summon a toilet!");
    this->guiText->setColor(255, 0, 0, 255);
    this->guiText->setFontSize(30); //font size is correlated with world size
    this->guiText->setPosition(Vector(0, 1, 0));
    this->guiText->setFontOrientation(FONT_ORIENTATION::foLEFT_TOP);
    this->guiText->setFontPath(sharedMM + "/fonts/arial.ttf");
    worldLst->push_back(this->guiText);

    // make progressbar
    this->progressBar = ProgressBar::New();
    worldLst->push_back(this->progressBar->bar);
    worldLst->push_back(this->progressBar->progress);
    
    // setup messenger client
    if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
        this->client = NetMessengerClient::New("127.0.0.1", "12682");
    } else {
        this->client = NetMessengerClient::New("127.0.0.1", "12683");
    }   

    PxMaterial* gMaterial = this->physics->physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxRigidStatic* groundPlane = PxCreatePlane(*this->physics->physics, PxPlane(PxVec3(0, 0, 1), 0), *gMaterial);
    this->physics->scene->addActor(*groundPlane);
}

void GLViewModule::updateWorld(){
    GLView::updateWorld();  
    ISoundManager::setListenerPosition(this->cam->getPosition(), this->cam->getLookDirection(), Vector(0, 0, 0), this->cam->getNormalDirection());
    // update warthog
    if (this->isDriving()) {
        this->warthog->update();
        if (this->client->isTCPSocketOpen()) {
            this->client->sendNetMsgSynchronousTCP(NetMsgWarthog(this->warthog->getPosition(), this->warthog->getLookDirection(), this->warthog->getTextLabelMatrix()));
        }
    }
    this->warthog->updateLabel();
    // update world text
    if (this->client->isTCPSocketOpen()) {
        if (this->worldText->getText() != this->startingText) {
            this->client->sendNetMsgSynchronousTCP(NetMsgTextWO(this->worldText->getText()));
        }
        if (this->progressBar != nullptr) {
            this->client->sendNetMsgSynchronousTCP(NetMsgProgressBar(this->progressBar->getProgressWidth()));
        }
    }
    // update physics
    this->physics->simulate(this->client, this->toilets);
    // update progress bar
    if (this->progressBar != nullptr) {
        if (this->progressBar->fill) {
            this->progressBar->fillBar();
        }
        else if (this->progressBar->empty) {
            this->progressBar->emptyBar();
        }
        if (this->progressBar->isFull()) {
            this->makeToilet();
            this->progressBar->reset();
        }
    }
}

void GLViewModule::onKeyDown( const SDL_KeyboardEvent& key ){
    if (!this->worldText->typing) {
        GLView::onKeyDown(key);
        if (this->isDriving()) this->warthog->onKeyDown(key);
    }
    this->processKeyPress(key.keysym.sym);
}

void GLViewModule::onKeyUp(const SDL_KeyboardEvent& key) {
    GLView::onKeyUp(key);
    if (key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT) { this->worldText->shift = false; }
    if (this->isDriving()) { this->warthog->onKeyUp(key); }   
}

void GLViewModule::onMouseDown(const SDL_MouseButtonEvent& e) {
    GLView::onMouseDown(e);
    if (this->isDriving()) { this->warthog->onMouseDown(e); }
}

void GLViewModule::onMouseUp(const SDL_MouseButtonEvent& e) {
    GLView::onMouseUp(e);
    if (this->isDriving()) { this->warthog->onMouseUp(e); }
}

void GLViewModule::onMouseMove(const SDL_MouseMotionEvent& e) {
    GLView::onMouseMove(e);
    if (this->isDriving()) { this->warthog->onMouseMove(e); }
}

void GLViewModule::onMouseWheelScroll(const SDL_MouseWheelEvent& e) {
    GLView::onMouseWheelScroll(e);
    if (this->isDriving()) { this->warthog->onMouseWheelScroll(e); }
}

void GLViewModule::loadMap(){
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();

    ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = false;
    this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+
    this->cam->setPosition(-25, 0, 15);  
    
    float ga = 0.1f; //Global Ambient Light level for this module
    ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
    WOLight* light = WOLight::New();
    light->isDirectionalLight(true);
    light->setPosition(Vector(0, 0, 100));
    //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
    //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
    light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
    light->setLabel("Light");
    worldLst->push_back(light);
    
    //Create the SkyBox
    WO* wo = WOSkyBox::New(sharedMM + "/images/skyboxes/space_thick_rb+6.jpg", this->getCameraPtrPtr());
    wo->setPosition(Vector(0, 0, 0));
    wo->setLabel("Sky Box");
    wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    worldLst->push_back(wo);

    ///*
    wo = WO::New(sharedMM + "/models/citytexday_3ds/city_tex_day.3DS", Vector(1, 1, 1), MESH_SHADING_TYPE::mstAUTO);
    wo->setPosition(Vector(0, 0, 1350));
    wo->renderOrderType = RENDER_ORDER_TYPE::roOVERLAY;
    wo->setLabel("City");
    worldLst->push_back(wo);
    //*/
    createNewModuleWayPoints();
}

void GLViewModule::createNewModuleWayPoints() {
    // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
    WayPointParametersBase params(this);
    params.frequency = 5000;
    params.useCamera = true;
    params.visible = false;
    WOWayPointSpherical* wayPt = WOWP1::New(params, 3);
    wayPt->setPosition(Vector(50, 0, 3));
    worldLst->push_back(wayPt);
}

void GLViewModule::processKeyPress(const SDL_Keycode& key) {
    if (this->worldText != nullptr && this->worldText->typing) {
        switch (key) {
            case SDLK_SLASH:
                this->worldText->typing = false;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                if (!this->worldText->shift) {
                    this->worldText->shift = true;
                }
                break;
            case SDLK_RETURN:
                // send message
                if (this->client->isTCPSocketOpen()) {
                    this->client->sendNetMsgSynchronousTCP(NetMsgTextWO(this->worldText->getText()));
                }
            case SDLK_ESCAPE:
                this->worldText->typing = false;
                break;
            default:
                this->worldText->type(key);
        }
        return;
    }

    switch (key) {
        case SDLK_SLASH:
            if (this->worldText != nullptr) {
                this->worldText->typing = true;
                if (this->worldText->getText() == this->startingText) {
                    this->worldText->setText("");
                }
            }
            return;
        case SDLK_f:
            if (this->warthog != nullptr) {
                if (this->cam != nullptr && !this->warthog->hasDriver()) {
                    this->warthog->setDriver(this->cam);
                } else {
                    this->warthog->setDriver(nullptr);
                }
            }
            break;
        case SDLK_w:
        case SDLK_UP:
            if (!this->isDriving() && this->cam != nullptr) this->cam->moveInLookDirection(10); 
            break;
        case SDLK_a:
        case SDLK_LEFT:
            if (!this->isDriving() && this->cam != nullptr) {
                for (int i = 0; i < 10; i++) this->cam->moveLeft(); 
            }
            break;
        case SDLK_s:
        case SDLK_DOWN:
            if (!this->isDriving() && this->cam != nullptr) this->cam->moveOppositeLookDirection(10); 
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            if (!this->isDriving() && this->cam != nullptr) {
                for (int i = 0; i < 10; i++) this->cam->moveRight(); 
            }
            break;
        case SDLK_0:
            if(this->cam != nullptr) this->cam->setPosition(Vector(0, 0, 0));
            break;
        case SDLK_t:
            this->makeToilet();
            break;
        case SDLK_o:
            ISoundManager::getEngine()->play2D("../mm/sounds/oof.mp3");
            break;
        case SDLK_6:
            if (this->progressBar != nullptr) this->progressBar->toggleEmpty();
            break;
        case SDLK_7:
            if (this->progressBar != nullptr) this->progressBar->toggleFill();
            break;
        default: break;
    }
}


bool GLViewModule::isDriving() { return this->warthog != nullptr && this->warthog->hasDriver(); }

void GLViewModule::makeToilet(Vector pos) {
    if (pos == Vector(0, 0, 0)) {
        pos = this->cam != nullptr ? this->cam->getPosition() : Vector(0, 0, 0);
    }
    PxTransform t = PxTransform(PxVec3(pos.x, pos.y, pos.z));
    PxShape* shape = this->physics->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->physics->physics->createMaterial(0.5f, 0.3f, 0.2f));
    PxRigidDynamic* actor = PxCreateDynamic(*this->physics->physics, t, *shape, 10.0f);
    
    // Add the item to the physx world
    WOPhysx* wo = WOPhysx::New(actor, "../mm/models/toilet/model.dae", Vector(15, 15, 15));
    wo->actor->userData = wo;
    wo->setPosition(pos);
    worldLst->push_back(wo);
    this->physics->scene->addActor(*wo->actor);
    
    // Play toilet flush sound :)
    ISoundManager::getEngine()->play2D("../mm/sounds/toilet.wav");

    // Send new object net message to spawn a new object
    if (this->client != nullptr && this->client->isTCPSocketOpen()) {
        NetMsgToilet msg;
        msg.pos = pos;
        this->client->sendNetMsgSynchronousTCP(msg);
    }
    
    // Register it to the local storage
    this->toilets.insert(std::pair(wo, numToilets));
    numToilets++;
    if (this->numToilets == 1) {
        this->guiText->setText(std::to_string(this->numToilets) + " toilet");
    } else {
        this->guiText->setText(std::to_string(this->numToilets) + " toilets");
    }
    
}

/*
std::string GLViewModule::getBackground() {
    switch (background) {
    case 0: return "/images/skyboxes/sky_water+6.jpg";
    case 1: return "/images/skyboxes/sky_dust+6.jpg";
    case 2: return "/images/skyboxes/sky_mountains+6.jpg";
    case 3: return "/images/skyboxes/sky_winter+6.jpg";
    case 4: return "/images/skyboxes/early_morning+6.jpg";
    case 5: return "/images/skyboxes/sky_afternoon+6.jpg";
    case 6: return "/images/skyboxes/sky_cloudy+6.jpg";
    case 7: return "/images/skyboxes/sky_cloudy3+6.jpg";
    case 8: return "/images/skyboxes/sky_day+6.jpg";
    case 9: return "/images/skyboxes/sky_day2+6.jpg";
    case 10: return "/images/skyboxes/sky_deepsun+6.jpg";
    case 11: return "/images/skyboxes/sky_evening+6.jpg";
    case 12: return "/images/skyboxes/sky_morning+6.jpg";
    case 13: return "/images/skyboxes/sky_morning2+6.jpg";
    case 14: return "/images/skyboxes/sky_noon+6.jpg";
    case 15: return "/images/skyboxes/sky_warp+6.jpg";
    case 16: return "/images/skyboxes/space_Hubble_Nebula+6.jpg";
    case 17: return "/images/skyboxes/space_gray_matter+6.jpg";
    case 18: return "/images/skyboxes/space_easter+6.jpg";
    case 19: return "/images/skyboxes/space_hot_nebula+6.jpg";
    case 20: return "/images/skyboxes/space_ice_field+6.jpg";
    case 21: return "/images/skyboxes/space_lemon_lime+6.jpg";
    case 22: return "/images/skyboxes/space_milk_chocolate+6.jpg";
    case 23: return "/images/skyboxes/space_solar_bloom+6.jpg";
    case 24: return "/images/skyboxes/space_thick_rb+6.jpg";
    default:
        background = 0;
        return getBackground();
    }
}
*/

////Create the infinite grass plane (the floor)
/*
wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
wo->setPosition( Vector( 0, 0, 0 ) );
wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiny" it is)
grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
wo->setLabel( "Grass" );
worldLst->push_back( wo );
*/