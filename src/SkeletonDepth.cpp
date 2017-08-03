#include "SkeletonDepth.h"
#include <stdio.h>
#include <opencv2/imgproc.hpp>

using namespace cv;

SkeletonDepth::SkeletonDepth(int width, int height, int subSample) {
	this->width = width;
	this->height = height;
	this->subSample = subSample;

	lineSize = width/subSample;

	diff_w = 0;
	diff_h = 0;
	diff = 180 + diff_w + diff_h;
	dist = 0;
	maxDiff = 240;
	max = 0;
}



void SkeletonDepth::paintDepthCopy(openni::RGB888Pixel*m_pTexMap, openni::VideoFrameRef * depthFrame, cv::Mat &binarized, short depthMat[]) {
	float factor[3] = {1, 1, 1};
	const float *f;

	const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame->getData();
	openni::RGB888Pixel* pTexRow = m_pTexMap + depthFrame->getCropOriginY() * width;
	int rowSize = depthFrame->getStrideInBytes() / sizeof(openni::DepthPixel);

	//printf("sizeof(openni::DepthPixel)=%ld\n", sizeof(openni::DepthPixel));
	//printf("sizeof(short)=%ld\n", sizeof(short));

	max = 0;
	if (pDepthRow && closest) 
		for (int y = 0; y < height; ++y)
		{
			const openni::DepthPixel* pDepth = pDepthRow;
			openni::RGB888Pixel* pTex = pTexRow + depthFrame->getCropOriginX();
			setDiffH(abs(closest->y-y)/5); // diferenca (height) do ponto atual para o ponto mais proximo
			for (int x = 0; x < width; ++x, ++pDepth, ++pTex)
			{
				if (*pDepth != 0)
				{
					setDiffW(abs(closest->x-x)/5); // diferenca (width) do ponto atual para o ponto mais proximo
					f = paintDepthCopyPixel(pDepth, x, y, binarized);
					depthMat[y*width + x] = *pDepth; 
					//printf("vals: %d\n", *pDepth);
					if (f)
						memcpy(factor, f, sizeof(float)*3);

					int nHistValue = 256*(1.0f - (*pDepth)/(float)(furthest->z-closest->z));
					//printf("::depths::%d::%d - %d::nHistValue = %d::%d\n", *pDepth, furthest->z, closest->z, nHistValue, old);
					pTex->r = nHistValue*factor[0];
					pTex->g = nHistValue*factor[1];
					pTex->b = nHistValue*factor[2];
					factor[0] = factor[1] = factor[2] = 1;
				}
			}
			pDepthRow += rowSize;
			pTexRow += width;
		}
	else
		printf("2::pDepthRow = NULL\n");

	if (max>0)
		maxDiff = max;
}





const float * SkeletonDepth::paintDepthCopyPixel(const openni::DepthPixel* pDepth, int x, int y, cv::Mat &binarized) {
	//diff = 180 + diff_w + diff_h;
	diff = 600 + diff_w + diff_h;

	if (*pDepth == closest->z)
	{
		rgb[0] = 0; // R
		rgb[1] = 1; // G
		rgb[2] = 0; // B

		return rgb;
	}
	else if (*pDepth >= closest->z && *pDepth <= closest->z+diff)
	{
		if (y%subSample==0 && x%subSample==0)
			binarized.data[(y/subSample)*lineSize+x/subSample]=255;

		if (diff>max) {
			max = diff;
		}

		dist = ((*pDepth) - closest->z)/maxDiff;
		if (dist<1) {
			rgb[2] = 1-dist; // R
			rgb[0] = dist;   // B
		}
		else {
			rgb[2] = 0; // R
			rgb[0] = 1; // B
		}
		rgb[1] = 0;      // G

		return rgb;
	}


	return 0;
}


void SkeletonDepth::prepareAnalisa(Point3D * closest, Point3D *furthest) {
	this->closest = closest;
	this->furthest = furthest;
}

void SkeletonDepth::setDiffH(int d) {
	diff_h = d;
}

void SkeletonDepth::setDiffW(int d) {
	diff_w = d;
}
