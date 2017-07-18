#ifndef SAMPLEVIEWER_H
#define SAMPLEVIEWER_H

#include "Skeleton.h"


#ifdef DEPTH

#include "SkeletonDepth.h"
#include "MyMwListener.h"

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
	void notifyListeners(SkeletonPoints * sp, int afa);

	static SampleViewer* ms_self;
	
	char			m_strSampleName[50];
	unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;
	int subSample;
	long frameCount;

	unsigned char * m_pTexMap;

	// Detecta o esqueleto
	Skeleton * skel;

	// lista de listeners que receberao o esqueleto.	
	std::vector<SkeletonListener*> listeners;

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
