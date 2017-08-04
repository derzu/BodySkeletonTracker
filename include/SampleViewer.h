#ifndef SAMPLEVIEWER_H
#define SAMPLEVIEWER_H

#include "Skeleton.h"


#ifdef DEPTH

#include "SkeletonDepth.h"
#include <OpenNI.h>

#endif

#include "SkeletonListener.h"

class SampleViewer
{
public:
	SampleViewer(const char* strSampleName, const char* deviceUri);
	virtual ~SampleViewer();

	 int init();
	 int run();
	 void registerListener(SkeletonListener * listener);

protected:
	virtual void display();

	void finalize();

private:
	SampleViewer(const SampleViewer&);
	SampleViewer& operator=(SampleViewer&);
	void notifyListeners(SkeletonPoints * sp, int afa, Point3D *closest, cv::Mat &frame);

	static SampleViewer* ms_self;
	
	char			m_strSampleName[50];
	unsigned int		width;
	unsigned int		height;
	int subSample;
	long frameCount;

	unsigned char * m_pTexMap;
	
	cv::VideoWriter * outputVideo;

	// Detecta o esqueleto
	Skeleton * skel;

	// lista de listeners que receberao o esqueleto.	
	std::vector<SkeletonListener*> listeners;

#ifdef DEPTH
	// OPENNI2
	int initOpenNI(const char* deviceUri);
	Point3D* getClosestPoint(openni::VideoFrameRef *frame, Point3D *furthest);
	openni::VideoFrameRef * getNextFrame();
	openni::VideoStream depth;
	openni::Device device;

	SkeletonDepth * skelD;
#else
	// OPENCV
	cv::VideoCapture capture;
#endif
};


#endif // VIEWER_H
