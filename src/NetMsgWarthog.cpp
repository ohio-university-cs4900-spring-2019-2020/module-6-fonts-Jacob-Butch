#include "NetMsgWarthog.h"

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "Model.h"
#include "GLViewModule.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

NetMsgMacroDefinition(NetMsgWarthog);

NetMsgWarthog::NetMsgWarthog(const Vector& pos, const Vector& dir, Mat4 matrix) {
	this->position = pos;
	this->direction = dir;
	this->textMat = matrix;
}

NetMsgWarthog::~NetMsgWarthog() {}

bool NetMsgWarthog::toStream(NetMessengerStreamBuffer& os) const {
	os << this->position.x << this->position.y << this->position.z;
	os << this->direction.x << this->direction.y << this->direction.z;
	os << this->textMat[0] << this->textMat[1] << this->textMat[8] << this->textMat[9];
	return true;
}

bool NetMsgWarthog::fromStream(NetMessengerStreamBuffer& is) {
	is >> this->position.x >> this->position.y >> this->position.z;
	is >> this->direction.x >> this->direction.y >> this->direction.z;
	float mat[16] = {
		0, -1, 0, 0,
		0, 0, -1, 0,
		1, 0, 0, 0,
		0, 0, 0, 1
	};
	is >> mat[0] >> mat[1] >> mat[8] >> mat[9];
	this->textMat = Mat4(mat);
	return true;
}

void NetMsgWarthog::onMessageArrived() {
	((GLViewModule*)ManagerGLView::getGLView())->warthog->fromNetMsg(this);
	std::cout << this->toString() << std::endl;
}

std::string NetMsgWarthog::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "\tPosition:  " << this->position << "...\n";
	ss << "\tDirection: " << this->direction << "...\n";
	ss << "\tText Label: " << this->textMat << "...\n";
	return ss.str();
}

#endif