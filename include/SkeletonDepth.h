#ifndef SKELETON_DEPTH_H
#define SKELETON_DEPTH_H

#include <OpenNI.h>
#include <opencv2/highgui.hpp>
#include "Point3D.h"

class SkeletonDepth {
    private:
	int width, height;
	int subSample;
	float rgb[3];
	int diff_w;
	int diff_h;
	int diff;
	int lineSize;
	int max;
	float maxDiff;
	float dist;
	Point3D * closest;
	Point3D * furthest;

	const float * paintDepthCopyPixel(const openni::DepthPixel* pDepth, int x, int y, cv::Mat &binarized);
	void setDiffH(int d);
	void setDiffW(int d);

    public:
	SkeletonDepth(int, int, int);
	void paintDepthCopy(openni::RGB888Pixel*m_pTexMap, openni::VideoFrameRef *depthFrame, cv::Mat &binarized, short depthMat[]);
	void prepareAnalisa(Point3D * closest, Point3D * furthest);
};


#endif // SKELETON_DEPTH_H
