#pragma once

#include "NetMsg.h"
#include "Vector.h"
#include "Mat4.h"
#include "NetMsgMacroMethods.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgToilet : public NetMsg {
		public:
			NetMsgMacroDeclaration(NetMsgToilet);

			virtual ~NetMsgToilet() {};
			virtual bool toStream(NetMessengerStreamBuffer& os) const;
			virtual bool fromStream(NetMessengerStreamBuffer& is);
			virtual void onMessageArrived();
			virtual std::string toString() const;

			Vector pos;
		protected:
			
	};
}

#endif