
#ifndef HEADTRACKING_TRACKIR_H
#define HEADTRACKING_TRACKIR_H

#include "headtracking/headtracking.h"
#include "headtracking/headtracking_internal.h"

namespace headtracking
{
	namespace trackir
	{
		class TrackIRProvider : public internal::HeadTrackingProvider
		{
		public:
			virtual bool init();

			virtual void shutdown();

			virtual bool query(HeadTrackingStatus* statusOut);

			virtual SCP_string getName() const;
		};
	}
}

#endif // HEADTRACKING_TRACKIR_H
