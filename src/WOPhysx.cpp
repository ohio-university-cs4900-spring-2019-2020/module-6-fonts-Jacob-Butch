#include "WOPhysx.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;
using namespace physx;

WOPhysx* WOPhysx::New(physx::PxRigidActor* actor, const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WOPhysx* wo = new WOPhysx(actor);
	wo->onCreate(modelFileName, scale, shadingType);
	return wo;
}

WOPhysx::WOPhysx(physx::PxRigidActor* actor) : IFace(this), WO() {
	this->actor = actor;
}

void WOPhysx::onCreate(const std::string& modelFileName, Vector scale, MESH_SHADING_TYPE shadingType) {
	WO::onCreate(modelFileName, scale, shadingType);
}

#endif