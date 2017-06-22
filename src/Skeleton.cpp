#include "Skeleton.h"
#include <stdio.h>
#include <GL/freeglut.h>
#include <opencv2/imgproc.hpp>

using namespace cv;

Skeleton::Skeleton(int width, int height, int subSample) {
	this->width = width;
	this->height = height;
	this->subSample = subSample;

	initialize();
}

void Skeleton::initialize() {
	afa = 70/subSample;

	rightShoulder = Point(10, 10);
	leftShoulder = Point(10, 10);
}

float euclideanDist(Point& p, Point& q) {
    Point diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}


void Skeleton::locateMaximus(cv::Mat * frame) {
	int width = frame->cols;
	int height = frame->rows;

	zeraMaximus();

	int centerWs = centerW/subSample;

//printf("centerWs=%d\n", centerWs);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (frame->data[y*width+x] == 255)
			{
				if (x>=right.x) {
					right.x = x;
					right.y = y;
				}
				if (x<left.x) {
					left.x = x;
					left.y = y;
				}
				if (x>=centerWs-afa && x<=centerWs+afa) {
					if (y<topCenter.y) {
						topCenter.x = x;
						topCenter.y = y;
					}
					if (y>bottomCenter.y) {
						bottomCenter.x = x;
						bottomCenter.y = y;
					}
				}
				if (x>centerWs+afa*1.2) {
					if (y<=topRight.y) {
						topRight.x = x;
						topRight.y = y;
					}
				}
				if (x>centerWs+afa*1.3) {
					if (y>=bottomRight.y) {
						bottomRight.x = x;
						bottomRight.y = y;
					}
				}
				if (x<centerWs-afa*1.2) {
					if (y<topLeft.y) {
						topLeft.x = x;
						topLeft.y = y;
					}
				}
				if (x<centerWs-afa*1.3) {
					if (y>bottomLeft.y) {
						bottomLeft.x = x;
						bottomLeft.y = y;
					}
				}
			}
			else if (frame->data[y*width+x] == 0) {
				//printf("frameZ: %d\n", frame->data[y*width+x]);
			}
			else 
				;//printf("frame: %d\n", frame->data[y*width+x]);
		}
	}

	setMaximus();
}



void Skeleton::locateShoulders(cv::Mat * frame) {
	int width = frame->cols;
	int height = frame->rows;

	int centerWs = centerW/subSample;
	int aff = afa-2;

	char nAchou1 = 1;
	char nAchou2 = 1;

	for (int y = maxTopCenter.y/subSample+40; y < height; y++)
	//for (int y = 0; y < height; y++)
	{
//printf("Achei y=%d \n", y);
		//if (centerWs+aff<width && centerWs-aff>0) 
		if (nAchou1 && centerWs+aff<width && (frame->data[y*width+centerWs+aff] == 255))
		{
			nAchou1 = 0;
			rightShoulder.x = (centerWs+aff)*subSample;
			rightShoulder.y = (y+10)*subSample;

			if (!nAchou2) break;
		}
		if (nAchou2 && centerWs-aff>0 && (frame->data[y*width+centerWs-aff] == 255))
		{
			nAchou2 = 0;
			leftShoulder.x = (centerWs-aff)*subSample;
			leftShoulder.y = (y+10)*subSample;

			if (!nAchou1) break;
		}
	}
}



void Skeleton::zeraMaximus() {
	right.x  = right.y = 0;
	left.x   = width; left.y = 0;
	topCenter.x    = 0; topCenter.y = height;
	topRight.x    = 0; topRight.y = height;
	topLeft.x    = 0; topLeft.y = height;
	bottomCenter.x = bottomCenter.y = 0;
	bottomRight.x = bottomRight.y = 0;
	bottomLeft.x = bottomLeft.y = 0;
}

void Skeleton::setMaximus() {
	if (right.y>0) {
		maxRight.x = right.x*subSample;
		maxRight.y = right.y*subSample;
	}
	if (left.y>0) {
		maxLeft.x = left.x*subSample;
		maxLeft.y = left.y*subSample;
	}
	if (topCenter.x>0)
	{
		maxTopCenter.x = topCenter.x*subSample;
		maxTopCenter.y = topCenter.y*subSample;
	}
	if (topRight.x>0)
	{
		maxTopRight.x = topRight.x*subSample;
		maxTopRight.y = topRight.y*subSample;
	}
	if (topLeft.x>0)
	{
		maxTopLeft.x = topLeft.x*subSample;
		maxTopLeft.y = topLeft.y*subSample;
	}
	if (bottomCenter.x>0) {
		maxBottomCenter.x = bottomCenter.x*subSample;
		maxBottomCenter.y = bottomCenter.y*subSample;
	}
	if (bottomRight.x>0) {
		maxBottomRight.x = bottomRight.x*subSample;
		maxBottomRight.y = bottomRight.y*subSample;
	}
	if (bottomLeft.x>0) {
		maxBottomLeft.x = bottomLeft.x*subSample;
		maxBottomLeft.y = bottomLeft.y*subSample;
	}

	//centerW = maxLeft.x + (maxRight.x-maxLeft.x)/2;
	/*centerH = (maxTopCenter.y + (maxBottomCenter.y-maxTopCenter.y)/2 +//);
		   maxTopRight.y  + (maxBottomRight.y-maxTopRight.y)/2 + 
		   maxTopLeft.y   + (maxBottomLeft.y-maxTopLeft.y)/2 ) / 3;
*/
}


void Skeleton::locateMainPoints(cv::Mat &frame) {
	float afa28 = afa*subSample*2.8;
	int shift = 50;

	// Cabeca. Sempre e o ponto mais alto.
	head = maxTopCenter;



	// Left Hand
	// Analisando apenas o lado esquerdo do corpo. Sera mao se:
	if (centerW-maxLeft.x > afa28) { // Certeza absoluta. Ponto mais a esquerda esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
		// O ponto mais a esquerda
		leftHand = maxLeft;
	}
	// Se o ponto mais alto tiver a esquerda do ponto mais baixo. Ou:
	// Se o ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	else if ((maxTopLeft.x < maxBottomLeft.x) || (maxBottomLeft.y < centerH+shift && euclideanDist(maxTopLeft, leftShoulder)>50) ) {
		// O ponto mais alto
		leftHand = maxTopLeft;
	} else {
		// O ponto mais baixo
		leftHand = maxBottomLeft;
	}

	// Right Hand
	// Analisando apenas o lado direito do corpo. Sera mao se:
	if (maxRight.x-centerW > afa28) {// Certeza absoluta. Ponto mais a direita esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
		// O ponto mais a direita
		rightHand = maxRight;
	}
	// Se o ponto mais alto tiver a direita do ponto mais baixo. Ou:
	// Se o ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	else if ((maxTopRight.x > maxBottomRight.x) || (maxBottomRight.y < centerH+shift && euclideanDist(maxTopRight, rightShoulder)>50)) {
		// O ponto mais alto
		rightHand = maxTopRight;
	} else {
		// O ponto mais baixo
		rightHand = maxBottomRight;
	}



	// Right Elbow/Cotovelo
	rightElbow.x=0;
	// Analisando apenas o lado direito do corpo. Sera cotovelo se:
	//
	if ((rightHand.y > centerH+shift) || (abs(rightShoulder.y-rightHand.y)<40 && abs(maxBottomRight.y-rightHand.y)<30 && rightHand.x-centerW > afa28)) { // braco esticado
		rightElbow = Point((rightHand.x+rightShoulder.x)/2, (rightHand.y+rightShoulder.y)/2);
	}
	else if (maxBottomRight.y > centerH-shift && maxBottomRight.y < centerH+shift) {
		rightElbow = maxBottomRight;
	}

	// Left Elbow/Cotovelo
	leftElbow.x=0;
	if ((leftHand.y > centerH+shift) || (abs(leftShoulder.y-leftHand.y)<40 && abs(maxBottomLeft.y-leftHand.y)<30 && centerW-leftHand.x > afa28)) { // braco esticado
		leftElbow = Point((leftHand.x+leftShoulder.x)/2, (leftHand.y+leftShoulder.y)/2);
	}
	else if (maxBottomLeft.y > centerH-shift && maxBottomLeft.y < centerH+shift) {
		leftElbow = maxBottomLeft;
	}



	LineIterator it(frame, leftShoulder, leftElbow, 8);
	for(int i = 0; i < it.count; i++, ++it)
	{
	    Point pt= it.pos(); 
	    //Draw Some stuff using that Point pt
	}
}


void Skeleton::drawMarkers(Mat &frame) {
	circle( frame, maxRight,        7, Scalar(255,255,0), 2, 8, 0 );
	circle( frame, maxLeft,         7, Scalar(255,255,0), 2, 8, 0 );
	circle( frame, maxBottomCenter, 7, Scalar(255,0,0),   2, 8, 0 );
	circle( frame, maxBottomRight,  7, Scalar(255,0,0),   2, 8, 0 );
	circle( frame, maxBottomLeft,   7, Scalar(255,0,0),   2, 8, 0 );
	circle( frame, maxTopCenter,    7, Scalar(0,255,255), 2, 8, 0 );
	circle( frame, maxTopRight,     7, Scalar(0,255,255), 2, 8, 0 );
	circle( frame, maxTopLeft,      7, Scalar(0,255,255), 2, 8, 0 );
	//imgHand.copyTo(frame.rowRange(maxTopRight.y, maxTopRight.y+imgHand.rows).colRange(maxTopRight.x, maxTopRight.x+imgHand.cols));
	/*if (maxTopLeft.x>0 && maxTopLeft.y>0) {
		cv::Rect roi( maxTopLeft, cv::Size( imgHand.cols, imgHand.rows));
		imgHand.copyTo( frame( roi ) );
	}*/
	//imgHand.copyTo(frame(cv::Rect(maxTopRight.x, maxTopRight.y, imgHand.cols, imgHand.rows)));
//return;
	// Desenha os quadrantes (linhas)
	Point ini = Point(centerW, 1);	
	Point fim = Point(centerW, height-1);
	Scalar c = Scalar(0,255,255);
	line(frame, ini, fim, c, 1, 8, 0 );
	ini.x -= afa*subSample; fim.x -= afa*subSample;
	line(frame, ini, fim, c, 1, 8, 0 );
	ini.x += afa*subSample*2; fim.x += afa*subSample*2;
	line(frame, ini, fim, c, 1, 8, 0 );

	ini = Point(1      , centerH);
	fim = Point(width-1, centerH);
	line(frame, ini, fim, c, 1, 8, 0 );

	// Cabeca
	circle( frame, head,           30, Scalar(255,255,255), 2, 8, 0 );

	// Ombros
	circle( frame, rightShoulder,  30, Scalar(255,255,255), 2, 8, 0 );
	circle( frame, leftShoulder,   30, Scalar(255,255,255), 2, 8, 0 );
	c = Scalar(255,255,255);
	line(frame, leftShoulder, rightShoulder, c, 2, 8, 0 );
	fim = Point((rightShoulder.x+leftShoulder.x)/2, abs(rightShoulder.y+leftShoulder.y)/2);
	line(frame, maxTopCenter, fim, c, 2, 8, 0 );

	// linha base
	ini = fim;
	fim = Point(ini.x, maxBottomCenter.y);
	line(frame, ini, fim, c, 2, 8, 0 );


	// Maos
	circle( frame, leftHand, 30, Scalar(255,0,255), 2, 8, 0 );
	circle( frame, rightHand, 30, Scalar(255,0,255), 2, 8, 0 );



	// Cotovelos
	if (rightElbow.x!=0) {
		circle( frame, rightElbow, 30, Scalar(0,0,255), 2, 8, 0 );
		line(frame, rightShoulder, rightElbow, c, 2, 8, 0 );
		line(frame, rightElbow, rightHand, c, 2, 8, 0 );
	} else {
		line(frame, rightShoulder, rightHand, c, 2, 8, 0 );
	}
	if (leftElbow.x!=0) {
		circle( frame, leftElbow, 30, Scalar(0,0,255), 2, 8, 0 );
		line(frame, leftShoulder, leftElbow, c, 2, 8, 0 );
		line(frame, leftElbow, leftHand, c, 2, 8, 0 );
	} else {
		line(frame, leftShoulder, leftHand, c, 2, 8, 0 );
	}
}


void Skeleton::removeLixo(Mat *frame, int cc) {
	int x,y;
	int w = frame->cols;
	int h = frame->rows;
	int c=0;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (	frame->data[(y-1)*w+x-1]==0 && frame->data[(y-1)*w+x]==0   && frame->data[(y-1)*w+x+1]==0 &&
				frame->data[ y   *w+x-1]==0 && frame->data[ y   *w+x]==255 && frame->data[ y   *w+x+1]==0 &&
				frame->data[(y+1)*w+x-1]==0 && frame->data[(y+1)*w+x]==0   && frame->data[(y+1)*w+x+1]==0)
			{
				frame->data[ y   *w+x]=0;
				c++;
			}
		}
	}
	
	//printf("Pixels removidos = %d\n", c);
}


void Skeleton::drawOverFrame(Mat * skelImg, Mat * frame) {
	Scalar cor = Scalar(0,255,0);
	int w = skelImg->cols;
	int h = skelImg->rows;
	int x,y;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (skelImg->data[y*w+x]==255) {
				//circle(Mat& img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
				circle(*frame, Point(x*subSample, y*subSample), 2, cor, 2, 8, 0);
				//circle(*frame, Point(x, y), 1, cor, 1, 8, 0);
				//frame->data[y*subSample*w*subSample*3 +x*subSample*3    ] = 0;
				//frame->data[y*subSample*w*subSample*3 +x*subSample*3 + 1] = 255;
				//frame->data[y*subSample*w*subSample*3 +x*subSample*3 + 2] = 0;
				//frame->data[y*subSample*w*3 +x*3    ] = 0;
				//frame->data[y*subSample*w*3 +x*3 + 1] = 255;
				//frame->data[y*subSample*w*3 +x*3 + 2] = 0;
			}
		}
	}
}


void Skeleton::drawOverFrame(std::list<cv::Point> pontos, Mat * frame) {
	Scalar cor = Scalar(0,255,255);
	Point p;

	for (std::list<Point>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
		p = *it;
		circle(*frame, Point(p.x*subSample, p.y*subSample), 2, cor, 2, 8, 0);
	}
}


void Skeleton::detectBiggerRegion(Mat * frame) {
	int x,y;
	int xM, yM, maior=0;
	int size;
	wC = frame->cols;
	hC = frame->rows;
	unsigned char datacp[wC*hC];


	maior = 0;
	memcpy(datacp, frame->data, wC*hC);
//printf("aqui2 %d %d\n", wC, hC); sleep(1);
	for (y=0 ; y<hC ; y++) {
		for (x=0 ; x<wC ; x++) {
			if (datacp[y*wC+x]==255) {
				size = 0;
				getSizeRegion(datacp, x, y, &size);
				if (maior < size) {
					//printf("maior = %d, size = %d\n", maior, size);
					// apaga a regiao que nao eh a maior da imagem original.
					if (maior>0)
						clearRegion(frame->data, xM, yM);

					maior = size;
					xM = x;
					yM = y;
				}
				else
					// apaga a regiao que nao eh a maior da imagem original.
					clearRegion(frame->data, x, y);
			}
		}
	}
}



void Skeleton::removeSmallsRegions(Mat * frame) {
	int x,y;
	int size;
	wC = frame->cols;
	hC = frame->rows;
	unsigned char datacp[wC*hC];

	memcpy(datacp, frame->data, wC*hC);

	for (y=0 ; y<hC ; y++) {
		for (x=0 ; x<wC ; x++) {
			if (datacp[y*wC+x]==255) {
				size = 0;
				getSizeRegion(datacp, x, y, &size);
				if (size<6) {
					clearRegion(frame->data, x, y);
				}
			}
		}
	}
}


cv::Point Skeleton::mediaPoint(Mat * frame) {
	int w = frame->cols;
	int h = frame->rows;
	int x, y;
	Point media = Point(0, 0);
	int c=1;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (frame->data[y*w+x]==255) {
				media.x += x;
				media.y += y;
				c++;
			}
		}
	}
	if (c>1) c--;
	media.x /= c;
	media.y /= c;

	return media;
}


// so entra aqui se o ponto mais a direita e o mais baixo tiverem o y proximos, Ou o mais a esquerda e o mais baixo.
std::vector<cv::Point> Skeleton::detectaRetas(Mat * skeleton, bool right) {
	int w = skeleton->cols;
	int h = skeleton->rows;
	int x, y;
	Point media = Point(0, 0);
	int c=1;
	std::vector<cv::Point> retas;

	int ini = 0, fim = w/2+afa;
	if (right) {
		ini = w/2+afa;
		fim = w;
	}

	for (y=0 ; y<h ; y++) {

		for (x=ini ; x<fim ; x++) {
			if (skeleton->data[y*w+x]==255) {
				media.x += x;
				media.y += y;
				c++;
			}
		}
	}
	if (c>1) c--;
	media.x /= c;
	media.y /= c;

	return retas;
}



std::list<cv::Point> Skeleton::getSkeletonBraco(Mat * skeleton, bool right) {
	int w = skeleton->cols;
	int h = skeleton->rows;
	int x, y;
	std::list<cv::Point> pontos;
	std::list<cv::Point> pontos_ordered;
	Point p;

	int ini = w/2-afa, fim = 0;
	if (right) {
		ini = w/2+afa;
		fim = w;
	}

	// localiza todos os pontos do esqueleto
	for (x=ini ; x!=fim ; ) {
		for (y=0 ; y<h ; y++) {
			if (skeleton->data[y*w+x]==255) {
				p = Point(x, y);
				pontos.push_back(p);
			}
		}
		right ? x++ : x--;
	}

	Point first;
	float dist;
	float menorDist;
	std::list<Point>::iterator closest;
	Point closestP;
	int tam = 0;
	
	// acha o mais proximo
	if (!pontos.empty()) {
		pontos_ordered.push_back(pontos.front());
		pontos.erase(pontos.begin());
		while (tam!=pontos.size()) {
			Point first = pontos_ordered.back();
			menorDist = 999999;
			tam = pontos.size();
			std::list<Point>::iterator it;
			for (it = pontos.begin(); it != pontos.end(); ++it) {
				dist = euclideanDist(first, *it);
				if (dist<menorDist && dist<20) {
					menorDist = dist;
					closest = it;
					if (dist<2) {
						//printf("dist == %f\n", dist);
						break;
					}
				}
			}
			if (menorDist < 999999) {
				closestP = *closest;
				pontos_ordered.push_back(closestP);
				pontos.erase(closest);
			}
		}
	}

	return pontos_ordered;
}


void Skeleton::clearRegion(unsigned char * frame, int x, int y) {
//printf("aqui3 %d %d\n", x, y);
	if (x<wC && y<hC && frame[y*wC+x]==255) {
		frame[y*wC+x]=0;
		clearRegion(frame, x-1, y-1);
		clearRegion(frame, x  , y-1);
		clearRegion(frame, x+1, y-1);
		clearRegion(frame, x-1, y);
		clearRegion(frame, x+1, y);
		clearRegion(frame, x-1, y+1);
		clearRegion(frame, x  , y+1);
		clearRegion(frame, x+1, y+1);
	}
}

void Skeleton::getSizeRegion(unsigned char * frame, int x, int y, int *quant) {
	if (x<wC && y<hC && frame[y*wC+x]==255) {
//printf("aqui4 %d %d %d\n", x, y, *quant);
		frame[y*wC+x]=0;
		(*quant)++;
		getSizeRegion(frame, x-1, y-1, quant);
		getSizeRegion(frame, x  , y-1, quant);
		getSizeRegion(frame, x+1, y-1, quant);
		getSizeRegion(frame, x-1, y,   quant);
		getSizeRegion(frame, x+1, y,   quant);
		getSizeRegion(frame, x-1, y+1, quant);
		getSizeRegion(frame, x  , y+1, quant);
		getSizeRegion(frame, x+1, y+1, quant);
	}
}


cv::Mat * Skeleton::skeletization(cv::Mat &binarized) {
	int width = binarized.cols;
	int height = binarized.rows;
	Mat * skeleton = new Mat(cv::Size(width, height), CV_8UC1, cv::Scalar(0));
	Mat temp;
	Mat eroded;
	Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
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

	Point media = mediaPoint(skeleton);
	centerW = media.x * subSample;
	centerH = media.y * subSample;

	return skeleton;
}


