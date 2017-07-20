#include "Point3D.h"

Point3D::Point3D(int x, int y, int z) : cv::Point(x,y) {
	this->z = z;
}

Point3D::Point3D(int x, int y) : cv::Point(x,y) {
	this->z = 0;
}

Point3D::Point3D() : cv::Point(0,0) {
	this->z = 0;
}

Point3D::~Point3D() {
}
