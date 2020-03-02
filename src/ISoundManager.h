<<<<<<< HEAD
#pragma once

#include <irrKlang.h>
#include <iostream>
#include "Camera.h"
#include "Vector.h"
#include "SoundEndListener.h"

namespace Aftr {
	class ISoundManager {
		public:
			static void init();
			static void drop();
			static irrklang::ISoundEngine* getEngine() { return engine; };
			static irrklang::vec3df toVec3df(const Vector& vector);
			static void setListenerPosition(const Vector& pos, const Vector& lookdir, const Vector& vel, const Vector& updir);
		protected:
			static irrklang::ISoundEngine* engine;
	};

=======
#pragma once

#include <irrKlang.h>
#include <iostream>
#include "Camera.h"
#include "Vector.h"
#include "SoundEndListener.h"

namespace Aftr {
	class ISoundManager {
		public:
			static void init();
			static void drop();
			static irrklang::ISoundEngine* getEngine() { return engine; };
			static irrklang::vec3df toVec3df(const Vector& vector);
			static void setListenerPosition(const Vector& pos, const Vector& lookdir, const Vector& vel, const Vector& updir);
		protected:
			static irrklang::ISoundEngine* engine;
	};

>>>>>>> 824740c3fc2ea164db81d1212f41b04fbc846812
}