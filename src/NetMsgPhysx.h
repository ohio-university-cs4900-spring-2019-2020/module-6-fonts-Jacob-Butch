#pragma once

#include "AftrAPI.h"
#include "NetMsgMacroMethods.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgPhysx : public NetMsg {
		public:
			NetMsgMacroDeclaration(NetMsgPhysx);

			virtual ~NetMsgPhysx() {};
			virtual bool toStream(NetMessengerStreamBuffer& os) const;
			virtual bool fromStream(NetMessengerStreamBuffer& is);
			virtual void onMessageArrived();
			virtual std::string toString() const;

			Mat4 pose;
			Vector pos;
			int index;			
	};
}

#endif