#include "NetMsgPhysx.h"

#include <iostream>
#include <sstream>
#include <string>
#include "GLViewModule.h"
#include "WOPhysx.h"
#include "WorldContainer.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

NetMsgMacroDefinition(NetMsgPhysx);

bool NetMsgPhysx::toStream(NetMessengerStreamBuffer& os) const {
	os << pos.x << pos.y << pos.z;
	os << pose[0] << pose[1] << pose[2] << pose[3];
	os << pose[4] << pose[5] << pose[6] << pose[7];
	os << pose[8] << pose[9] << pose[10] << pose[11];
	os << pose[12] << pose[13] << pose[14] << pose[15];
	os << index;
	return true;
}

bool NetMsgPhysx::fromStream(NetMessengerStreamBuffer& is) {
	float x, y, z;
	is >> x >> y >> z;
	pos = Vector(x, y, z);
	is >> pose[0] >> pose[1] >> pose[2] >> pose[3];
	is >> pose[4] >> pose[5] >> pose[6] >> pose[7];
	is >> pose[8] >> pose[9] >> pose[10] >> pose[11];
	is >> pose[12] >> pose[13] >> pose[14] >> pose[15];
	is >> index;
	return true;
}

void NetMsgPhysx::onMessageArrived() {
	for (auto& i : ((GLViewModule*)ManagerGLView::getGLView())->toilets) {
		if (i.second == this->index) {
			WOPhysx* wo = i.first;
			wo->setDisplayMatrix(this->pose);
			wo->setPosition(this->pos);
			std::cout << this->toString() << std::endl;
		}
	}
}

std::string NetMsgPhysx::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "Message: " << std::endl;
	ss << "\tPosition:\t" << this->pos << std::endl;
	ss << "\tPose:\t\t" << this->pose << std::endl;
	ss << "\tIndex:\t\t" << this->index << std::endl;

	return ss.str();
}

#endif