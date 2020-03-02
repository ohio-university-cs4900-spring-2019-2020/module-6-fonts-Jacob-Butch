<<<<<<< HEAD
#pragma once

#include "WOWayPointSpherical.h"

namespace Aftr
{
    class WOWP1 : public WOWayPointSpherical {
        public:	
            static WOWP1* New(const WayPointParametersBase& params, float radius);
            virtual ~WOWP1();
            virtual void onTrigger();
        protected:
            WOWP1( const WayPointParametersBase& params, float radius );
    };
} //namespace Aftr

=======
#pragma once

#include "WOWayPointSpherical.h"

namespace Aftr
{
    class WOWP1 : public WOWayPointSpherical {
        public:	
            static WOWP1* New(const WayPointParametersBase& params, float radius);
            virtual ~WOWP1();
            virtual void onTrigger();
        protected:
            WOWP1( const WayPointParametersBase& params, float radius );
    };
} //namespace Aftr

>>>>>>> 824740c3fc2ea164db81d1212f41b04fbc846812
