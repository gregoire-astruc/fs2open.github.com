
#include "headtracking/headtracking.h"
#include "headtracking/headtracking_internal.h"

#include "headtracking/trackir.h"
#include "headtracking/freetrack.h"

#include "external_dll/trackirpublic.h"

namespace headtracking
{
	internal::HeadTrackingProvider* currentProvider = nullptr;
	HeadTrackingStatus* status = nullptr;

	template<class Provider>
	bool initProvider()
	{
		Assert(currentProvider == nullptr);

		currentProvider = new Provider();
		
		if (currentProvider->init())
		{
			mprintf(("    Successfully initialized '%s'\n", currentProvider->getName().c_str()));
			return true;
		}
		else
		{
			mprintf(("    Failed to initialize '%s'.\n", currentProvider->getName().c_str()));

			delete currentProvider;
			currentProvider = nullptr;
			
			return false;
		}
	}

	bool init()
	{
		mprintf(("Initializing head tracking...\n"));

		status = new HeadTrackingStatus();

		if (initProvider<trackir::TrackIRProvider>())
		{
			return true;
		}

		if (initProvider<freetrack::FreeTrackProvider>())
		{
			return true;
		}

		return true;
	}

	bool isEnabled()
	{
		return currentProvider != nullptr;
	}

	bool query()
	{
		if (currentProvider == nullptr)
		{
			return false;
		}

		return currentProvider->query(status);
	}

	HeadTrackingStatus* getStatus()
	{
		return status;
	}

	void shutdown()
	{
		if (currentProvider != nullptr)
		{
			currentProvider->shutdown();

			delete currentProvider;
			currentProvider = nullptr;
		}

		if (status != nullptr)
		{
			delete status;
			status = nullptr;
		}
	}
}
