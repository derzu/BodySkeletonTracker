#ifndef SKELETON_H
#define SKELETON_H

#include <opencv2/highgui.hpp>
#include <Point3D.h>
#include <vector>

#include "SkeletonPoints.h"

/**
 * This class process a frame and find the main skeleton points, that will be stored at a SkeletonPoints.
 *
 * @author derzu
 **/
class Skeleton {
    private:
        int subSample;
	int width, height;

	int wC; // w clear
	int hC; // w clear
	cv::Point right, left, topCenter, topRight, topLeft, bottomCenter, bottomRight, bottomLeft;

	// Main interest points
	Point3D maxRight, maxLeft;
	Point3D maxTopCenter,    maxTopRight,    maxTopLeft;
	Point3D maxBottomCenter, maxBottomRight, maxBottomLeft;
	Point3D middleArmRight, middleArmLeft;
	Point3D middleArmRight45, middleArmLeft45;
	Point3D maxRightMaxBottom, maxLeftMaxBottom;
	Point3D middleStraightArmRight, middleStraightArmLeft;

	// Skeleton Points (hands, shoulders, elbow, head)
	// Pontos do esqueleto (maos, ombros, cotovelos, cabeca)
	SkeletonPoints * sp;

	// Depth Mat	
	short * depthMat;

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
	Point3D * getElbowHard(std::vector<Point3D> &armPoints, int ang);
	Point3D mediaPoint(cv::Mat * frame);
	void obtainZ(Point3D &point);
	void removeSmallsRegions(cv::Mat * frame);
	void locateMaximus(cv::Mat *frame);
	int  getMeanDepthValue(cv::Point& p);

    public:
	Skeleton(int width, int height, int subSample);
	virtual ~Skeleton();
	static float euclideanDist(cv::Point& p, cv::Point& q);
	static float euclideanDist3D(Point3D& p, Point3D& q);
	void locateMainBodyPoints(cv::Mat &frame);
	void drawMarkers(cv::Mat &frame);
	void setDepthMat(short depth[]);
	void prepare(short depth[], Point3D* closest);
	void notifyListeners();
	void drawOverFrame(cv::Mat * skelImg, cv::Mat &frame);
	void drawOverFrame(std::vector<Point3D> pontos, cv::Mat &frame);
	cv::Mat * thinning01(cv::Mat &binarized);
	cv::Mat * thinning02(cv::Mat &binarized);
	void detectBiggerRegion(cv::Mat &frame);
	std::vector<Point3D> getSkeletonArm(cv::Mat * skeleton, bool right);
	SkeletonPoints* getSkeletonPoints();
	int getAfa();
	void analyse(cv::Mat * skeleton);
};


#endif // SKELETON_H
