#ifndef SKELETON_H
#define SKELETON_H

#include <opencv2/highgui.hpp>
#include <vector>

#include "Tiago.h"
#include "SkeletonPoints.h"

#define BUF_SIZE 5

class Skeleton {
    private:
        int subSample;
	int width, height;

	int wC; // w clear
	int hC; // w clear
	int centerW, centerH;
	int centerWV[BUF_SIZE], centerHV[BUF_SIZE];
	int centerWHead, centerHHead;
	cv::Point right, left, topCenter, topRight, topLeft, bottomCenter, bottomRight, bottomLeft;

	// Main interest points
	cv::Point maxRight, maxLeft;
	cv::Point maxTopCenter,    maxTopRight,    maxTopLeft;
	cv::Point maxBottomCenter, maxBottomRight, maxBottomLeft;
	cv::Point middleArmRight, middleArmLeft;

	// Skeleton Points (hands, shoulders, elbow, head)
	// Pontos do esqueleto (maos, ombros, cotovelos, cabeca)
	SkeletonPoints * sp;

	// afastamento
	int afa;
	float afa28;
	int shift;

	Tiago * tiago;

    protected:
	void initialize();
	void zeraMaximus();
	void setMaximus();
	void locateShoulders(cv::Mat &frame);
	//float euclideanDist(cv::Point& p, cv::Point& q);
	void getSizeRegion(unsigned char * frame, int x, int y, int *quant);
	void clearRegion(unsigned char * frame, int x, int y);
	cv::Point * getElbowHard(std::vector<cv::Point> &armPoints);
	cv::Point mediaPoint(cv::Mat * frame);
	int calculaMedia(int vector[]);
	int calculaMedia2(int vector[]);
	cv::Point calculaMedia2(cv::Point vector[]);
	void removeSmallsRegions(cv::Mat * frame);
	void locateMaximus(cv::Mat *frame);

    public:
	Skeleton(int width, int height, int subSample);
	virtual ~Skeleton();
	void locateMainBodyPoints(cv::Mat &frame);
	void drawMarkers(cv::Mat &frame);
	void drawOverFrame(cv::Mat * skelImg, cv::Mat &frame);
	void drawOverFrame(std::vector<cv::Point> pontos, cv::Mat &frame);
	cv::Mat * thinning(cv::Mat &binarized);
	void detectBiggerRegion(cv::Mat &frame);
	std::vector<cv::Point> getSkeletonArm(cv::Mat * skeleton, bool right);
	//void detectTiagoCommands(SkeletonPoints * s);
	SkeletonPoints* getSkeletonPoints();
	void analyse(cv::Mat * skeleton);
};


#endif // SKELETON_H
