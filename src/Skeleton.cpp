#include "Skeleton.h"
#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <strings.h>
#include <list>
#include <stack>
#include "DrawAux.h"

//#define DEBUG 

using namespace cv;

/**
 * This class process a frame and find the main skeleton points, that will be stored at a SkeletonPoints.
 *
 * @author derzu
 **/


/**
 * The constructor
 **/
Skeleton::Skeleton(int width, int height, int subSample) {
	this->width = width;
	this->height = height;
	this->subSample = subSample;

	initialize();
}

void Skeleton::initialize() {
	afa = 70/subSample;
	shift = 50;
	afa28 = afa*subSample*2.8;

	showSkeleton = false;

	sp = new SkeletonPoints();
}


Skeleton::~Skeleton() {
	//if (sp)
	//	delete sp;
}

void Skeleton::setDepthMat(short depth[]) {
	depthMat = depth;
}


/**
 * Locate some interesting points on the skeleton region:
 * The max right, max left, max bottom, maxtop.
 *
 * @param frame frame where the maximuns will be found.
 **/
void Skeleton::locateMaximus(cv::Mat *frame) {
	int width = frame->cols;
	int height = frame->rows;

	zeraMaximus();

	int centerWs = sp->center.x/subSample;

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
			/*else if (frame->data[y*width+x] == 0) {
				//printf("frameZ: %d\n", frame->data[y*width+x]);
			}
			else 
				;//printf("frame: %d\n", frame->data[y*width+x]);*/
		}
	}

	setMaximus();
}



/**
 * Localiza os ombros, tecnica bem simples. Pega a posicao X da cabeca adiciona +-afa e baixa o Y ate chegar em algum ponto valido
 * Deve receber como parametro a imagem binarizada e nao o esqueleto.
 *
 * @param frame frame where the Shoulders will be found.
 **/
void Skeleton::locateShoulders(cv::Mat &frame) {
	int width = frame.cols;
	int height = frame.rows;

	int centerWs = sp->center.x/subSample;
	int aff = afa-2;

	char nAchou1 = 1;
	char nAchou2 = 1;

	// Y varia do maxTop + 30 ate 2*height/3. Supoe que os ombros estao sempre acima de 2/3 da imagem
	for (int y = sp->head.y/subSample+30; y < 2*height/3; y++)
	//for (int y = 0; y < height; y++)
	{
		//printf("Achei y=%d \n", y);
		//if (centerWs+aff<width && centerWs-aff>0) 
		if (nAchou1 && centerWs+aff<width && (frame.data[y*width+centerWs+aff] == 255))
		{
			nAchou1 = 0;
			sp->rightShoulder.x = (centerWs+aff)*subSample;
			sp->rightShoulder.y = (y+10)*subSample; // adiciona 10 ao Y para ir para dentro do braco, nao ficar no ponto da borda.
			obtainZ(sp->rightShoulder);
			sp->computePoint(SkeletonPoints::RIGHT_SHOULDER);

			if (!nAchou2) break;
		}
		if (nAchou2 && centerWs-aff>0 && (frame.data[y*width+centerWs-aff] == 255))
		{
			nAchou2 = 0;
			sp->leftShoulder.x = (centerWs-aff)*subSample;
			sp->leftShoulder.y = (y+10)*subSample; // adiciona 10 ao Y para ir para dentro do braco, nao ficar no ponto da borda.
			obtainZ(sp->leftShoulder);
			sp->computePoint(SkeletonPoints::LEFT_SHOULDER);

			if (!nAchou1) break;
		}
	}

	if (nAchou1 && nAchou2)
		showSkeleton = false;
	else
		showSkeleton = true;

//printf("0::centerWs = %d %d %d (%d)\n", centerWs, rightShoulder.x, leftShoulder.x, rightShoulder.x-leftShoulder.x);
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
	middleArmRight45.x = 0;
	middleArmLeft45.x = 0;
	maxRightMaxBottom.x = 0;
	maxLeftMaxBottom.x = 0;
	middleStraightArmRight.x = 0;
	middleStraightArmLeft.x = 0;
}

/**
 * After locate de maximus, they are stored at anothers variables.
 * Also set the middleStraightArm point.
 *
 * @see locateMaximus()
 **/
void Skeleton::setMaximus() {
	if (right.y>0) {
		maxRight.x = right.x*subSample;
		maxRight.y = right.y*subSample;
		obtainZ(maxRight);
	}
	if (left.y>0) {
		maxLeft.x = left.x*subSample;
		maxLeft.y = left.y*subSample;
		obtainZ(maxLeft);
	}
	if (topCenter.x>0)
	{
		maxTopCenter.x = topCenter.x*subSample;
		maxTopCenter.y = topCenter.y*subSample;
		obtainZ(maxTopCenter);
	}
	if (topRight.x>0)
	{
		maxTopRight.x = topRight.x*subSample;
		maxTopRight.y = topRight.y*subSample;
		obtainZ(maxTopRight);
	}
	if (topLeft.x>0)
	{
		maxTopLeft.x = topLeft.x*subSample;
		maxTopLeft.y = topLeft.y*subSample;
		obtainZ(maxTopLeft);
	}
	if (bottomCenter.x>0) {
		maxBottomCenter.x = bottomCenter.x*subSample;
		maxBottomCenter.y = bottomCenter.y*subSample;
		obtainZ(maxBottomCenter);
	}
	if (bottomRight.x>0) {
		maxBottomRight.x = bottomRight.x*subSample;
		maxBottomRight.y = bottomRight.y*subSample;
		obtainZ(maxBottomRight);
	}
	if (bottomLeft.x>0) {
		maxBottomLeft.x = bottomLeft.x*subSample;
		maxBottomLeft.y = bottomLeft.y*subSample;
		obtainZ(maxBottomLeft);
	}
	if (maxRight.x>0 && maxBottomRight.x>0) {
			// add 5 para ficar mais para dentro do braco
		maxRightMaxBottom = Point3D(maxRight.x-10, maxBottomRight.y);
		obtainZ(maxRightMaxBottom);
	}
	if (maxLeft.x>0 && maxBottomLeft.x>0) {
		maxLeftMaxBottom = Point3D(maxLeft.x+10, maxBottomLeft.y);
		obtainZ(maxLeftMaxBottom);
	}
	if (sp->rightHand.x>0 && sp->rightShoulder.x>0) {
		middleStraightArmRight = Point3D((sp->rightHand.x+sp->rightShoulder.x)/2, (sp->rightHand.y+sp->rightShoulder.y)/2); // braco esticado
		obtainZ(middleStraightArmRight);
	}
	if (sp->leftHand.x>0 && sp->leftShoulder.x>0) {
		middleStraightArmLeft = Point3D((sp->leftHand.x+sp->leftShoulder.x)/2, (sp->leftHand.y+sp->leftShoulder.y)/2); // braco esticado
		obtainZ(middleStraightArmLeft);
	}

}

/**
 * A partir da posicao xy o ponto z eh buscado na matriz de profundiade.
 *
 **/
void Skeleton::obtainZ(Point3D &point) {
#ifdef DEPTH
	point.z = getMeanDepthValue(point);
#endif
}

/**
 * Aqui ficam as regras de localização des alguns dos principais pontos do corpo:
 * Cabeca, ombros, cotovelos e maos.
 * @param frame matriz binarizada e utilizada para detecao dos ombros.
 **/
void Skeleton::locateMainBodyPoints(cv::Mat &frame) {
	// Cabeca. Sempre e o ponto mais alto.
	if (maxTopCenter.x!=0) {
		sp->head = maxTopCenter;
	}
	sp->computePoint(SkeletonPoints::HEAD);

	// Ombros
	locateShoulders(frame);

	//if (!showSkeleton)
		//return;

	// Right Hand
	sp->rightHand.x = 0;
	// Analisando apenas o lado direito do corpo. Sera mao se:
	// Certeza absoluta. Ponto mais a direita esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
	// OU os y do maxRight e do maxBottom e maxTop sao bem proximos.
	if (maxRight.x!=0 &&
	(maxRight.x - sp->center.x > afa28 || (abs(maxRight.y - maxBottomRight.y)<30 && abs(maxRight.y - maxTopRight.y)<30))  ) {
		// O ponto mais a direita
		sp->rightHand = maxRight;
		//printf("\n\nrightHand::case Right 1\n");
	// O ponto mais alto tiver a direita do ponto mais baixo. Ou:
	// O ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	} else if (maxTopRight.x!=0 && ((maxTopRight.x > maxBottomRight.x) || (maxBottomRight.y < sp->center.y+shift && maxTopRight.y < sp->rightShoulder.y+shift && DrawAux::euclideanDist(maxTopRight, sp->rightShoulder)>50))) {
		// O ponto mais alto
		sp->rightHand = maxTopRight;
		//printf("\n\nrightHand::case Top\n");
	} else if (maxBottomRight.x!=0) {
		// O ponto mais baixo
		sp->rightHand = maxBottomRight;
		//printf("\n\nrightHand::case Bottom\n");
	}
	//printf("rightHand::%d %d\n",sp->rightHand.x, sp->rightHand.y);
	sp->computePoint(SkeletonPoints::RIGHT_HAND);
	//printf("rightHand::%d %d\n",sp->rightHand.x, sp->rightHand.y);


	// Left Hand
	sp->leftHand.x = 0;
	// Analisando apenas o lado esquerdo do corpo. Sera mao se:
	
	// Certeza absoluta. Ponto mais a esquerda esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
	// OU os y do maxLeft e do maxBottom e maxTop sao bem proximos.
	if (maxLeft.x!=0 &&
	(sp->center.x-maxLeft.x > afa28 || (abs(maxLeft.y - maxBottomLeft.y)<30 && abs(maxLeft.y - maxTopLeft.y)<30))  ) {
		// O ponto mais a esquerda
		sp->leftHand = maxLeft;
	// Se o ponto mais alto tiver a esquerda do ponto mais baixo. Ou:
	// Se o ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	} else if (maxTopLeft.x!=0 && ((maxTopLeft.x < maxBottomLeft.x) || (maxBottomLeft.y < sp->center.y+shift && maxTopLeft.y < sp->leftShoulder.y+shift && DrawAux::euclideanDist(maxTopLeft, sp->leftShoulder)>50) )) {
		// O ponto mais alto
		sp->leftHand = maxTopLeft;
	} else if (maxBottomLeft.x!=0) {
		// O ponto mais baixo
		sp->leftHand = maxBottomLeft;
	}
	sp->computePoint(SkeletonPoints::LEFT_HAND);


	// Right Elbow/Cotovelo
	sp->rightElbow.x=0;
	int points;
	int menor;
	Point3D p;
	// braco esticado
	// Se o ponto middleStraightArm estiver dentro da area do corpo E longe da mao. Analisa variacoes do ponto com 10 pixels em 4 direcoes.
	if (middleStraightArmRight.x!=0 &&
	qPointsLineOutside(frame, middleStraightArmRight, sp->rightShoulder) < 10 &&
	(frame.data[(middleStraightArmRight.y/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255 ||
	 //frame.data[(middleStraightArmRight.y/subSample)*frame.cols + (middleStraightArmRight.x+10)/subSample]==255 ||
	 //frame.data[(middleStraightArmRight.y/subSample)*frame.cols + (middleStraightArmRight.x-10)/subSample]==255 ||
	 frame.data[((middleStraightArmRight.y+10)/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255 ||
	 frame.data[((middleStraightArmRight.y-10)/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255 ) ) {
		sp->rightElbow = middleStraightArmRight;
		//printf("\n\nelbow::case middleStraightArmRight \n" );
		//imwrite( "middleStraightArmRight.png", frame );
		if (frame.data[(middleStraightArmRight.y/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255)
			;
		else if (frame.data[((middleStraightArmRight.y+10)/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255)
			sp->rightElbow.y = middleStraightArmRight.y+10;
		else if (frame.data[((middleStraightArmRight.y-10)/subSample)*frame.cols + middleStraightArmRight.x/subSample]==255)
			sp->rightElbow.y = middleStraightArmRight.y-10;
	}
	else {
		menor=99999;
		for (std::vector<Point3D>::iterator it = rightArm.begin(); it != rightArm.end(); ++it) {
			p = *it;
			p *= subSample;
			points = qPointsLineOutside(frame, p, sp->rightHand) + qPointsLineOutside(frame, p, sp->rightShoulder);
			if (points<menor) {
				menor = points;
				sp->rightElbow = p;
			}		
		}
		//printf("menor=%d\n", menor);
	}
	obtainZ(sp->rightElbow);
	sp->computePoint(SkeletonPoints::RIGHT_ELBOW);
	



	// Left Elbow/Cotovelo
	sp->leftElbow.x=0;
	// Analisando apenas o lado esquerdo do corpo. Sera cotovelo se:





	// braco esticado
	// Se o ponto middleStraightArm estiver dentro da area do corpo E longe da mao.
	if (middleStraightArmLeft.x!=0 &&
	qPointsLineOutside(frame, middleStraightArmLeft, sp->leftShoulder) < 10 &&
	(frame.data[(middleStraightArmLeft.y/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255 ||
	 //frame.data[(middleStraightArmLeft.y/subSample)*frame.cols + (middleStraightArmLeft.x+10)/subSample]==255 ||
	 //frame.data[(middleStraightArmLeft.y/subSample)*frame.cols + (middleStraightArmLeft.x-10)/subSample]==255 ||
	 frame.data[((middleStraightArmLeft.y+10)/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255 ||
	 frame.data[((middleStraightArmLeft.y-10)/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255 ) ) {
		sp->leftElbow = middleStraightArmLeft;
		//printf("\n\nelbow::case middleStraightArmLeft\n");
		if (frame.data[(middleStraightArmLeft.y/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255)
			;
		else if (frame.data[((middleStraightArmLeft.y+10)/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255)
			sp->leftElbow.y = middleStraightArmLeft.y+10;
		else if (frame.data[((middleStraightArmLeft.y-10)/subSample)*frame.cols + middleStraightArmLeft.x/subSample]==255)
			sp->leftElbow.y = middleStraightArmLeft.y-10;
	}
	else {
		menor=99999;
		for (std::vector<Point3D>::iterator it = leftArm.begin(); it != leftArm.end(); ++it) {
			p = *it;
			p *= subSample;
			points = qPointsLineOutside(frame, p, sp->leftHand) + qPointsLineOutside(frame, p, sp->leftShoulder);
			if (points<menor) {
				menor = points;
				sp->leftElbow = p;
			}		
		}
		//printf("menor=%d\n", menor);
	}
	obtainZ(sp->leftElbow);
	sp->computePoint(SkeletonPoints::LEFT_ELBOW);



}


/**
 * Verify if the line between the 2 points has any point outside de body region.
 *
 * @param frame binarized image where the line will be checked.
 * @param p1 first point of the line.
 * @param p1 second point of the line.
 *
 * @return true if all the points of the line are inside the body region. false otherwise.
 **/
bool Skeleton::isLineInside(Mat &frame, cv::Point p1, cv::Point p2) {
	p1.x /= subSample;
	p1.y /= subSample;
	p2.x /= subSample;
	p2.y /= subSample;
	std::vector<cv::Point> * lines = DrawAux::lineBresenham(p1, p2);
	//printf("isLineInside::count=%ld::%d %d::%d %d\n", lines->size(), p1.x, p1.y, p2.x, p2.y);
	int q = 0;
	for(int i = 0; i < lines->size(); i++)
	{
	    Point pl= lines->at(i);
	    //printf("p x y = %d %d %d\n", pl.x, pl.y, frame.data[(pl.y/subSample)*frame.cols + pl.x/subSample]);
	    if (frame.data[pl.y*frame.cols + pl.x]!=255) {
	    	//printf("ponto outside %d!!\n", pl.x);
	    	q++;
	    	//break;
	    }
	}
	
	//printf("quantidade outside=%d\n", q);
	if (q>50)
		return false;
	
	if (lines)
		delete lines;
		
	return true;
}

int Skeleton::qPointsLineOutside(Mat &frame, cv::Point p1, cv::Point p2) {
	p1.x /= subSample;
	p1.y /= subSample;
	p2.x /= subSample;
	p2.y /= subSample;
	std::vector<cv::Point> * lines = DrawAux::lineBresenham(p1, p2);
	//printf("isLineInside::count=%ld::%d %d::%d %d\n", lines->size(), p1.x, p1.y, p2.x, p2.y);
	int q = 0;
	for(int i = 0; i < lines->size(); i++)
	{
	    Point pl= lines->at(i);
	    //printf("p x y = %d %d %d\n", pl.x, pl.y, frame.data[(pl.y/subSample)*frame.cols + pl.x/subSample]);
	    if (frame.data[pl.y*frame.cols + pl.x]!=255) {
	    	//printf("ponto outside %d!!\n", pl.x);
	    	q++;
	    }
	}
	
	//printf("quantidade outside=%d\n", q);
	return q;
}


/**
 * Draw the markers of the body skeleton.
 *
 * @param frame the frame where the markers will be drawn.
 *
 * @author derzu
 **/
void Skeleton::drawMarkers(Mat &frame) {
	static int draws = 0;

	if (!showSkeleton)
		return;

	draws++;

#ifdef DEBUG
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
		circle( frame, middleArmRight,  11, Scalar(255,255,255), 2, 8, 0 );
	if (middleArmLeft.x != 0)
		circle( frame, middleArmLeft,   11, Scalar(255,255,255), 2, 8, 0 );
	if (middleArmRight45.x != 0)
		circle( frame, middleArmRight45,  15, Scalar(0,255,255), 2, 8, 0 );
	if (middleArmLeft45.x != 0)
		circle( frame, middleArmLeft45,   15, Scalar(0,255,255), 2, 8, 0 );
	if (maxRightMaxBottom.x != 0)
		circle( frame, maxRightMaxBottom,  7, Scalar(0,0,255), 2, 8, 0 );
	if (maxLeftMaxBottom.x != 0)
		circle( frame, maxLeftMaxBottom,   7, Scalar(0,0,255), 2, 8, 0 );
	if (middleStraightArmRight.x != 0)
		circle( frame, middleStraightArmRight,  7, Scalar(0,0,255), 2, 8, 0 );
	if (middleStraightArmLeft.x != 0)
		circle( frame, middleStraightArmLeft,   7, Scalar(0,0,255), 2, 8, 0 );

#endif


//return;

	// Desenha os quadrantes (linhas)
	Point ini = Point(sp->center.x, 1);	
	Point fim = Point(sp->center.x, height-1);
	Scalar c = Scalar(0,255,255);
//	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical central
	ini.x -= afa*subSample; fim.x -= afa*subSample;
//	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical esquerda
	ini.x += afa*subSample*2; fim.x += afa*subSample*2;
//	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical direita

	ini = Point(1      , sp->center.y);
	fim = Point(width-1, sp->center.y);
//	line(frame, ini, fim, c, 1, 8, 0 ); // linha horizontal central

	// Cabeca/Head
	circle( frame, sp->head,           15, Scalar(255,255,0), 2, 8, 0 );

	// Center
	//circle( frame, sp->center,         20, Scalar(255,255,0), 2, 8, 0 );

	// Maos/Hands
	if (sp->leftHand.x != 0)
		circle( frame, sp->leftHand, 15, Scalar(0,255,0), 2, 8, 0 );
	if (sp->rightHand.x != 0)
		circle( frame, sp->rightHand, 15, Scalar(0,255,0), 2, 8, 0 );

	// Ombros/Shoulders
	circle( frame, sp->rightShoulder,  15, Scalar(255,255,0), 2, 8, 0 );
	circle( frame, sp->leftShoulder,   15, Scalar(255,255,0), 2, 8, 0 );

	c = Scalar(255,255,255);
	line(frame, sp->leftShoulder, sp->rightShoulder, c, 2, 8, 0 ); // linha entre os ombros
	fim = Point((sp->rightShoulder.x+sp->leftShoulder.x)/2, abs(sp->rightShoulder.y+sp->leftShoulder.y)/2);
	line(frame, sp->head, fim, c, 2, 8, 0 ); // linha da cabeca e o centro dos ombros

	// linha/line base
	ini = fim;
	fim = Point(ini.x, maxBottomCenter.y);
	line(frame, ini, fim, c, 2, 8, 0 ); // linha central do corpo do esqueleto. Entre o meio dos ombros e a parte de baixo.



	// Cotovelos/Elbowsfor (std::vector<Point3D>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
	if (sp->rightElbow.x != 0) {
		circle( frame, sp->rightElbow, 15, Scalar(255,0,0), 2, 8, 0 );
		if (sp->rightShoulder.x!=0 && sp->rightElbow.x!=0)
			line(frame, sp->rightShoulder, sp->rightElbow, c, 2, 8, 0 );
		if (sp->rightHand.x!=0)
			line(frame, sp->rightElbow, sp->rightHand, c, 2, 8, 0 );
	} else if (sp->rightShoulder.x!=0 && sp->rightHand.x!=0) {
		line(frame, sp->rightShoulder, sp->rightHand, c, 2, 8, 0 );
	}
	if (sp->leftElbow.x != 0) {
		circle( frame, sp->leftElbow, 15, Scalar(255,0,0), 2, 8, 0 );
		if (sp->leftShoulder.x!=0 && sp->leftElbow.x!=0)
			line(frame, sp->leftShoulder, sp->leftElbow, c, 2, 8, 0 );
		if (sp->leftHand.x!=0)
			line(frame, sp->leftElbow, sp->leftHand, c, 2, 8, 0 );
	} else if (sp->leftShoulder.x!=0 && sp->leftHand.x!=0) {
		line(frame, sp->leftShoulder, sp->leftHand, c, 2, 8, 0 );
	}

}



/**
 * Desenha os pontos==255 de uma matriz (skelImg) sobre outra matriz (frame).
 *
 * @param skelImg Matriz cujos de pontos serao desenhados em frame
 * @param frame Matriz onde os pontos serao desenhados.
 **/
void Skeleton::drawOverFrame(Mat * skelImg, Mat &frame) {
	Scalar cor = Scalar(0,255,0);
	int w = skelImg->cols;
	int h = skelImg->rows;
	int x,y;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (skelImg->data[y*w+x]==255) {
				//circle(Mat& img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
				circle(frame, Point(x*subSample, y*subSample), 1, cor, 2, 8, 0);
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



/**
 * Desenha os pontos==255 de uma matriz binaria (bin) sobre outra matriz (frame) em um tom de cinza.
 *
 * @param bin Matriz cujos de pontos serao desenhados em frame
 * @param frame Matriz onde os pontos serao desenhados.
 **/
void Skeleton::drawOverFrame2(Mat * bin, Mat &frame) {
	int w = bin->cols;
	int h = bin->rows;
	int x,y;

	for (y=0 ; y<h ; y++) {
		for (x=0 ; x<w ; x++) {
			if (bin->data[y*w+x]==255) {
				frame.data[y*w*3 + x*3    ] = 200;
				frame.data[y*w*3 + x*3 + 1] = 200;
				frame.data[y*w*3 + x*3 + 2] = 200;
			}
		}
	}
}


/**
 * Desenha o vetor de pontos sobre a matriz (frame).
 *
 * @param pontos vetor com os pontos que serao desenhados em frame
 * @param frame Matriz onde os pontos serao desenhados.
 **/
void Skeleton::drawOverFrame(std::vector<Point3D> pontos, Mat &frame) {
	Scalar cor = Scalar(0,255,255);
	Point3D p;

	for (std::vector<Point3D>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
		p = *it;
		circle(frame, Point(p.x*subSample, p.y*subSample), 1, cor, 2, 8, 0);
	}
}

/**
 * Detecta a maior regiao continua da imagem por "region growing". As demais regioes sao apagadas.
 *
 * @param frame Matriz onde a regiao sera localizada.
 **/
void Skeleton::detectBiggerRegion(Mat &frame) {
	int x,y;
	int xM, yM, maior=0;
	int size;
	wC = frame.cols;
	hC = frame.rows;
	unsigned char datacp[wC*hC];

	maior = 0;
	memcpy(datacp, frame.data, wC*hC);

	for (y=0 ; y<hC ; y++) {
		for (x=0 ; x<wC ; x++) {
			if (datacp[y*wC+x]==255) {
				size = 0;
				//getSizeRegionRecursive(datacp, x, y, &size);
				size = getSizeRegion(datacp, x, y);
				if (maior < size) {
					//printf("maior = %d, size = %d\n", maior, size);
					// apaga a regiao que nao eh a maior da imagem original.
					if (maior>0)
						clearRegion(frame.data, xM, yM);

					//printf("size1 = %d, size2 = %d\n", size, size2)	;
					maior = size;
					xM = x;
					yM = y;
				}
				else
					// apaga a regiao que nao eh a maior da imagem original.
					clearRegion(frame.data, x, y);

				// exclui a regiao para que ela nao seja mais buscada.
				clearRegion(datacp, x, y);
			}
		}
	}

	// localiza os pontos medios
	sp->center = mediaPoint(&frame);
	sp->center.x *= subSample;
	sp->center.y *= subSample;
	obtainZ(sp->center);
	//sp->center.y = sp->head.y + ((sp->rightShoulder.y+sp->leftShoulder.y)/2 - sp->head.y)*2;
	//centerHV[centerHHead++ % BUF_SIZE] = sp->center.y;
	//sp->center.y = calculaMedia(centerHV);
	sp->computePoint(SkeletonPoints::CENTER);
}



/**
 * Limpa a imagem removendo regioes pequenas.
 * 
 * @param frame imagem a ser limpa.
 **/
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
				//getSizeRegionRecursive(datacp, x, y, &size);
				size = getSizeRegion(datacp, x, y);
				if (size<6) {
					clearRegion(frame->data, x, y);
				}
			}
		}
	}
}

void Skeleton::analyse(cv::Mat * skeleton) {
	removeSmallsRegions(skeleton);
	locateMaximus(skeleton);
}


/**
 * Calcula a media entre os pixels==255 da matriz
 * 
 * @return O ponto que tem os valores da medias x e y.
 **/
Point3D Skeleton::mediaPoint(Mat * frame) {
	int w = frame->cols;
	int h = frame->rows;
	int x, y;
	Point3D media = Point3D(0, 0);
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




/**
 * Localiza um possivel ponto do cotovelo a partir dos pontos do braco
 * @param arm braco do esquelto
 * @return o ponto na curva do esquelto do braco
 **/
Point3D * Skeleton::getElbowHard(std::vector<Point3D> &arm, int ang) {
	Point3D * p = NULL;

	// calcula as declividades
	double ang1, ang2=0, ang3=0, diff;
	for (int i=0; i<(int)arm.size()-1 ; i++) {
		//printf("%dx%d - %dx%d\n", arm[i].x, arm[i].y, arm[i+1].x, arm[i+1].y);
		if (DrawAux::euclideanDist(arm[i], arm[i+1])<5) {
			ang1 = atan2((arm[i].y-arm[i+1].y), (arm[i].x-arm[i+1].x) ) * 180./CV_PI;
			//ang1 = atan2(abs(arm[i].y-arm[i+1].y), abs(arm[i].x-arm[i+1].x) ) * 180.;
			if (ang==-1) {
				if (ang1>-110 && ang1<-70)
					ang1 = -90;
				/*if (ang1>70 && ang1<110)
					ang1 = 90;*/
				if (ang1>-20 && ang1<20)
					ang1 = 0;
				if (ang1>160 || ang1<-160)
					ang1 = 180;
			}
			else {
				if (ang1>20 && ang1<70)
					ang1 = 45;
				if (ang1>-70 && ang1<-20)
					ang1 = -45;
				if (ang1>-160 && ang1<-110)
					ang1 = -135;
				if (ang1>110 && ang1<160)
					ang1 = 135;
			}
			diff = abs(ang1-ang2);
			
			// localiza uma sequencia de 3 ponstos iguais, esses pontos provavelmente estarao apos a curva do cotovelo.

//if (ang!=-1)			printf("ang: %6.2lf(%6.2lf)\n", ang1, diff);
			
			// localiza o ponto no meio da curva
			if (ang1==ang2 && ang2==ang3 && i>5 &&
			(((ang1==0  || ang1==-90 || /*ang1==90 ||*/ ang1==180 || ang1==47) && ang==-1) || 
			 ((ang1==45 || ang1==-45 || ang1==-135 || ang1==135) && ang!=-1) ) ) {
				p = new Point3D(arm[i].x*subSample, arm[i].y*subSample);
				break;
			}

			// guarda os 3 ultimos angulos
			ang3 = ang2;
			ang2 = ang1;
		}
	
	}
//if (ang!=-1)	printf("\n\n");

	return p;
}


/**
 * Localiza os pontos do esqueleto do braco direito ou esquerdo.
 *
 * @param skeleton matriz de ponstos com o esqueleto
 * @param right se verdadeiro busca pelo braco direito, falso pelo esquerdo.
 *
 * @return vetor de Point3D com os pontos do braco
 **/
std::vector<Point3D> Skeleton::getSkeletonArm(Mat * skeleton, bool right) {
	int w = skeleton->cols;
	int h = skeleton->rows;
	int x, y;
	//std::list<Point3D> pontos;
	std::vector<Point3D> pontos;
	std::vector<Point3D> pontos_ordered;
	std::vector<Point3D> pontos_ordered_smoth;
	Point3D p;

	int centerWs = sp->center.x/subSample;
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
				p = Point3D(x, y);
				pontos.push_back(p);
			}
		}
		right ? x++ : x--;
	}
	
	if (right)
		rightArm = pontos;
	else
		leftArm = pontos;
		
	return pontos; // TODO CONFIRM


	Point3D first;
	float dist;
	float menorDist;
	std::vector<Point3D>::iterator closest;
	Point3D closestP;
	int tam = 0;
	
	// Ordena os pontos
	// acha o mais proximo
	if (!pontos.empty()) {
		pontos_ordered.push_back(pontos.front());
		pontos.erase(pontos.begin());
		while (tam!=pontos.size()) {
			Point3D first = pontos_ordered.back();
			menorDist = 999999;
			tam = pontos.size();
			std::vector<Point3D>::iterator it;
			for (it = pontos.begin(); it != pontos.end(); ++it) {
				dist = DrawAux::euclideanDist(first, *it);
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
	Point3D m;
	int q;
	for (int i=0; i<pontos_ordered.size() ; i++) {
		m = Point3D(0,0);
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

	Point3D * el = getElbowHard(pontos_ordered_smoth, -1);
	if (el) {
		if (right) {
			middleArmRight = *el;

		} else {
			middleArmLeft = *el;

		}
		delete el;
	}
	el = getElbowHard(pontos_ordered_smoth, 45);
	if (el) {
		if (right) {
			middleArmRight45 = *el;

		} else {
			middleArmLeft45 = *el;

		}
		delete el;
	}
	

	return pontos_ordered_smoth;
}


/**
 * Clear a 8-Connected region with the point specified by the params xy. 
 * All the region must be with the 255 value and will be replaced by 0.
 * Keep doint that on the 8-Connected neighborhood recursively.
 * 
 * Recursive version
 *
 * @param frame the imagem frame
 * @param x coordinate of the point
 * @param y coordinate of the point
 **/
void Skeleton::clearRegionRecursive(unsigned char * frame, int x, int y) {
	if (x<wC && y<hC && x>=0 && y>=0 && frame[y*wC+x]==255) {
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
void Skeleton::clearRegion(unsigned char * frame, int x, int y) {
	std::stack<cv::Point> points;
	points.push(Point(x, y));
	frame[y*wC+x]=0;

	Point p;
	do {
		p = points.top();
		points.pop();
		x = p.x;
		y = p.y;
		if (verifyRegion(frame, x-1, y-1)) { points.push(Point(x-1, y-1)); }
		if (verifyRegion(frame, x  , y-1)) { points.push(Point(x  , y-1)); }
		if (verifyRegion(frame, x+1, y-1)) { points.push(Point(x+1, y-1)); }
		if (verifyRegion(frame, x-1, y  )) { points.push(Point(x-1, y  )); }
		if (verifyRegion(frame, x+1, y  )) { points.push(Point(x+1, y  )); }
		if (verifyRegion(frame, x-1, y+1)) { points.push(Point(x-1, y+1)); }
		if (verifyRegion(frame, x  , y+1)) { points.push(Point(x  , y+1)); }
		if (verifyRegion(frame, x+1, y+1)) { points.push(Point(x+1, y+1)); }
	} while (points.size()>0);
}


/**
 * Get the size of a 8-Connected region with the point specified by the params xy. 
 * All the region must be with the 255 value and will be replaced by 0.
 * Keep doint that on the 8-Connected neighborhood recursively.
 * 
 * Recursive version
 *
 * @param frame the imagem frame
 * @param x coordinate of the point
 * @param y coordinate of the point
 * @param quant the result (size of the region) will be stored here.
 **/
void Skeleton::getSizeRegionRecursive(unsigned char * frame, int x, int y, int *quant) {
	if (x<wC && y<hC && x>=0 && y>=0 && frame[y*wC+x]==255) {
		if (*quant > wC*hC*2/3)
			return;
		frame[y*wC+x]=0;
		(*quant)++;
		getSizeRegionRecursive(frame, x-1, y-1, quant);
		getSizeRegionRecursive(frame, x  , y-1, quant);
		getSizeRegionRecursive(frame, x+1, y-1, quant);
		getSizeRegionRecursive(frame, x-1, y,   quant);
		getSizeRegionRecursive(frame, x+1, y,   quant);
		getSizeRegionRecursive(frame, x-1, y+1, quant);
		getSizeRegionRecursive(frame, x  , y+1, quant);
		getSizeRegionRecursive(frame, x+1, y+1, quant);
	}
}


bool Skeleton::verifyRegion(unsigned char * frame, int x, int y) {
	if (x<wC && y<hC && x>=0 && y>=0 && frame[y*wC+x]==255)
	{
		frame[y*wC+x]=0;
		return true;
	}
	else
		return false;
}

/**
 * Get the size of a 8-Connected region with the point specified by the params xy. 
 * All the region must be with the 255 value and will be replaced by 0.
 * Keep doint that on the 8-Connected neighborhood recursively.
 * 
 * Iterative version
 *
 * @param frame the imagem frame
 * @param x coordinate of the point
 * @param y coordinate of the point
 * @param quant the result (size of the region) will be stored here.
 **/
int Skeleton::getSizeRegion(unsigned char * frame, int x, int y) {
	std::stack<cv::Point> points;
	points.push(Point(x, y));
	frame[y*wC+x]=0;
	int quant = 1;

	Point p;
	do {
		p = points.top();
		points.pop();
		x = p.x;
		y = p.y;
		if (verifyRegion(frame, x-1, y-1)) { quant++; points.push(Point(x-1, y-1)); }
		if (verifyRegion(frame, x  , y-1)) { quant++; points.push(Point(x  , y-1)); }
		if (verifyRegion(frame, x+1, y-1)) { quant++; points.push(Point(x+1, y-1)); }
		if (verifyRegion(frame, x-1, y  )) { quant++; points.push(Point(x-1, y  )); }
		if (verifyRegion(frame, x+1, y  )) { quant++; points.push(Point(x+1, y  )); }
		if (verifyRegion(frame, x-1, y+1)) { quant++; points.push(Point(x-1, y+1)); }
		if (verifyRegion(frame, x  , y+1)) { quant++; points.push(Point(x  , y+1)); }
		if (verifyRegion(frame, x+1, y+1)) { quant++; points.push(Point(x+1, y+1)); }
	
		if (quant > wC*hC*2/3)
			break;
	} while (points.size()>0);
	
	return quant;
}




SkeletonPoints* Skeleton::getSkeletonPoints() {
	return sp;
}

int Skeleton::getAfa() {
	return afa*subSample;
}


/**
 * Pega o valor medio de uma regiao 5x5 circundando o ponto x,y. Pontos com valor zero nao sao considerados
 *
 * @param center point of the region.
 **/
int Skeleton::getMeanDepthValue(cv::Point& p) {
	int v;
	int q=0;
	int t=0;
	int w = width;
	int h = height;
	
	if (p.x<w-2 && p.x>1 && p.y>1 && p.y<w-2) {
		v = depthMat[(p.y-2)*w + (p.x-2)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-2)*w + (p.x-1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-2)*w +  p.x];    
		if (v) { t += v; q++; }
		v = depthMat[(p.y-2)*w + (p.x+1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-2)*w + (p.x+2)];
		if (v) { t += v; q++; }
	
		v = depthMat[(p.y-1)*w + (p.x-2)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w + (p.x-1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w +  p.x];    
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w + (p.x+1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w + (p.x+2)];
		if (v) { t += v; q++; }

		v = depthMat[(p.y)*w + (p.x-2)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y)*w + (p.x-1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y)*w +  p.x];    
		if (v) { t += v; q++; }
		v = depthMat[(p.y)*w + (p.x+1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y)*w + (p.x+2)];
		if (v) { t += v; q++; }

		v = depthMat[(p.y+1)*w + (p.x-2)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + (p.x-1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w +  p.x];    
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + (p.x+1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + (p.x+2)];
		if (v) { t += v; q++; }

		v = depthMat[(p.y+2)*w + (p.x-2)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+2)*w + (p.x-1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+2)*w +  p.x];    
		if (v) { t += v; q++; }
		v = depthMat[(p.y+2)*w + (p.x+1)];
		if (v) { t += v; q++; }
		v = depthMat[(p.y+2)*w + (p.x+2)];
		if (v) { t += v; q++; }
		
	}
	
	if (q>0) {
		//printf("v9::%d::%d::%d::%.1f\n", t, q, t/q, (float)t/(float)q);
		return t/q;
	}
	else
		return t;
}


