#ifndef SKELETON_POINTS_H
#define SKELETON_POINTS_H

#include <opencv2/highgui.hpp>

#define BUF_SIZE 5
#define MAX_BODY_POINTS 7

class SkeletonPoints {

	public:
		SkeletonPoints();
		SkeletonPoints(cv::Point rightHand, cv::Point rightElbow, cv::Point rightShoulder, cv::Point leftHand, cv::Point leftElbow, cv::Point leftShoulder, cv::Point head);
		void computePoint(int type);
		int getCenterY();

		// Main body points
		cv::Point rightHand, rightElbow, rightShoulder;
		cv::Point leftHand,  leftElbow,  leftShoulder;
		cv::Point head;

		// pointer to the main body points
		cv::Point * bodyPoints[MAX_BODY_POINTS];

		// Main body points CONSTANTS values
		static const int HEAD          = 0;
		static const int RIGHT_HAND    = 1;
		static const int RIGHT_ELBOW   = 2;
		static const int RIGHT_SHOULDER= 3;
    		static const int LEFT_HAND     = 4;
    		static const int LEFT_ELBOW    = 5;
    		static const int LEFT_SHOULDER = 6;


		// Main body points vectors (history)
		cv::Point pointsV[MAX_BODY_POINTS][BUF_SIZE];

		// Main body points vector heads (last elemment added)
		unsigned char vHead[MAX_BODY_POINTS];
	private:
		void init();
		int addToVector(int type, cv::Point *el);
		cv::Point getMediaVector(int type);



};

#endif
