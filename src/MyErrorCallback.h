#pragma once

#include "foundation/PxErrorCallback.h"

//#ifdef AFTR_CONFIG_USE_BOOST

namespace physx {
	class MyErrorCallback : public PxErrorCallback {
	public:

		virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {};

	};
}

//#endif