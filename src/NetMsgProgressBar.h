<<<<<<< HEAD
#pragma once

#include "NetMsg.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgProgressBar : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgProgressBar);

		NetMsgProgressBar(const float width = 0.0f);
		virtual ~NetMsgProgressBar() {};
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		float width;

	};
}

=======
#pragma once

#include "NetMsg.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgProgressBar : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgProgressBar);

		NetMsgProgressBar(const float width = 0.0f);
		virtual ~NetMsgProgressBar() {};
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		float width;

	};
}

>>>>>>> 824740c3fc2ea164db81d1212f41b04fbc846812
#endif