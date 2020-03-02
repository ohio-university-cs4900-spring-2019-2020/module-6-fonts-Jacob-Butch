#pragma once

#include "PxPhysicsAPI.h"
#include "NetMessengerClient.h"
#include <map>

namespace Aftr {
	class WOPhysx;
	class ModelDataSharedID;

	class IPhysicsManager {
		public:
			IPhysicsManager();
			static IPhysicsManager* New() { return new IPhysicsManager(); };
			~IPhysicsManager();
			void simulate(NetMessengerClient* client, std::map<WOPhysx*, int> toilets);
			physx::PxPhysics* physics;
			physx::PxScene* scene;

		protected:

		private:
			physx::PxFoundation* foundation;
			physx::PxU32 version = PX_PHYSICS_VERSION;
			physx::PxDefaultErrorCallback gErrorCallback;
			physx::PxDefaultAllocator gAllocator;
			physx::PxPvd* gPvd;
			physx::PxCooking* gCooking;
			physx::PxDefaultCpuDispatcher* gDispatcher;
			physx::PxMaterial* gMaterial;
			physx::PxPvdSceneClient* gClient;

	};
}


