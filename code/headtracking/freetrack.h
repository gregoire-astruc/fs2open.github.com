
#ifndef HEADTRACKING_FREETRACK_H
#define HEADTRACKING_FREETRACK_H

#include "headtracking/headtracking.h"
#include "headtracking/headtracking_internal.h"

#include "external_dll/externalcode.h"

namespace headtracking
{
	namespace freetrack
	{
		struct FreeTrackData
		{
			unsigned long int dataID;
			long int camWidth;
			long int camHeight;

			float yaw;
			float pitch;
			float roll;
			float x;
			float y;
			float z;

			float rawyaw;
			float rawpitch;
			float rawroll;
			float rawx;
			float rawy;
			float rawz;

			float x1;
			float y1;
			float x2;
			float y2;
			float x3;
			float y3;
			float x4;
			float y4;
		};

		typedef bool (WINAPI *FTGetData_PTR)(FreeTrackData * data);
		typedef char *(WINAPI *FTGetDllVersion_PTR)(void);
		typedef void (WINAPI *FTReportID_PTR)(int name);
		typedef char *(WINAPI *FTProvider_PTR)(void);

		class FreeTrackLibrary : public SCP_ExternalCode
		{
		private:
			FTGetData_PTR mFTGetData;
			FTGetDllVersion_PTR mFTGetDllVersion;
			FTReportID_PTR mFTReportID;
			FTProvider_PTR mFTProvider;

			bool mEnabled;

		public:
			FreeTrackLibrary();

			~FreeTrackLibrary() {}

			bool GetData(FreeTrackData * data);

			char* GetDllVersion(void);

			void ReportID(int name);

			char* Provider(void);

			bool Enabled() { return mEnabled; }
		};

		class FreeTrackProvider : public internal::HeadTrackingProvider
		{
		private:
			FreeTrackLibrary library;

			FreeTrackData* data;

		public:
			virtual bool init();

			virtual void shutdown();

			virtual bool query(HeadTrackingStatus* statusOut);

			virtual SCP_string getName() const;
		};
	}
}

#endif // HEADTRACKING_FREETRACK_H
