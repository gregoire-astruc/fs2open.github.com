
#ifndef HEADTRACKING_INTERNAL_H
#define HEADTRACKING_INTERNAL_H

#include "globalincs/pstypes.h"

#include "headtracking/headtracking.h"

namespace headtracking
{
	namespace internal
	{
		class HeadTrackingProvider
		{
		public:
			virtual ~HeadTrackingProvider() {}

			virtual bool init() = 0;

			virtual void shutdown() = 0;

			virtual bool query(HeadTrackingStatus* statusOut) = 0;

			virtual SCP_string getName() const = 0;
		};
	}
}

#endif // HEADTRACKING_INTERNAL_H
