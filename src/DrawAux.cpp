#include "DrawAux.h"
#include <opencv2/imgproc.hpp>

using namespace cv;

/**
 * Calculate de Euclidian distance 2D between 2 points.
 **/
float DrawAux::euclideanDist(cv::Point& p, cv::Point& q) {
	if (p.x==q.x && p.y==q.y)
		return 0;
	cv::Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}


/**
 * Calculate de Euclidian distance 3D between 2 points.
 **/
float DrawAux::euclideanDist3D(Point3D& p, Point3D& q) {
	if (p.x==q.x && p.y==q.y)
		return 0;
	return cv::sqrt( (p.x-q.x)*(p.x-q.x) + (p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.z-q.z) );
}





/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 *
 * Author:  Nash (nash [at] opencv-code [dot] com) 
 * Website: http://opencv-code.com
 * Source: https://raw.githubusercontent.com/bsdnoobz/zhang-suen-thinning/master/thinning.cpp
 */
/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * Parameters:
 * 		im    Binary image with range = [0,1]
 * 		iter  0=even, 1=odd
 */
void thinningIteration(cv::Mat& img, int iter)
{
    CV_Assert(img.channels() == 1);
    CV_Assert(img.depth() != sizeof(uchar));
    CV_Assert(img.rows > 3 && img.cols > 3);

    cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

    int nRows = img.rows;
    int nCols = img.cols;

    if (img.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int x, y;
    uchar *pAbove;
    uchar *pCurr;
    uchar *pBelow;
    uchar *nw, *no, *ne;    // north (pAbove)
    uchar *we, *me, *ea;
    uchar *sw, *so, *se;    // south (pBelow)

    uchar *pDst;

    // initialize row pointers
    pAbove = NULL;
    pCurr  = img.ptr<uchar>(0);
    pBelow = img.ptr<uchar>(1);

    for (y = 1; y < img.rows-1; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr  = pBelow;
        pBelow = img.ptr<uchar>(y+1);

        pDst = marker.ptr<uchar>(y);

        // initialize col pointers
        no = &(pAbove[0]);
        ne = &(pAbove[1]);
        me = &(pCurr[0]);
        ea = &(pCurr[1]);
        so = &(pBelow[0]);
        se = &(pBelow[1]);

        for (x = 1; x < img.cols-1; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ne = &(pAbove[x+1]);
            we = me;
            me = ea;
            ea = &(pCurr[x+1]);
            sw = so;
            so = se;
            se = &(pBelow[x+1]);

            int A  = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) + 
                     (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) + 
                     (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
                     (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            int B  = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                pDst[x] = 1;
        }
    }

    img &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,255]
 * 		dst  The destination image
 */
void thinning02_(const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();
    dst /= 255;         // convert to binary image

    cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Mat diff;

    do {
        thinningIteration(dst, 0);
        thinningIteration(dst, 1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    } 
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
}


cv::Mat * DrawAux::thinning02(cv::Mat &binarized) {
	Mat * skeleton = new Mat(cv::Size(binarized.cols, binarized.rows), CV_8UC1, cv::Scalar(0));
	
	thinning02_(binarized, *skeleton);
	
	return skeleton;
}




/**
 * Thinning/skeletonization operation over the binarized region of the body
 *
 * @param binarized - Mat with the binarized image
 * @return the thinning skeleton
 * @autor http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 */
cv::Mat * DrawAux::thinning01(cv::Mat &binarized) {
	int width = binarized.cols;
	int height = binarized.rows;
	Mat * skeleton = new Mat(cv::Size(width, height), CV_8UC1, cv::Scalar(0));
	Mat temp;
	Mat eroded;
	//Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
	Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(5,5));
	bool done=false;      
	do
	{
		cv::erode(binarized, eroded, element);
		cv::dilate(eroded, temp, element);
		cv::subtract(binarized, temp, temp);
		cv::bitwise_or(*skeleton, temp, *skeleton);
		eroded.copyTo(binarized);
		done = (cv::countNonZero(binarized) == 0);
	}
	while (!done);

	return skeleton;
}



// http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm#C.2B.2B
std::vector<cv::Point> * DrawAux::lineBresenham(cv::Point p1, cv::Point p2)
{
    std::vector<cv::Point> * points = new std::vector<cv::Point>();
    
    int delta_x(p2.x - p1.x);
    // if p1.x == p2.x, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    int delta_y(p2.y - p1.y);
    // if p1.y == p2.y, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;
 
    //plot(p1.x, p1.y);
    points->push_back(p1);
 
    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));
 
        while (p1.x != p2.x)
        {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= delta_x;
                p1.y += iy;
            }
            // else do nothing
 
            error += delta_y;
            p1.x += ix;
 
            //plot(p1.x, p1.y);
            points->push_back(p1);
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));
 
        while (p1.y != p2.y)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= delta_y;
                p1.x += ix;
            }
            // else do nothing
 
            error += delta_x;
            p1.y += iy;
 
            //plot(p1.x, p1.y);
            points->push_back(p1);
        }
    }
    
    return points;
}


