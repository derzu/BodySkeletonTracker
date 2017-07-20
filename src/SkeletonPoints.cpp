#include "SkeletonPoints.h"

/**
 * This class has all the Skeleton Body info with the main 3D Points.
 * 
 * @author derzu
 * 
 **/
 
 

/**
 * The constructor
 **/
SkeletonPoints::SkeletonPoints() {
	Point3D zero = Point3D(0,0,0);
	this->rightHand = zero;
	this->rightElbow = zero;
	this->rightShoulder = zero;

	this->leftHand = zero;
	this->leftElbow = zero;
	this->leftShoulder = zero;

	this->head = zero;

	init();
}

SkeletonPoints::~SkeletonPoints() {
	
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

	bzero(pointsV,  sizeof(Point3D)*MAX_BODY_POINTS*BUF_SIZE);
	bzero(vHead,    MAX_BODY_POINTS);
}


/**
 * Add a point to a circular vector.
 *
 * @param type the type of the point. The types are declared at the .h file.
 *
 * @return 1 if sucess, -1 if error.
 **/
int SkeletonPoints::addToVector(int type, Point3D * el) {
	if (type>MAX_BODY_POINTS || type<0)
		return -1; // error

	pointsV[type][(vHead[type]++) % BUF_SIZE] = *el;

	return 1;// addition sucessfuly
}


/**
 * Compute the mean point of a vector of 3D Points.
 *
 * @param type the type of the point. The types are declared at the .h file.
 *
 * @return the mean 3D Point.
 **/
Point3D SkeletonPoints::getMeanVector(int type) {
	Point3D *vector = pointsV[type];

	Point3D m = Point3D(0,0);
	int q=0;
	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (vector[i].x!=0) {
			m.x += vector[i].x;
			m.y += vector[i].y;
			m.z += vector[i].z;
			q++;
		}
	}
	if (q>0) {
		m.x /= q;
		m.y /= q;
		m.y /= q;
	}

	return m;
}


/**
 * Add the point of the specified type to a history vector.
 * Then change the value of the point to the Mean/Median of the vector of the last BUF_SIZE points. 
 *
 * @param type the type of the point. The types are declared at the .h file.
 **/
void SkeletonPoints::computePoint(int type) {
	if (bodyPoints[type]->x!=0)
		addToVector(type, bodyPoints[type]);
	//*(bodyPoints[type]) = getMeanVector(type);
	*(bodyPoints[type]) = getMedianaVector(type);
}


/**
 * Compute the median point of a vector of 3D Points.
 *
 * @param type the type of the point. The types are declared at the .h file.
 *
 * @return the median 3D Point.
 **/
Point3D SkeletonPoints::getMedianaVector(int type) {
	Point3D *vector = pointsV[type];
	int q1=0, q2=0, q3=0;
	Point3D m = Point3D(0,0);

	// serpara o vetor de pontos em 3 vetores
	int v1[BUF_SIZE], v2[BUF_SIZE], v3[BUF_SIZE];
	for (int i=0 ; i<BUF_SIZE ; i++) {
		v1[i] = vector[i].x;
		v2[i] = vector[i].y;
		v3[i] = vector[i].z;
	}

	quick_sort(v1, 0, BUF_SIZE);
	quick_sort(v2, 0, BUF_SIZE);
	quick_sort(v3, 0, BUF_SIZE);

	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (v1[i]!=0)
			q1++;
		if (v2[i]!=0)
			q2++;
		if (v3[i]!=0)
			q3++;
	}
	if (q1>0)
		m.x = v1[q1/2+BUF_SIZE-q1];
	if (q2>0)
		m.y = v2[q2/2+BUF_SIZE-q2];
	if (q3>0)
		m.z = v3[q3/2+BUF_SIZE-q3];

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


