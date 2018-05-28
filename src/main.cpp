#include "SampleViewer.h"

int main(int argc, char** argv)
{
#ifdef DEPTH
	int rc = openni::STATUS_OK;
	const char* deviceURI = openni::ANY_DEVICE;
#else
	int rc = 1;
	const char* deviceURI = NULL;
#endif

#ifdef DEPTH
printf("main::Compiled with Depth\n");
#else
printf("main::Compiled witout Depth\n");
#endif

	/*if (argc > 1)
	{
		deviceURI = argv[1];
	}*/
	SampleViewer sampleViewer("Body Skeleton Tracker", deviceURI);
	rc = sampleViewer.init();
#ifdef DEPTH
	if (rc != openni::STATUS_OK)
#else
	if (rc != 0)
#endif
	{
		return 1;
	}
	sampleViewer.run();
}

