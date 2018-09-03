#include "SampleViewer.h"

#include <math.h>
#include <string.h>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "DrawAux.h"

using namespace cv;
using namespace std;

#ifdef DEPTH
#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms
using namespace openni;
#endif

SampleViewer* SampleViewer::ms_self = NULL;

SampleViewer::SampleViewer(const char* strSampleName, const char* deviceUri)
{
#ifdef DEPTH
	initOpenNI(deviceUri);
	
	skelD = NULL;
#else
    if( deviceUri==NULL )
    {
        int camera = 0;
        if(!capture.open(camera))
            cout << "Capture from camera #" <<  camera << " didn't work" << endl;
    }
    else
    {
        Mat image = imread( deviceUri, 1 );
        if( image.empty() )
        {
            if(!capture.open( deviceUri ))
                cout << "Could not read " << deviceUri << endl;
        }
    }
#endif
	ms_self = this;
	strncpy(m_strSampleName, strSampleName, strlen(strSampleName));

	skel = NULL;
	subSample = 2;
	frameCount = 0;
}

#ifdef DEPTH
int SampleViewer::initOpenNI(const char* deviceUri) {
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	rc = device.open(deviceUri);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	// Get the device vender and name, if Asus Xtion or Primesense set it's resolution to 640x480.
	openni::Array<openni::DeviceInfo> deviceInfoList;
	OpenNI::enumerateDevices(&deviceInfoList);
	for (int i = 0; i < deviceInfoList.getSize(); i++)
	{
		printf(//"%d: Uri: %s\n"
		    "Vendor: %s"
		    ", Name: %s\n", /*i, deviceInfoList[i].getUri(),*/ deviceInfoList[i].getVendor(), deviceInfoList[i].getName());
		    
		//if (strcmp(deviceInfoList[i].getVendor(), "PrimeSense")==0) // Asus Xtion and Primesense devices
		{
			// set resolution
			// depth modes
			cout << "Depth modes" << endl;
			const openni::SensorInfo* sinfo = device.getSensorInfo(openni::SENSOR_DEPTH);
			const openni::Array< openni::VideoMode>& modesDepth = sinfo->getSupportedVideoModes();
			bool set = false;
			for (int i = 0; i < modesDepth.getSize(); i++) {
				printf("%i: %ix%i, %i fps, %i format\n", i, modesDepth[i].getResolutionX(), modesDepth[i].getResolutionY(),
				modesDepth[i].getFps(), modesDepth[i].getPixelFormat()); //PIXEL_FORMAT_DEPTH_1_MM = 100, PIXEL_FORMAT_DEPTH_100_UM
			        if (!set && modesDepth[i].getResolutionX() == 640 && modesDepth[i].getResolutionY() == 480) {
					rc = depth.setVideoMode(modesDepth[i]); // 640x480
					set = true;     	
					if (openni::STATUS_OK != rc)
					    cout << "error: depth fromat not supprted..." << endl;
				}
			}
		}
	}


	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}
}


/**
 * Ler o proximo frame e o retorna.
 **/
VideoFrameRef * SampleViewer::getNextFrame() {
	int changedStreamDummy;
	VideoStream* pStream = &depth;
	int rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
	if (rc != STATUS_OK)
	{
		printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
		return NULL;
	}
	
	openni::VideoFrameRef *frame = new VideoFrameRef();
	rc = depth.readFrame(frame);
	if (rc != STATUS_OK)
	{
		printf("Read failed!\n%s\n", OpenNI::getExtendedError());
		return NULL;
	}

	if (frame->getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame->getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
	{
		printf("Unexpected frame format\n");
		return NULL;
	}

	return frame;
}


Point3D* SampleViewer::getClosestPoint(openni::VideoFrameRef *frame, Point3D *furthest) {
	Point3D *closestPoint = new Point3D();
	DepthPixel* pDepth = (DepthPixel*)frame->getData();
	//DepthPixel p;
	bool found = false;
	closestPoint->z = 0xffff;
	furthest->z = 0;
	int width = frame->getWidth();
	int height = frame->getHeight();
	
	//printf("new way::%d::%d\n", width, height);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			//p = pDepth[y*width + x]; 
			if (*pDepth < closestPoint->z && *pDepth != 0)
			{
				closestPoint->x = x;
				closestPoint->y = y;
				closestPoint->z = *pDepth;
				//closestPoint->z = p;
				found = true;
			}
			else if (*pDepth > furthest->z && *pDepth != 0)
			{
				furthest->x = x;
				furthest->y = y;
				furthest->z = *pDepth;
			}
		}
		
	//printf("closest::%d\n", closestPoint->z);

	if (!found)
	{
		return NULL;
	}

	return closestPoint;
}
#endif

SampleViewer::~SampleViewer()
{
	finalize();

	if (m_pTexMap)
		delete[] m_pTexMap;

	ms_self = NULL;

	if (skel)
		delete skel;
#ifdef DEPTH
	if (skelD)
		delete skelD;
#endif
	if (outputVideo) {
		outputVideo->release();
		delete outputVideo;
	}
}

void SampleViewer::finalize()
{
#ifdef DEPTH
	depth.stop();
	depth.destroy();
	device.close();
	OpenNI::shutdown();
#endif
}

int SampleViewer::init()
{
	m_pTexMap = NULL;

	return 0;
}

int SampleViewer::run()	//Does not return
{
#ifdef DEPTH
printf("Compiled with Depth\n");
#else
printf("Compiled without Depth\n");
#endif

	while (1) {
		display();
		char c = (char)waitKey(10);
		//char c = (char)waitKey(100);
		//char c = (char)waitKey(1000);
	        if( c == 27 || c == 'q' || c == 'Q' )
        	        break;
	}
	//int r = system("killall -9 rostopic");
#ifdef DEPTH
	return openni::STATUS_OK;
#else
	return 0;
#endif
}
void SampleViewer::display()
{
	int sizePixel = 3;
	Point3D * furthest = NULL;
	Point3D * closest = NULL;
#ifdef DEPTH
	sizePixel = sizeof(openni::RGB888Pixel);
	openni::VideoFrameRef * srcFrame = getNextFrame();

	if (srcFrame==NULL)
		return;
	furthest = new Point3D();
	closest = getClosestPoint(srcFrame, furthest);

#else
	Mat srcFrame;
	srcFrame.data = NULL;
	Mat framec;
	framec.data = NULL;
	if( capture.isOpened() ) {
		capture >> framec;
		if(! framec.empty() ) {
			srcFrame = cv::Mat(framec.size(), CV_8UC1);
			cvtColor(framec, srcFrame, CV_RGB2GRAY);
			//srcFrame = framec.clone();
		}
		else
			printf("null frame\n");
	}
#endif


	// So entra nesses if na primeira vez
#ifdef DEPTH
	if (m_pTexMap == NULL)
	{
		// Texture map init
		width = srcFrame->getWidth();
		height = srcFrame->getHeight();
		if (width==320)
			subSample = 1;
#else
	if (m_pTexMap == NULL && srcFrame.data!=NULL)
	{
		// TODO pegar da webcam opencv
		width = srcFrame.cols;
		height = srcFrame.rows;
#endif
printf("w x h = %d x %d\n", width, height);
		m_pTexMap = new unsigned char[width * height * sizePixel];

		skel = new Skeleton(width, height, subSample);
#ifdef DEPTH
		skelD = new SkeletonDepth(width, height, subSample);
#endif
		cvNamedWindow("Skeleton Traker", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
		resizeWindow("Skeleton Traker", width, height);
		
		outputVideo = new VideoWriter("skeletonVideo.avi",
		//outputVideo = new VideoWriter("skeletonVideo.mp4",  
		       CV_FOURCC('D','I','V','X'),
		       //CV_FOURCC('M', 'P', '4', '2'),
		       //CV_FOURCC('L', 'A', 'G', 'S'),
		       //CV_FOURCC('F', 'M', 'P', '4'),
		       15, // FPS
		       cv::Size(width, height));
	}

//printf("sizeof(openni::RGB888Pixel)=%ld\n", sizeof(openni::RGB888Pixel) );

	frameCount++;

	// check if we need to draw depth frame to texture
#ifdef DEPTH
	if (srcFrame->isValid())
	{
		Mat binarized(cv::Size(width/subSample, height/subSample), CV_8UC1, cv::Scalar(0));
		short depthMat[width*height*sizeof(short)];
		bzero(depthMat, width*height*sizeof(short));
		memset(m_pTexMap, 0, width*height*sizePixel);

		skelD->prepareAnalisa(closest, furthest);
		//paint with color and get the binarized image.
		skelD->paintDepthCopy((openni::RGB888Pixel*)m_pTexMap, srcFrame, binarized, depthMat);
		
		skel->setDepthMat(depthMat);

		// Convert openni::VideoFrameRef (srcFrame) to cv::Mat (frame)
		Mat frame = Mat(Size(width, height), CV_8UC3);
		memcpy(frame.data, m_pTexMap, width*height*sizePixel);
#else
	if( srcFrame.data != NULL )
	{
		Mat binarized(cv::Size(width/subSample, height/subSample), CV_8UC1, cv::Scalar(0));
		Mat frame = framec;
		Mat frameB;

		cv::threshold(srcFrame, frameB, 70, 255, cv::THRESH_BINARY_INV);		
		cv::resize(frameB, binarized, binarized.size());

		skel->drawOverFrame2(&frameB, frame);
		// mode webcam RGB, discard the first 10 frames, because they can be too white.
		if (frameCount>10) {
#endif
		skel->detectBiggerRegion(binarized);

		Mat * skeleton = DrawAux::thinning(binarized);
		skel->analyse(skeleton);

		std::vector<Point3D> rightArm = skel->getSkeletonArm(skeleton, true);
		std::vector<Point3D> leftArm= skel->getSkeletonArm(skeleton, false);

		skel->locateMainBodyPoints(binarized);

		//skel->drawOverFrame(skeleton, frame);
		//skel->drawOverFrame(rightArm, frame);
		//skel->drawOverFrame(leftArm, frame);

		skel->drawMarkers(frame);
		
		notifyListeners(skel->getSkeletonPoints(), skel->getAfa(), closest, frame);

		if (skeleton)
			delete skeleton;
#ifndef DEPTH
		} // if (frameCount>10)
#endif
		//imshow("Skeleton Traker", *skeleton);
		imshow("Skeleton Traker", frame );
		//imshow("Skeleton Traker", binarized2 );
		//outputVideo->write(frame);
	}

#ifdef DEPTH
	if (srcFrame)
		delete srcFrame;
#endif
	if (closest)
		delete closest;
	if (furthest)
		delete furthest;
}


void SampleViewer::notifyListeners(SkeletonPoints * sp, int afa, Point3D *closest, Mat &frame) {
	//printf("CLOSE=%6d :: head.z=%6d  left/rightHand=%6d::%6d::ombros=%6d::%6d\n", closest->z, sp->head.z, sp->leftHand.z, sp->rightHand.z, sp->leftShoulder.z, sp->rightShoulder.z);
	std::vector<cv::Rect> * recs;
	int i;
	Scalar c;
	c = Scalar(255,0,0);
	for (std::vector<SkeletonListener*>::iterator it = listeners.begin(); it != listeners.end(); it++) {
		recs = (*it)->onEvent(sp, afa, closest);
		if (recs != NULL && recs->size() > 0) {
			for (int i = 0 ; i< recs->size() ; i++) {
				if (i==1) c = Scalar(0,0,255);
				if (i==2) c = Scalar(0,255,0);
				rectangle(frame, recs->at(i), c, 3, 8, 0 );
			}
				
		}
	}
}

void SampleViewer::registerListener(SkeletonListener * listener) {
	listeners.push_back(listener);
}



