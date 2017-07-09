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
	bodyPoints[CENTER]         = &center;

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
	if (bodyPoints[type]->x!=0)
		addToVector(type, bodyPoints[type]);
	//*(bodyPoints[type]) = getMediaVector(type);
	*(bodyPoints[type]) = getMedianaVector(type);
}

/**
 * Mediana
 **/
cv::Point SkeletonPoints::getMedianaVector(int type) {
	cv::Point *vector = pointsV[type];
	int q1=0, q2=0;
	cv::Point m = cv::Point(0,0);

	// serpara o vetor de pontos em 2 vetores
	int v1[BUF_SIZE], v2[BUF_SIZE];
	for (int i=0 ; i<BUF_SIZE ; i++) {
		v1[i] = vector[i].x;
		v2[i] = vector[i].y;
	}

	quick_sort(v1, 0, BUF_SIZE);
	quick_sort(v2, 0, BUF_SIZE);

	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (v1[i]!=0)
			q1++;
		if (v2[i]!=0)
			q2++;
	}
	if (q1>0)
		m.x = v1[q1/2+BUF_SIZE-q1];
	if (q2>0)
		m.y = v2[q2/2+BUF_SIZE-q2];

	return m;
}


/**
 *  Quick sort function
 *
 **/
void SkeletonPoints::quick_sort(int *a, int left, int right) {
    int i, j, x, y;
     
    i = left;
    j = right;
    x = a[(left + right) / 2];
     
    while(i <= j) {
        while(a[i] < x && i < right) {
            i++;
        }
        while(a[j] > x && j > left) {
            j--;
        }
        if(i <= j) {
            y = a[i];
            a[i] = a[j];
            a[j] = y;
            i++;
            j--;
        }
    }
     
    if(j > left) {
        quick_sort(a, left, j);
    }
    if(i < right) {
        quick_sort(a, i, right);
    }
}


