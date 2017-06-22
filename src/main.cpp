/*****************************************************************************
*                                                                            *
*  OpenNI 2.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#include "Viewer.h"

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
printf("main::Compilado com Depth\n");
#else
printf("main::Compilado SEM Depth\n");
#endif

	/*if (argc > 1)
	{
		deviceURI = argv[1];
	}*/

	SampleViewer sampleViewer("Skeleton Tracker", deviceURI);

	rc = sampleViewer.init(argc, argv);

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
