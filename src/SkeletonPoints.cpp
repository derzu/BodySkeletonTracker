#include "SkeletonPoints.h"

SkeletonPoints::SkeletonPoints(cv::Point rightHand, cv::Point rightElbow, cv::Point rightShoulder, cv::Point leftHand, cv::Point leftElbow, cv::Point leftShoulder, cv::Point head) {
	this->rightHand = rightHand;
	this->rightElbow = rightElbow;
	this->rightShoulder = rightShoulder;

	this->leftHand = leftHand;
	this->leftElbow = leftElbow;
	this->leftShoulder = leftShoulder;

	this->head = head;

	init();
}

SkeletonPoints::SkeletonPoints() {
	cv::Point zero = cv::Point(0,0);
	this->rightHand = zero;
	this->rightElbow = zero;
	this->rightShoulder = zero;

	this->leftHand = zero;
	this->leftElbow = zero;
	this->leftShoulder = zero;

	this->head = zero;

	init();
}

void SkeletonPoints::init() {
	bodyPoints[HEAD]           = &head;
	bodyPoints[RIGHT_HAND]     = &rightHand;
	bodyPoints[RIGHT_ELBOW]    = &rightElbow;
	bodyPoints[RIGHT_SHOULDER] = &rightShoulder;
	bodyPoints[LEFT_HAND]      = &leftHand;
	bodyPoints[LEFT_ELBOW]     = &leftElbow;
	bodyPoints[LEFT_SHOULDER]  = &leftShoulder;

	bzero(pointsV,  sizeof(cv::Point)*MAX_BODY_POINTS*BUF_SIZE);
	bzero(vHead,    MAX_BODY_POINTS);
}


int SkeletonPoints::addToVector(int type, cv::Point * el) {
	if (type>MAX_BODY_POINTS || type<0)
		return -1; // error

	pointsV[type][(vHead[type]++) % BUF_SIZE] = *el;

	return 1;// addition sucessfuly
}


// TODO testar com a mediana
cv::Point SkeletonPoints::getMediaVector(int type) {
	cv::Point *vector = pointsV[type];

	cv::Point m = cv::Point(0,0);
	int q=0;
	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (vector[i].x!=0) {
			m.x += vector[i].x;
			m.y += vector[i].y;
			q++;
		}
	}
	if (q>0) {
		m.x /= q;
		m.y /= q;
	}

	return m;
}


void SkeletonPoints::computePoint(int type) {
	addToVector(type, bodyPoints[type]);
	*(bodyPoints[type]) = getMediaVector(type);
}

int SkeletonPoints::getCenterY() {
	//return bodyPoints[HEAD]->y + 150;
	return (bodyPoints[HEAD]->y - (bodyPoints[RIGHT_SHOULDER]->y+bodyPoints[LEFT_SHOULDER]->y)/2)*2;
}



