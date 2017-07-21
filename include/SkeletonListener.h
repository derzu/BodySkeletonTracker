#ifndef SKELETON_LISTENER_H
#define SKELETON_LISTENER_H

#include <SkeletonPoints.h>

class SkeletonListener {

	public:
		SkeletonListener();
		virtual ~SkeletonListener();
		virtual void onEvent(SkeletonPoints * sp, int afa, Point3D * closest)=0;
};


#endif
