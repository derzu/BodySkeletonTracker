#ifndef VIEWER_H
#define VIEWER_H

#include "Skeleton.h"


#ifdef DEPTH

#include "SkeletonDepth.h"
#include "MyMwListener.h"

#endif

class SampleViewer
{
public:
	SampleViewer(const char* strSampleName, const char* deviceUri);
	virtual ~SampleViewer();

	virtual int init(int argc, char **argv);
	virtual int run();	//Does not return

protected:
	virtual void display();

	void finalize();

private:
	SampleViewer(const SampleViewer&);
	SampleViewer& operator=(SampleViewer&);

	static SampleViewer* ms_self;
	
	char			m_strSampleName[50];
	unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;
	int subSample;
	long frameCount;

	unsigned char * m_pTexMap;

	// Detecta o esqueleto
	Skeleton * skel;

#ifdef DEPTH
	// OPENNI2
	SkeletonDepth * skelD;

	closest_point::ClosestPoint* m_pClosestPoint;
	MyMwListener* m_pClosestPointListener;
#else
	// OPENCV
	cv::VideoCapture capture;
#endif
};


#endif // VIEWER_H
