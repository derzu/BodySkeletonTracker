#ifndef SKELETON_POINTS_H
#define SKELETON_POINTS_H

#include <Point3D.h>

#define BUF_SIZE 3
#define MAX_BODY_POINTS 8

/**
 * This class has all the Skeleton Body info with the main 3D Points.
 * 
 * @author derzu
 * 
 **/
class SkeletonPoints {

	public:
		SkeletonPoints();
		virtual ~SkeletonPoints();
		void computePoint(int type);
		static void quick_sort(int *a, int left, int right);

		// Main body points
		Point3D rightHand, rightElbow, rightShoulder;
		Point3D leftHand,  leftElbow,  leftShoulder;
		Point3D head;
		Point3D center;

		// pointer to the main body points
		Point3D * bodyPoints[MAX_BODY_POINTS];

		// Main body points CONSTANTS TYPES values
		static const int HEAD          = 0;
		static const int RIGHT_HAND    = 1;
		static const int RIGHT_ELBOW   = 2;
		static const int RIGHT_SHOULDER= 3;
    		static const int LEFT_HAND     = 4;
    		static const int LEFT_ELBOW    = 5;
    		static const int LEFT_SHOULDER = 6;
		static const int CENTER        = 7;


		// Main body points vectors (history)
		Point3D pointsV[MAX_BODY_POINTS][BUF_SIZE];

		// Main body points vector heads (last elemment added)
		unsigned char vHead[MAX_BODY_POINTS];
	private:
		void init();
		int addToVector(int type, Point3D *el);
		Point3D getMeanVector(int type);
		Point3D getMedianaVector(int type);



};

#endif
