#pragma once

#include "NetMsg.h"
#include "Vector.h"
#include "Mat4.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgWarthog: public NetMsg {
		public:
			NetMsgMacroDeclaration(NetMsgWarthog);

			NetMsgWarthog(const Vector& pos = Vector(0, 0, 0), const Vector& dir = Vector(0, 0, 0), Mat4 matrix = Mat4());
			virtual ~NetMsgWarthog();
			virtual bool toStream(NetMessengerStreamBuffer& os) const;
			virtual bool fromStream(NetMessengerStreamBuffer& is);
			virtual void onMessageArrived();
			virtual std::string toString() const;

			Vector position;
			Vector direction;
			Mat4 textMat;
			
	};
}

#endif