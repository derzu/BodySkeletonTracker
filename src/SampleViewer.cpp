#include "SampleViewer.h"

#include <math.h>
#include <string.h>

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

SampleViewer* SampleViewer::ms_self = NULL;

SampleViewer::SampleViewer(const char* strSampleName, const char* deviceUri)
{
#ifdef DEPTH
	m_pClosestPointListener = NULL;
	m_pClosestPoint = new closest_point::ClosestPoint(deviceUri);
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
}

void SampleViewer::finalize()
{
#ifdef DEPTH
	if (m_pClosestPoint)
	{
		m_pClosestPoint->resetListener();
		delete m_pClosestPoint;
		m_pClosestPoint = NULL;
	}
	if (m_pClosestPointListener)
	{
		delete m_pClosestPointListener;
		m_pClosestPointListener = NULL;
	}
#endif
}

int SampleViewer::init()
{
	m_pTexMap = NULL;

#ifdef DEPTH
	if (!m_pClosestPoint->isValid())
	{
		return openni::STATUS_ERROR;
	}

	m_pClosestPointListener = new MyMwListener;
	m_pClosestPoint->setListener(*m_pClosestPointListener);

	return openni::STATUS_OK;
#else
	return 0;
#endif

}
int SampleViewer::run()	//Does not return
{
#ifdef DEPTH
printf("Compilado com Depth\n");
#else
printf("Compilado SEM Depth\n");
#endif

	while (1) {
		display();
		char c = (char)waitKey(10);
		//char c = (char)waitKey(100);
		//char c = (char)waitKey(200);
		//char c = (char)waitKey(500);
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
#ifdef DEPTH
	sizePixel = sizeof(openni::RGB888Pixel);
	if (!m_pClosestPointListener->isAvailable())
	{
		return;
	}
	
	// depthFrame
	openni::VideoFrameRef srcFrame = m_pClosestPointListener->getFrame();
	const closest_point::IntPoint3D& closest = m_pClosestPointListener->getClosestPoint();
	m_pClosestPointListener->setUnavailable();
#else
	Mat srcFrame;
	srcFrame.data = NULL;
	if( capture.isOpened() ) {
		Mat framec;
		capture >> framec;
		if(! framec.empty() ) {
			srcFrame = cv::Mat(framec.size(), CV_8UC1);
			cvtColor(framec, srcFrame, CV_RGB2GRAY);
			//srcFrame = framec.clone();
		}
		else
			printf("frame nulo\n");
	}
#endif




	// So entra nesses if na primeira vez
#ifdef DEPTH
	if (m_pTexMap == NULL)
	{
		// Texture map init
		m_nTexMapX = srcFrame.getWidth();
		m_nTexMapY = srcFrame.getHeight();
#else
	if (m_pTexMap == NULL && srcFrame.data!=NULL)
	{
		// TODO pegar da webcam opencv
		m_nTexMapX = srcFrame.cols;
		m_nTexMapY = srcFrame.rows;
#endif
//printf("w x h = %d x %d\n", m_nTexMapX, m_nTexMapY);
		m_pTexMap = new unsigned char[m_nTexMapX * m_nTexMapY * sizePixel];

		skel = new Skeleton(m_nTexMapX, m_nTexMapY, subSample);
#ifdef DEPTH
		skelD = new SkeletonDepth(m_nTexMapX, m_nTexMapY, subSample);
#endif
		cvNamedWindow("Skeleton Traker", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
		//cvSetWindowProperty("Skeleton Traker", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		//resizeWindow("Skeleton Traker", m_nTexMapX*2, m_nTexMapY*2);
		resizeWindow("Skeleton Traker", m_nTexMapX, m_nTexMapY);
	}

//printf("sizeof(openni::RGB888Pixel)=%ld\n", sizeof(openni::RGB888Pixel) );

	frameCount++;

	// check if we need to draw depth frame to texture
#ifdef DEPTH
	if (srcFrame.isValid())
	{
		Mat binarized(cv::Size(m_nTexMapX/subSample, m_nTexMapY/subSample), CV_8UC1, cv::Scalar(0));
		short depthMat[m_nTexMapX*m_nTexMapY*sizeof(short)];
		bzero(depthMat, m_nTexMapX*m_nTexMapY*sizeof(short));

		memset(m_pTexMap, 0, m_nTexMapX*m_nTexMapY*sizeof(openni::RGB888Pixel));

		skelD->prepareAnalisa(closest);
		//colore e obtem a imagem binarizada
		skelD->paintDepthCopy((openni::RGB888Pixel*)m_pTexMap, srcFrame, binarized, depthMat);

		// Converte o openni::VideoFrameRef (srcFrame) para um cv::Mat (frame)
		Mat frame = Mat(Size(m_nTexMapX, m_nTexMapY), CV_8UC3);
		memcpy(frame.data, m_pTexMap, m_nTexMapX*m_nTexMapY*sizePixel);
#else
	if( srcFrame.data != NULL )
	{
		Mat binarizedFirst(cv::Size(m_nTexMapX/subSample, m_nTexMapY/subSample), CV_8UC1, cv::Scalar(0));
		Mat binarized     (cv::Size(m_nTexMapX/subSample, m_nTexMapY/subSample), CV_8UC1, cv::Scalar(0));

		cv::resize(srcFrame, binarizedFirst, binarizedFirst.size());

		cv::threshold(binarizedFirst, binarized, 50, 255, cv::THRESH_BINARY_INV);
		

		Mat frame = srcFrame;
		//Mat frame = binarizedFirst;

		// mode webcam RGB, discard the first 10 frames, because they can be too white.
		if (frameCount>10) {
#endif

		Mat binarizedCp = binarized.clone();
		skel->detectBiggerRegion(binarized);

		//Mat binarized2    (cv::Size(m_nTexMapX/subSample, m_nTexMapY/subSample), CV_8UC1, cv::Scalar(0));
		//Mat binarizedFirst(cv::Size(m_nTexMapX/subSample, m_nTexMapY/subSample), CV_8UC1, cv::Scalar(0));
		//cv::resize(frame, binarizedFirst, binarizedFirst.size());
		//Canny(binarizedFirst, binarized2, 50, 200, 3);
		//Canny(binarized, binarized2, 50, 200, 3);


		Mat * skeleton = skel->thinning02(binarized);
		skel->analyse(skeleton);

		std::vector<cv::Point> bdireito = skel->getSkeletonArm(skeleton, true);
		std::vector<cv::Point> besquerdo= skel->getSkeletonArm(skeleton, false);

		skel->locateMainBodyPoints(binarizedCp);

		//skel->drawOverFrame(skeleton, frame);
		//skel->drawOverFrame(bdireito, frame);
		//skel->drawOverFrame(besquerdo, frame);
		//skel->drawOverFrame(&binarizedCp, frame);

		skel->drawMarkers(frame);
		skel->prepare(depthMat, (closest_point::IntPoint3D&) closest);
		
		skel->notifyListeners();

		if (skeleton)
			delete skeleton;
#ifndef DEPTH
		}
#endif
		//imshow("Skeleton Traker", *skeleton);
		imshow("Skeleton Traker", frame );
		//imshow("Skeleton Traker", binarized2 );
	}

}



