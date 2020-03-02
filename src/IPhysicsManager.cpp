#include "IPhysicsManager.h"

#include <iostream>
#include "WOPhysx.h"
#include "NetMsgPhysx.h"
#include "Model.h"

using namespace Aftr;
using namespace physx;

IPhysicsManager::IPhysicsManager() {
    this->foundation = PxCreateFoundation(version, gAllocator, gErrorCallback);
    this->gPvd = PxCreatePvd(*this->foundation);
    this->gPvd->connect(*PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10), PxPvdInstrumentationFlag::eALL);

    this->physics = PxCreateBasePhysics(version, *this->foundation, PxTolerancesScale(), true, this->gPvd);
    this->gCooking = PxCreateCooking(version, *this->foundation, PxCookingParams(PxTolerancesScale()));

    this->gMaterial = this->physics->createMaterial(0.5f, 0.5f, 0.6f);

    physx::PxSceneDesc gSceneDesc(this->physics->getTolerancesScale());
    gSceneDesc.gravity = (PxVec3(0.0f, 0.0f, -9.81f));
    gSceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    this->gDispatcher = PxDefaultCpuDispatcherCreate(4);
    gSceneDesc.cpuDispatcher = this->gDispatcher;
    gSceneDesc.filterShader = PxDefaultSimulationFilterShader;
    this->scene = this->physics->createScene(gSceneDesc);
    this->gClient = scene->getScenePvdClient();
    if (this->gClient) {
        this->gClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        this->gClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        this->gClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

IPhysicsManager::~IPhysicsManager() {
    this->scene->release();
    this->foundation->release();
    this->physics->release();
}   

void IPhysicsManager::simulate(NetMessengerClient* client, std::map<WOPhysx*, int> toilets) {
    this->scene->simulate(ManagerSDLTime::getTimeSinceLastPhysicsIteration() / 1000.0f);
    this->scene->fetchResults(true);
    PxU32 num_transforms = 0;
    PxActor** activeActors = this->scene->getActiveActors(num_transforms);
    for (PxU32 i = 0; i < num_transforms; i++) {
        WOPhysx* wo = static_cast<WOPhysx*>(activeActors[i]->userData); // Get the pair
        if (wo != nullptr && wo->actor != nullptr) {
            PxTransform t = wo->actor->getGlobalPose(); // Get the transform
            PxMat44 pose = PxMat44(t); // Get the physx pose matrix
            // Collect the new pose into an aftr-acceptable state
            float convert[16] = { pose(0, 0), pose(0, 1), pose(0, 2), pose(3, 0),
                                  pose(1, 0), pose(1, 1), pose(1, 2), pose(3, 1),
                                  pose(2, 0), pose(2, 1), pose(2, 2), pose(3, 2),
                                  pose(0, 3), pose(1, 3), pose(2, 3), pose(3, 3)
            };

            Mat4 mat(convert);
            // Send orientation net message to update an item's orientation
            if (client != nullptr && client->isTCPSocketOpen()) {
                NetMsgPhysx msg;
                msg.pos = Vector(mat[12], mat[13], mat[14]);
                msg.pose = mat;
                msg.index = toilets[wo];
                client->sendNetMsgSynchronousTCP(msg);
            }

            // Apply the new pose
            wo->setDisplayMatrix(mat);
            wo->setPosition(mat[12], mat[13], mat[14]);
        }
    }
}
