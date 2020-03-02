#pragma once

#include "WO.h"
#include "Model.h"
#include "IPhysicsManager.h"
#include "PxPhysicsAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMessengerClient;
	class IPhysicsManager;

	class WOPhysx : public WO {
		public:
			WOMacroDeclaration(WOPhysx, WO);

			static WOPhysx* New(
				physx::PxRigidActor* actor = nullptr, 
				const std::string& modelFileName = ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl",
				Vector scale = Vector(1, 1, 1), 
				MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO
			);
			virtual void onCreate(const std::string& modelFileName, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);

			physx::PxRigidActor* actor;
			void setDisplayMatrix(Mat4 matrix) { WO::getModel()->setDisplayMatrix(matrix); }

		protected:
			WOPhysx(physx::PxRigidActor* actor);
	};
}

#endif