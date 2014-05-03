
#include "headtracking/trackir.h"

#include "external_dll/trackirpublic.h"

#include "osapi/osapi.h"

#include <SDL_syswm.h>

namespace headtracking
{
	namespace trackir
	{
		bool TrackIRProvider::init()
		{
			// calling the function that will init all the function pointers for TrackIR stuff (Swifty)
			int trackIrInitResult = gTirDll_TrackIR.Init(os_get_window());
			if (trackIrInitResult != SCP_INITRESULT_SUCCESS)
			{
				mprintf(("TrackIR Init Failed - %d\n", trackIrInitResult));
				return false;
			}
			else
			{
				return true;
			}
		}

		void TrackIRProvider::shutdown()
		{
			gTirDll_TrackIR.Close();
		}

		bool TrackIRProvider::query(HeadTrackingStatus* statusOut)
		{
			gTirDll_TrackIR.Query();

			statusOut->pitch = gTirDll_TrackIR.GetPitch();
			statusOut->roll = gTirDll_TrackIR.GetRoll();
			statusOut->yaw = gTirDll_TrackIR.GetYaw();

			statusOut->x = gTirDll_TrackIR.GetX();
			statusOut->y = gTirDll_TrackIR.GetY();
			statusOut->z = gTirDll_TrackIR.GetZ();

			return true;
		}

		SCP_string TrackIRProvider::getName() const
		{
			return "TrackIR";
		}
	}
}
