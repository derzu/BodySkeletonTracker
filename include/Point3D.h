#ifndef POINT3D_H
#define POINT3D_H

#include <opencv2/highgui.hpp>

/**
 * This class is a 3D Point based on the 2D point of the OpenCV.
 * 
 * @author derzu
 **/
class Point3D : public cv::Point {
	public:
		Point3D(int x, int y, int z);
		Point3D(int x, int y);
		Point3D();
		virtual ~Point3D();
		
		int z;
};


#endif
