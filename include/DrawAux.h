#include <opencv2/core.hpp>
#include "Point3D.h"

class DrawAux {

	public:
		static float euclideanDist(cv::Point& p, cv::Point& q);
		static float euclideanDist3D(Point3D& p, Point3D& q);
		static cv::Mat * thinning(cv::Mat &binarized);
		static std::vector<cv::Point> * lineBresenham(cv::Point p1, cv::Point p2);

};
