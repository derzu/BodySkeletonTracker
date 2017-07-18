#ifndef SKELETON_H
#define SKELETON_H

#include <opencv2/highgui.hpp>
#include <vector>

#include "SkeletonPoints.h"
#include "MWClosestPoint.h"

class Skeleton {
    private:
        int subSample;
	int width, height;

	int wC; // w clear
	int hC; // w clear
	cv::Point right, left, topCenter, topRight, topLeft, bottomCenter, bottomRight, bottomLeft;

	// Main interest points
	cv::Point maxRight, maxLeft;
	cv::Point maxTopCenter,    maxTopRight,    maxTopLeft;
	cv::Point maxBottomCenter, maxBottomRight, maxBottomLeft;
	cv::Point middleArmRight, middleArmLeft;
	cv::Point middleArmRight45, middleArmLeft45;
	cv::Point maxRightMaxBottom, maxLeftMaxBottom;
	cv::Point middleStraightArmRight, middleStraightArmLeft;

	// Skeleton Points (hands, shoulders, elbow, head)
	// Pontos do esqueleto (maos, ombros, cotovelos, cabeca)
	SkeletonPoints * sp;

	// afastamento
	int afa;
	float afa28;
	int shift;

	bool showSkeleton;

    protected:
	void initialize();
	void zeraMaximus();
	void setMaximus();
	void locateShoulders(cv::Mat &frame);
	void getSizeRegion(unsigned char * frame, int x, int y, int *quant);
	void clearRegion(unsigned char * frame, int x, int y);
	cv::Point * getElbowHard(std::vector<cv::Point> &armPoints, int ang);
	cv::Point mediaPoint(cv::Mat * frame);
	void removeSmallsRegions(cv::Mat * frame);
	void locateMaximus(cv::Mat *frame);

    public:
	Skeleton(int width, int height, int subSample);
	virtual ~Skeleton();
	static float euclideanDist(cv::Point& p, cv::Point& q);
	void locateMainBodyPoints(cv::Mat &frame);
	void drawMarkers(cv::Mat &frame);
	void prepare(short depth[], closest_point::IntPoint3D& closest);
	void notifyListeners();
	void drawOverFrame(cv::Mat * skelImg, cv::Mat &frame);
	void drawOverFrame(std::vector<cv::Point> pontos, cv::Mat &frame);
	cv::Mat * thinning01(cv::Mat &binarized);
	cv::Mat * thinning02(cv::Mat &binarized);
	void detectBiggerRegion(cv::Mat &frame);
	std::vector<cv::Point> getSkeletonArm(cv::Mat * skeleton, bool right);
	SkeletonPoints* getSkeletonPoints();
	int getAfa();
	void analyse(cv::Mat * skeleton);
};


#endif // SKELETON_H
