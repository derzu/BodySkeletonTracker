#ifndef SKELETON_H
#define SKELETON_H

#include <opencv2/highgui.hpp>
#include <vector>

#define BUF_SIZE 3

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

	// Main body points
	cv::Point rightHand, rightElbow, rightShoulder;
	cv::Point leftHand,  leftElbow,  leftShoulder;
	cv::Point head;

	// Main body points vectors
	cv::Point rightHandV[BUF_SIZE], rightElbowV[BUF_SIZE], rightShoulderV[BUF_SIZE];
	cv::Point leftHandV[BUF_SIZE],  leftElbowV[BUF_SIZE],  leftShoulderV[BUF_SIZE];
	cv::Point headV[BUF_SIZE];

	// Main body points vector heads
	unsigned char rightHandH, rightElbowH, rightShoulderH;
	unsigned char leftHandH,  leftElbowH,  leftShoulderH;
	unsigned char headH;

	// afastamento
	int afa;

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
	cv::Point calculaMedia(cv::Point vector[]);
	int calculaMedia2(int vector[]);
	cv::Point calculaMedia2(cv::Point vector[]);
    public:
	Skeleton(int width, int height, int subSample);
	void locateMaximus(cv::Mat *frame);
	void locateMainBodyPoints(cv::Mat &frame);
	void drawMarkers(cv::Mat &frame);
	void removeSmallsRegions(cv::Mat * frame);
	void drawOverFrame(cv::Mat * skelImg, cv::Mat &frame);
	void drawOverFrame(std::vector<cv::Point> pontos, cv::Mat &frame);
	cv::Mat * thinning(cv::Mat &binarized);
	void detectBiggerRegion(cv::Mat &frame);
	std::vector<cv::Point> getSkeletonArm(cv::Mat * skeleton, bool right);

};


#endif // SKELETON_H
