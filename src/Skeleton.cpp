#include "Skeleton.h"
#include <stdio.h>
#include <GL/freeglut.h>
#include <opencv2/imgproc.hpp>

#include <list>

#define DEBUG 1

using namespace cv;

// TODO 
// Media dos 10 ultimos
// DONE Limpar os pontos
// Cabeca nao pegar a mao quando levantar as maos

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

/**
 * Calculate de Euclidian distance between 2 points.
 **/
float Skeleton::euclideanDist(Point& p, Point& q) {
	if (p.x==q.x && p.y==q.y)
		return 0;
	Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}


void Skeleton::locateMaximus(cv::Mat *frame) {
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



void Skeleton::locateShoulders(cv::Mat &frame) {
	int width = frame.cols;
	int height = frame.rows;

	int centerWs = centerW/subSample;
	int aff = afa-2;

	char nAchou1 = 1;
	char nAchou2 = 1;

	for (int y = maxTopCenter.y/subSample+40; y < height; y++)
	//for (int y = 0; y < height; y++)
	{
		//printf("Achei y=%d \n", y);
		//if (centerWs+aff<width && centerWs-aff>0) 
		if (nAchou1 && centerWs+aff<width && (frame.data[y*width+centerWs+aff] == 255))
		{
			nAchou1 = 0;
			rightShoulder.x = (centerWs+aff)*subSample;
			rightShoulder.y = (y+10)*subSample;

			if (!nAchou2) break;
		}
		if (nAchou2 && centerWs-aff>0 && (frame.data[y*width+centerWs-aff] == 255))
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
	maxRight.x = 0;
	maxLeft.x = 0;
	maxTopCenter.x = 0;
	maxTopRight.x = 0;
	maxTopLeft.x = 0;
	maxBottomCenter.x = 0;
	maxBottomRight.x = 0;
	maxBottomLeft.x = 0;
	middleArmRight.x = 0;
	middleArmLeft.x = 0;
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

/**
 * Aqui ficam as regras de localização des alguns dos principais pontos do corpo:
 * Cabeca, ombros, cotovelos e maos.
 * 
 **/
void Skeleton::locateMainBodyPoints(cv::Mat &frame) {
	float afa28 = afa*subSample*2.8;
	int shift = 50;

	// Cabeca. Sempre e o ponto mais alto.
	head = maxTopCenter;

	// Ombros
	locateShoulders(frame);

	// Right Hand
	rightHand.x = 0;
	// Analisando apenas o lado direito do corpo. Sera mao se:
	if (maxRight.x!=0 && maxRight.x-centerW > afa28) {// Certeza absoluta. Ponto mais a direita esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
		// O ponto mais a direita
		rightHand = maxRight;
	}
	// O ponto mais alto tiver a direita do ponto mais baixo. Ou:
	// O ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	else if (maxTopRight.x!=0 && ((maxTopRight.x > maxBottomRight.x) || (maxBottomRight.y < centerH+shift && euclideanDist(maxTopRight, rightShoulder)>50))) {
		// O ponto mais alto
		rightHand = maxTopRight;
	} else if (maxBottomRight.x!=0) {
		// O ponto mais baixo
		rightHand = maxBottomRight;
	}


	// Left Hand
	leftHand.x = 0;
	// Analisando apenas o lado esquerdo do corpo. Sera mao se:
	if (maxLeft.x!=0 && centerW-maxLeft.x > afa28) { // Certeza absoluta. Ponto mais a esquerda esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
		// O ponto mais a esquerda
		leftHand = maxLeft;
	}
	// Se o ponto mais alto tiver a esquerda do ponto mais baixo. Ou:
	// Se o ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	else if (maxTopLeft.x!=0 && ((maxTopLeft.x < maxBottomLeft.x) || (maxBottomLeft.y < centerH+shift && euclideanDist(maxTopLeft, leftShoulder)>50) )) {
		// O ponto mais alto
		leftHand = maxTopLeft;
	} else if (maxBottomLeft.x!=0) {
		// O ponto mais baixo
		leftHand = maxBottomLeft;
	}



	// Right Elbow/Cotovelo
	rightElbow.x=0;
	// Analisando apenas o lado direito do corpo. Sera cotovelo se:

	// Mao esta mais a esquerda do que o cotovelo E o cotovelo nao pode estar proxima da mao E o x do Top, do Right e do Bottom nao estarem muito proximos E distante da mao
	if (maxRight.x!=0 && ( (maxRight.x-rightHand.x > 5) && euclideanDist(maxRight, rightHand)>20  && !( abs(maxRight.x-maxTopRight.x) < 15 && abs(maxRight.x-maxBottomRight.x) < 30) &&
		euclideanDist(maxRight, rightHand)>50)) {
		rightElbow = maxRight;
		//printf("\n\nelbow::case1\n");
	}
	// O ponto mais baixo estiver na linha da cintura (linha da cintura +- shift) E distante da mao E os Y's de maxBottom, maxRight e middleArm NAO estiverem proximos.
	else if (maxBottomRight.x!=0 && ((maxBottomRight.y > centerH-shift*1.3 && maxBottomRight.y < centerH+shift) && euclideanDist(maxBottomRight, rightHand)>50 &&
               !( abs(maxBottomRight.y-maxRight.y) < 20 && abs(maxBottomRight.y-middleArmRight.y) < 20) )) {
		rightElbow = maxBottomRight;
		//printf("\n\nelbow::case2\n");
	}
	// O y do mais baixo e do mais a direita e o do middleArm forem muito proximos. E o y do ombro longe (caso do braco todo esticado na linha do ombro). E distante da mao
	else if (middleArmRight.x!=0 && (abs(maxRight.y-maxBottomRight.y)<35 && abs(maxRight.y-middleArmRight.y)<35 && abs(rightShoulder.y-middleArmRight.y)>30 &&
		euclideanDist(middleArmRight, rightHand)>50 )) {
		rightElbow = middleArmRight;
		//printf("\n\nelbow::case3\n");
	}
	// O x do mais baixo e do mais a direita e o do middleArm forem muito proximos.  E distante da mao
	else if (middleArmRight.x!=0 && (abs(maxRight.x-maxBottomRight.x)<35 && abs(maxRight.x-middleArmRight.x)<35 && /*abs(rightShoulder.y-middleArmRight.y)>30 &&*/
		euclideanDist(middleArmRight, rightHand)>50 )) {
		rightElbow = middleArmRight;
		//printf("\n\nelbow::case4\n");
	}
	// A mao estiver abaixo do centro OU
	// A altura da mao para o ombro for pequena E a altura da mao para o ponto mais baixo for pequena E mao direita nao esta colada no corpo.
	else if ((rightHand.x!=0 && rightShoulder.x!=0) &&
		((rightHand.y > centerH+shift) || (abs(rightShoulder.y-rightHand.y)<40 && abs(maxBottomRight.y-rightHand.y)<30 && rightHand.x-centerW > afa28))) { 
		rightElbow = Point((rightHand.x+rightShoulder.x)/2, (rightHand.y+rightShoulder.y)/2); // braco esticado
		//printf("\n\nelbow::case5\n");
	}



	// Left Elbow/Cotovelo
	leftElbow.x=0;
	// Analisando apenas o lado esquerdo do corpo. Sera cotovelo se:

	// Mao esta mais a direita do que o cotovelo E o cotovelo nao pode estar proxima da mao E o x do Top, do Left e do Bottom nao estarem muito proximos E distante da mao
	if (maxLeft.x!=0 && ( (leftHand.x-maxLeft.x > 5) && euclideanDist(maxLeft, leftHand)>20  && !( abs(maxLeft.x-maxTopLeft.x) < 15 && abs(maxLeft.x-maxBottomLeft.x) < 30) &&
		euclideanDist(maxLeft, leftHand)>50)) {
		leftElbow = maxLeft;
		//printf("\n\nelbow::case1\n");
	}
	// O ponto mais baixo estiver na linha da cintura (linha da cintura +- shift) E distante da mao E os Y's de maxBottom, maxLeft e middleArm NAO estiverem proximos.
	else if (maxBottomLeft.x!=0 && ((maxBottomLeft.y > centerH-shift*1.3 && maxBottomLeft.y < centerH+shift) && euclideanDist(maxBottomLeft, leftHand)>50 &&
               !( abs(maxBottomLeft.y-maxLeft.y) < 20 && abs(maxBottomLeft.y-middleArmLeft.y) < 20) )) {
		leftElbow = maxBottomLeft;
		//printf("\n\nelbow::case2\n");
	}
	// O y do mais baixo e do mais a esquerda e o do middleArm forem muito proximos. E o y do ombro longe (caso do braco todo esticado na linha do ombro). E distante da mao
	else if (middleArmLeft.x!=0 && (abs(maxLeft.y-maxBottomLeft.y)<35 && abs(maxLeft.y-middleArmLeft.y)<35 && abs(leftShoulder.y-middleArmLeft.y)>30 &&
		euclideanDist(middleArmLeft, leftHand)>50 )) {
		leftElbow = middleArmLeft;
		//printf("\n\nelbow::case3\n");
	}
	// O x do mais baixo e do mais a esquerda e o do middleArm forem muito proximos.  E distante da mao
	else if (middleArmLeft.x!=0 && (abs(maxLeft.x-maxBottomLeft.x)<35 && abs(maxLeft.x-middleArmLeft.x)<35 && /*abs(leftShoulder.y-middleArmLeft.y)>30 &&*/
		euclideanDist(middleArmLeft, leftHand)>50 )) {
		leftElbow = middleArmLeft;
		//printf("\n\nelbow::case4\n");
	}
	// A mao estiver abaixo do centro OU
	// A altura da mao para o ombro for pequena E a altura da mao para o ponto mais baixo for pequena E mao esquerda nao esta colada no corpo.
	else if ((leftHand.x!=0 && leftShoulder.x!=0) &&
		((leftHand.y > centerH+shift) || (abs(leftShoulder.y-leftHand.y)<40 && abs(maxBottomLeft.y-leftHand.y)<30 && centerW-leftHand.x > afa28))) { 
		leftElbow = Point((leftHand.x+leftShoulder.x)/2, (leftHand.y+leftShoulder.y)/2); // braco esticado
		//printf("\n\nelbow::case5\n");
	}



	LineIterator it(frame, leftShoulder, leftElbow, 8);
	for(int i = 0; i < it.count; i++, ++it)
	{
	    Point pt= it.pos(); 
	    //Draw Some stuff using that Point pt
	}
}


void Skeleton::drawMarkers(Mat &frame) {
	if (maxRight.x!=0)
		circle( frame, maxRight,        7, Scalar(255,255,0), 2, 8, 0 );
	if (maxLeft.x != 0)
		circle( frame, maxLeft,         7, Scalar(255,255,0), 2, 8, 0 );
	if (maxTopCenter.x != 0)
		circle( frame, maxTopCenter,    7, Scalar(0,255,255), 2, 8, 0 );
	if (maxTopRight.x != 0)
		circle( frame, maxTopRight,     7, Scalar(0,255,255), 2, 8, 0 );	
	if (maxTopLeft.x != 0)
		circle( frame, maxTopLeft,      7, Scalar(0,255,255), 2, 8, 0 );
	if (maxBottomCenter.x != 0)
		circle( frame, maxBottomCenter, 7, Scalar(255,0,0),   2, 8, 0 );
	if (maxBottomRight.x != 0)
		circle( frame, maxBottomRight,  7, Scalar(255,0,0),   2, 8, 0 );
	if (maxBottomLeft.x != 0)
		circle( frame, maxBottomLeft,   7, Scalar(255,0,0),   2, 8, 0 );
	if (middleArmRight.x != 0)
		circle( frame, middleArmRight,  7, Scalar(255,255,255), 2, 8, 0 );
	if (middleArmLeft.x != 0)
		circle( frame, middleArmLeft,   7, Scalar(255,255,255), 2, 8, 0 );

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
	if (leftHand.x != 0)
		circle( frame, leftHand, 30, Scalar(255,0,255), 2, 8, 0 );
	if (rightHand.x != 0)
		circle( frame, rightHand, 30, Scalar(255,0,255), 2, 8, 0 );


	// Cotovelos
	if (rightElbow.x != 0) {
		circle( frame, rightElbow, 30, Scalar(0,0,255), 2, 8, 0 );
		if (rightShoulder.x!=0 && rightElbow.x!=0)
			line(frame, rightShoulder, rightElbow, c, 2, 8, 0 );
		if (rightHand.x!=0)
			line(frame, rightElbow, rightHand, c, 2, 8, 0 );
	} else if (rightShoulder.x!=0 && rightHand.x!=0) {
		line(frame, rightShoulder, rightHand, c, 2, 8, 0 );
	}
	if (leftElbow.x != 0) {
		circle( frame, leftElbow, 30, Scalar(0,0,255), 2, 8, 0 );
		if (leftShoulder.x!=0 && leftElbow.x!=0)
			line(frame, leftShoulder, leftElbow, c, 2, 8, 0 );
		if (leftHand.x!=0)
			line(frame, leftElbow, leftHand, c, 2, 8, 0 );
	} else if (leftShoulder.x!=0 && leftHand.x!=0) {
		line(frame, leftShoulder, leftHand, c, 2, 8, 0 );
	}
}



void Skeleton::drawOverFrame(Mat * skelImg, Mat &frame) {
	Scalar cor = Scalar(0,255,0);
	int w = skelImg->cols;
	int h = skelImg->rows;
	int x,y;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (skelImg->data[y*w+x]==255) {
				//circle(Mat& img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
				circle(frame, Point(x*subSample, y*subSample), 2, cor, 2, 8, 0);
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


void Skeleton::drawOverFrame(std::vector<cv::Point> pontos, Mat &frame) {
	Scalar cor = Scalar(0,255,255);
	Point p;

	for (std::vector<Point>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
		p = *it;
		circle(frame, Point(p.x*subSample, p.y*subSample), 2, cor, 2, 8, 0);
	}
}


void Skeleton::detectBiggerRegion(Mat &frame) {
	int x,y;
	int xM, yM, maior=0;
	int size;
	wC = frame.cols;
	hC = frame.rows;
	unsigned char datacp[wC*hC];


	maior = 0;
	memcpy(datacp, frame.data, wC*hC);
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
						clearRegion(frame.data, xM, yM);

					maior = size;
					xM = x;
					yM = y;
				}
				else
					// apaga a regiao que nao eh a maior da imagem original.
					clearRegion(frame.data, x, y);
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


/**
 * Localiza um possivel ponto do cotovelo a partir dos pontos do braco
 *
 **/
cv::Point * Skeleton::getElbowHard(std::vector<cv::Point> &arm) {
	cv::Point * p = NULL;

	// calcula as declividades
	double ang1, ang2=0, ang3=0, diff;
	for (int i=0; i<(int)arm.size()-1 ; i++) {
		//printf("%dx%d - %dx%d\n", arm[i].x, arm[i].y, arm[i+1].x, arm[i+1].y);
		if (euclideanDist(arm[i], arm[i+1])<5) {
			ang1 = atan2(abs(arm[i].y-arm[i+1].y), abs(arm[i].x-arm[i+1].x) ) * 180.;
			if (ang1>250 && ang1<290)
				ang1 = 270;
			diff = abs(ang1-ang2);
			
			//printf("ang: %6.2lf(%6.2lf)\n", ang1, diff);
			// localiza o ponto no meio da curva
			if (ang1==ang2 && ang2==ang3 && (ang1==0 || ang1==270) && i>5) {
				p = new Point(arm[i].x*subSample, arm[i].y*subSample);
				break;
			}
			/*else if (ang1==ang2 && ang2==ang3 && ang1==270 && i>5) {
				p = new Point(arm[i].x*subSample, arm[i].y*subSample);
				break;
			}*/
			// guarda os 3 ultimos angulos
			ang3 = ang2;
			ang2 = ang1;
		}
	
	}
	//printf("\n\n\n\n");

	return p;
}


/**
 * Localiza os pontos do esqueleto do braco direito ou esquerdo.
 *
 **/
std::vector<cv::Point> Skeleton::getSkeletonArm(Mat * skeleton, bool right) {
	int w = skeleton->cols;
	int h = skeleton->rows;
	int x, y;
	std::list<cv::Point> pontos;
	std::vector<cv::Point> pontos_ordered;
	std::vector<cv::Point> pontos_ordered_smoth;
	Point p;

	int centerWs = centerW/subSample;
	int ini = centerWs-afa*1.2, fim = 0;
	if (ini<0) ini = 0;
	if (right) {
		ini = centerWs+afa*1.2;
		if (ini>=w) ini = w-1;
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
	
	// Ordena os pontos
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
				if (dist<menorDist && dist<10) {
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


	// Suaviza braco. Aplica uma mascara de media em formato de + de tamanho 7x7
	Point m;
	int q;
	for (int i=0; i<pontos_ordered.size() ; i++) {
		m = Point(0,0);
		q = 1;
		if (i-3>=0) {
			m.x += pontos_ordered[i-3].x;
			m.y += pontos_ordered[i-3].y;
			q++;
		}
		if (i-2>=0) {
			m.x += pontos_ordered[i-2].x;
			m.y += pontos_ordered[i-2].y;
			q++;
		}
		if (i-1>=0) {
			m.x += pontos_ordered[i-1].x;
			m.y += pontos_ordered[i-1].y;
			q++;
		}
		m.x += pontos_ordered[i].x;
		m.y += pontos_ordered[i].y;
		if (i+1<pontos_ordered.size()) {
			m.x += pontos_ordered[i+1].x;
			m.y += pontos_ordered[i+1].y;
			q++;
		}
		if (i+2<pontos_ordered.size()) {
			m.x += pontos_ordered[i+2].x;
			m.y += pontos_ordered[i+2].y;
			q++;
		}
		if (i+3<pontos_ordered.size()) {
			m.x += pontos_ordered[i+3].x;
			m.y += pontos_ordered[i+3].y;
			q++;
		}
		m.x /= q;
		m.y /= q;
		pontos_ordered_smoth.push_back(m);
	}


	cv::Point * el = getElbowHard(pontos_ordered_smoth);
	if (el) {
		if (right)
			middleArmRight = *el;
		else
			middleArmLeft = *el;
		delete el;
	}

	return pontos_ordered_smoth;
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

/**
 * Thinning/skeletonization operation over the binarized region of the body
 *
 * @param binarized - Mat with the binarized image
 * @return the thinning skeleton
 */
cv::Mat * Skeleton::thinning(cv::Mat &binarized) {
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


