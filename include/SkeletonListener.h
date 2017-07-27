#ifndef SKELETON_LISTENER_H
#define SKELETON_LISTENER_H

#include <SkeletonPoints.h>
#include <vector>
#include <opencv2/highgui.hpp>

class SkeletonListener {

	public:
		SkeletonListener();
		virtual ~SkeletonListener();
		virtual std::vector<cv::Rect> *onEvent(SkeletonPoints * sp, int afa, Point3D * closest)=0;
};


#endif
