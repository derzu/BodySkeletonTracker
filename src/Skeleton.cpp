#include "Skeleton.h"
#include <stdio.h>
#include <GL/freeglut.h>
#include <opencv2/imgproc.hpp>
#include <strings.h>
#include <list>

#include "AuxFuncs.h"

//#define DEBUG

using namespace cv;

// TODO 
// DONE Media dos 10 ultimos
// DONE Nao exibir os pontos quando nao encontra-los.
// Cabeca nao pegar a mao quando levantar as maos
// Gravar video

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

	rightShoulder = Point(10, 10);
	leftShoulder = Point(10, 10);

	bzero(rightHandV,     sizeof(cv::Point)*BUF_SIZE);
	bzero(rightElbowV,    sizeof(cv::Point)*BUF_SIZE);
	bzero(rightShoulderV, sizeof(cv::Point)*BUF_SIZE);
	bzero(leftHandV,      sizeof(cv::Point)*BUF_SIZE);
	bzero(leftElbowV,     sizeof(cv::Point)*BUF_SIZE);
	bzero(leftShoulderV,  sizeof(cv::Point)*BUF_SIZE);
	bzero(headV,          sizeof(cv::Point)*BUF_SIZE);
	bzero(centerWV,       sizeof(int)*BUF_SIZE);
	bzero(centerHV,       sizeof(int)*BUF_SIZE);

	rightHandH = rightElbowH = rightShoulderH = 0;
	leftHandH  = leftElbowH  = leftShoulderH = 0;
	headH = 0;
	centerWHead = centerHHead = 0;

	tiago = new Tiago();
}


Skeleton::~Skeleton() {
	if (tiago)
		delete tiago;
}


/**
 * Calculate de Euclidian distance between 2 points.
 **
float euclideanDist(cv::Point& p, cv::Point& q) {
	if (p.x==q.x && p.y==q.y)
		return 0;
	cv::Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}
*/
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



/**
 * Localiza os ombros, tecnica bem simples. Pega a posicao X da cabeca adiciona +-afa e baixa o Y ate chegar em algum ponto valido
 * Deve receber como parametro a imagem binarizada e nao o esqueleto.
 **/
void Skeleton::locateShoulders(cv::Mat &frame) {
	int width = frame.cols;
	int height = frame.rows;

	int centerWs = centerW/subSample;
	int aff = afa-2;

	char nAchou1 = 1;
	char nAchou2 = 1;

	// Y varia do maxTop + 30 ate 2*height/3. Supoe que os ombros estao sempre acima de 2/3 da imagem
	for (int y = head.y/subSample+30; y < 2*height/3; y++)
	//for (int y = 0; y < height; y++)
	{
		//printf("Achei y=%d \n", y);
		//if (centerWs+aff<width && centerWs-aff>0) 
		if (nAchou1 && centerWs+aff<width && (frame.data[y*width+centerWs+aff] == 255))
		{
			nAchou1 = 0;
			rightShoulder.x = (centerWs+aff)*subSample;
			rightShoulder.y = (y+10)*subSample; // adiciona 10 ao Y para ir para dentro do braco, nao ficar no ponto da borda.
			rightShoulderV[rightShoulderH++ % BUF_SIZE] = rightShoulder;
			rightShoulder = calculaMedia(rightShoulderV);

			if (!nAchou2) break;
		}
		if (nAchou2 && centerWs-aff>0 && (frame.data[y*width+centerWs-aff] == 255))
		{
			nAchou2 = 0;
			leftShoulder.x = (centerWs-aff)*subSample;
			leftShoulder.y = (y+10)*subSample; // adiciona 10 ao Y para ir para dentro do braco, nao ficar no ponto da borda.
			leftShoulderV[leftShoulderH++ % BUF_SIZE] = leftShoulder;
			leftShoulder = calculaMedia(leftShoulderV);

			if (!nAchou1) break;
		}
	}

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
 * @param frame matriz binarizada e utilizada para detecao dos ombros.
 **/
void Skeleton::locateMainBodyPoints(cv::Mat &frame) {
	// Cabeca. Sempre e o ponto mais alto.
	if (maxTopCenter.x!=0) {
		head = maxTopCenter;
		headV[headH++ % BUF_SIZE] = head;
		head = calculaMedia(headV);
	}

	// Ombros
	locateShoulders(frame);


	// Right Hand
	rightHand.x = 0;
	// Analisando apenas o lado direito do corpo. Sera mao se:
	if (maxRight.x!=0 && maxRight.x-centerW > afa28) // Certeza absoluta. Ponto mais a direita esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
	{
// TODO melhorar esse caso.
		// O ponto mais a direita
		rightHand = maxRight;
	// O ponto mais alto tiver a direita do ponto mais baixo. Ou:
	// O ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	} else if (maxTopRight.x!=0 && ((maxTopRight.x > maxBottomRight.x) || (maxBottomRight.y < centerH+shift && euclideanDist(maxTopRight, rightShoulder)>50))) {
		// O ponto mais alto
		rightHand = maxTopRight;
	} else if (maxBottomRight.x!=0) {
		// O ponto mais baixo
		rightHand = maxBottomRight;
	}
	if (rightHand.x!=0) {
		rightHandV[rightHandH++ % BUF_SIZE] = rightHand;
		rightHand = calculaMedia(rightHandV);
	}


	// Left Hand
	leftHand.x = 0;
	// Analisando apenas o lado esquerdo do corpo. Sera mao se:
	if (maxLeft.x!=0 && centerW-maxLeft.x > afa28) { // Certeza absoluta. Ponto mais a esquerda esta muito longe, entao nao tem como ser um cotovelo, sera uma mao.
		// O ponto mais a esquerda
		leftHand = maxLeft;
		leftHandV[leftHandH++ % BUF_SIZE] = leftHand;
	// Se o ponto mais alto tiver a esquerda do ponto mais baixo. Ou:
	// Se o ponto mais baixo, estiver acima da linha da cintura, e a distancia entre o ponto mais alto e o ombro for grande.
	} else if (maxTopLeft.x!=0 && ((maxTopLeft.x < maxBottomLeft.x) || (maxBottomLeft.y < centerH+shift && euclideanDist(maxTopLeft, leftShoulder)>50) )) {
		// O ponto mais alto
		leftHand = maxTopLeft;
		leftHandV[leftHandH++ % BUF_SIZE] = leftHand;
	} else if (maxBottomLeft.x!=0) {
		// O ponto mais baixo
		leftHand = maxBottomLeft;
		leftHandV[leftHandH++ % BUF_SIZE] = leftHand;
	}
	if (leftHand.x!=0) {
		leftHandV[leftHandH++ % BUF_SIZE] = leftHand;
		leftHand = calculaMedia(leftHandV);
	}


	// Right Elbow/Cotovelo
	rightElbow.x=0;
	// Analisando apenas o lado direito do corpo. Sera cotovelo se:

	// Mao esta mais a esquerda do que o cotovelo E o cotovelo nao pode estar proxima da mao E o x do Top, do Right e do Bottom nao estarem muito proximos E distante da mao
	if (maxRight.x!=0 && ( (maxRight.x-rightHand.x > 5) && euclideanDist(maxRight, rightHand)>20  && !( abs(maxRight.x-maxTopRight.x) < 15 && abs(maxRight.x-maxBottomRight.x) < 30) &&
		euclideanDist(maxRight, rightHand)>50)) {
		rightElbow = maxRight;
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
	if (rightElbow.x!=0) {
		rightElbowV[rightElbowH++ % BUF_SIZE] = rightElbow;
		rightElbow = calculaMedia(rightElbowV);
	}



	// Left Elbow/Cotovelo
	leftElbow.x=0;
	// Analisando apenas o lado esquerdo do corpo. Sera cotovelo se:
	// Mao esta mais a direita do que o cotovelo E o cotovelo nao pode estar proxima da mao E o x do Top, do Left e do Bottom nao estarem muito proximos E distante da mao
	if (maxLeft.x!=0 && ( (leftHand.x-maxLeft.x > 5) && euclideanDist(maxLeft, leftHand)>20  && !( abs(maxLeft.x-maxTopLeft.x) < 15 && abs(maxLeft.x-maxBottomLeft.x) < 30) &&
		euclideanDist(maxLeft, leftHand)>50)) {
		leftElbow = maxLeft;
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		//printf("\n\nelbow::case1\n");
	}
	// O ponto mais baixo estiver na linha da cintura (linha da cintura +- shift) E distante da mao E os Y's de maxBottom, maxLeft e middleArm NAO estiverem proximos.
	else if (maxBottomLeft.x!=0 && ((maxBottomLeft.y > centerH-shift*1.3 && maxBottomLeft.y < centerH+shift) && euclideanDist(maxBottomLeft, leftHand)>50 &&
               !( abs(maxBottomLeft.y-maxLeft.y) < 20 && abs(maxBottomLeft.y-middleArmLeft.y) < 20) )) {
		leftElbow = maxBottomLeft;
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		//printf("\n\nelbow::case2\n");
	}
	// O y do mais baixo e do mais a esquerda e o do middleArm forem muito proximos. E o y do ombro longe (caso do braco todo esticado na linha do ombro). E distante da mao
	else if (middleArmLeft.x!=0 && (abs(maxLeft.y-maxBottomLeft.y)<35 && abs(maxLeft.y-middleArmLeft.y)<35 && abs(leftShoulder.y-middleArmLeft.y)>30 &&
		euclideanDist(middleArmLeft, leftHand)>50 )) {
		leftElbow = middleArmLeft;
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		//printf("\n\nelbow::case3\n");
	}
	// O x do mais baixo e do mais a esquerda e o do middleArm forem muito proximos.  E distante da mao
	else if (middleArmLeft.x!=0 && (abs(maxLeft.x-maxBottomLeft.x)<35 && abs(maxLeft.x-middleArmLeft.x)<35 && /*abs(leftShoulder.y-middleArmLeft.y)>30 &&*/
		euclideanDist(middleArmLeft, leftHand)>50 )) {
		leftElbow = middleArmLeft;
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		//printf("\n\nelbow::case4\n");
	}
	// A mao estiver abaixo do centro OU
	// A altura da mao para o ombro for pequena E a altura da mao para o ponto mais baixo for pequena E mao esquerda nao esta colada no corpo.
	else if ((leftHand.x!=0 && leftShoulder.x!=0) &&
		((leftHand.y > centerH+shift) || (abs(leftShoulder.y-leftHand.y)<40 && abs(maxBottomLeft.y-leftHand.y)<30 && centerW-leftHand.x > afa28))) { 
		leftElbow = Point((leftHand.x+leftShoulder.x)/2, (leftHand.y+leftShoulder.y)/2); // braco esticado
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		//printf("\n\nelbow::case5\n");
	}
	if (leftElbow.x!=0) {
		leftElbowV[leftElbowH++ % BUF_SIZE] = leftElbow;
		leftElbow = calculaMedia(leftElbowV);
	}


/*
	LineIterator it(frame, leftShoulder, leftElbow, 8);
	for(int i = 0; i < it.count; i++, ++it)
	{
	    Point pt= it.pos(); 
	    //Draw Some stuff using that Point pt
	}*/
}


// TODO testar com a mediana
cv::Point Skeleton::calculaMedia(cv::Point vector[]) {
	Point m = Point(0,0);
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


/**
 * Mediana
 **/
cv::Point Skeleton::calculaMedia2(cv::Point vector[]) {
	int q1=0, q2=0;
	Point m = Point(0,0);

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


int Skeleton::calculaMedia(int vector[]) {
	int m = 1;
	int q=0;
	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (vector[i]!=0) {
			m += vector[i];
			q++;
		}
	}
	if (q>0)
		m /= q;

	return m;
}


/**
 * Mediana
 **/
int Skeleton::calculaMedia2(int vector[]) {
	int m = 1;
	int q=0;

	quick_sort(vector, 0, BUF_SIZE);

	for (int i=0 ; i<BUF_SIZE ; i++) {
		if (vector[i]!=0) {
			q++;
		}
	}
	if (q>0)
		return vector[q/2+BUF_SIZE-q];

	return m;
}

void Skeleton::drawMarkers(Mat &frame) {
	static int draws = 0;
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
		circle( frame, middleArmRight,  7, Scalar(255,255,255), 2, 8, 0 );
	if (middleArmLeft.x != 0)
		circle( frame, middleArmLeft,   7, Scalar(255,255,255), 2, 8, 0 );
#endif

//return;

	// Desenha os quadrantes (linhas)
	Point ini = Point(centerW, 1);	
	Point fim = Point(centerW, height-1);
	Scalar c = Scalar(0,255,255);
	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical central
	ini.x -= afa*subSample; fim.x -= afa*subSample;
	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical esquerda
	ini.x += afa*subSample*2; fim.x += afa*subSample*2;
	line(frame, ini, fim, c, 1, 8, 0 ); // linha vertical direita

	ini = Point(1      , centerH);
	fim = Point(width-1, centerH);
	line(frame, ini, fim, c, 1, 8, 0 ); // linha horizontal central

	// Cabeca/Head
	circle( frame, head,           20, Scalar(255,255,0), 2, 8, 0 );

	// Maos/Hands
	if (leftHand.x != 0)
		circle( frame, leftHand, 20, Scalar(0,255,0), 2, 8, 0 );
	if (rightHand.x != 0)
		circle( frame, rightHand, 20, Scalar(0,255,0), 2, 8, 0 );

	// Ombros/Shoulders
	circle( frame, rightShoulder,  20, Scalar(255,255,0), 2, 8, 0 );
	circle( frame, leftShoulder,   20, Scalar(255,255,0), 2, 8, 0 );

	c = Scalar(255,255,255);
	line(frame, leftShoulder, rightShoulder, c, 2, 8, 0 ); // linha entre os ombros
	fim = Point((rightShoulder.x+leftShoulder.x)/2, abs(rightShoulder.y+leftShoulder.y)/2);
	line(frame, head, fim, c, 2, 8, 0 ); // linha da cabeca e o centro dos ombros

	// linha/line base
	ini = fim;
	fim = Point(ini.x, maxBottomCenter.y);
	line(frame, ini, fim, c, 2, 8, 0 ); // linha central do corpo do esqueleto. Entre o meio dos ombros e a parte de baixo.



	// Cotovelos/Elbows
	if (rightElbow.x != 0) {
		circle( frame, rightElbow, 20, Scalar(255,0,0), 2, 8, 0 );
		if (rightShoulder.x!=0 && rightElbow.x!=0)
			line(frame, rightShoulder, rightElbow, c, 2, 8, 0 );
		if (rightHand.x!=0)
			line(frame, rightElbow, rightHand, c, 2, 8, 0 );
	} else if (rightShoulder.x!=0 && rightHand.x!=0) {
		line(frame, rightShoulder, rightHand, c, 2, 8, 0 );
	}
	if (leftElbow.x != 0) {
		circle( frame, leftElbow, 20, Scalar(255,0,0), 2, 8, 0 );
		if (leftShoulder.x!=0 && leftElbow.x!=0)
			line(frame, leftShoulder, leftElbow, c, 2, 8, 0 );
		if (leftHand.x!=0)
			line(frame, leftElbow, leftHand, c, 2, 8, 0 );
	} else if (leftShoulder.x!=0 && leftHand.x!=0) {
		line(frame, leftShoulder, leftHand, c, 2, 8, 0 );
	}
}


/**
 * Desenha os pontos==255 de uma matriz (skelImg) sobre outra matriz (frame).
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

/**
 * Desenha o vetor de pontos sobre a matriz (frame).
 * @param pontos vetor com os pontos que serao desenhados em frame
 * @param frame Matriz onde os pontos serao desenhados.
 **/
void Skeleton::drawOverFrame(std::vector<cv::Point> pontos, Mat &frame) {
	Scalar cor = Scalar(0,255,255);
	Point p;

	for (std::vector<Point>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
		p = *it;
		circle(frame, Point(p.x*subSample, p.y*subSample), 2, cor, 2, 8, 0);
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
				getSizeRegion(datacp, x, y, &size);
				if (size<6) {
					clearRegion(frame->data, x, y);
				}
			}
		}
	}
}


/**
 * Calcula a media entre os pixels==255 da matriz
 * 
 * @return O ponto que tem os valores da medias x e y.
 **/
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

/**
 * Localiza um possivel ponto do cotovelo a partir dos pontos do braco
 * @param arm braco do esquelto
 * @return o ponto na curva do esquelto do braco
 **/
cv::Point * Skeleton::getElbowHard(std::vector<cv::Point> &arm) {
	cv::Point * p = NULL;

	// calcula as declividades
	double ang1, ang2=0, ang3=0, diff;
	for (int i=0; i<(int)arm.size()-1 ; i++) {
		//printf("%dx%d - %dx%d\n", arm[i].x, arm[i].y, arm[i+1].x, arm[i+1].y);
		if (euclideanDist(arm[i], arm[i+1])<5) {
			//ang1 = atan2(abs(arm[i].y-arm[i+1].y), abs(arm[i].x-arm[i+1].x) ) * 180./CV_PI;
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
 * @param skeleton matriz de ponstos com o esqueleto
 * @param right se verdadeiro busca pelo braco direito, falso pelo esquerdo.
 *
 * @return vetor de cv::Point com os pontos do braco
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
		if (right) {
			middleArmRight = *el;

		} else {
			middleArmLeft = *el;

		}
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

	centerWV[centerWHead++ % BUF_SIZE] = centerW;
	centerHV[centerHHead++ % BUF_SIZE] = centerH;

	this->centerW = calculaMedia(centerWV);
	this->centerH = calculaMedia(centerHV);

	return skeleton;
}



void Skeleton::detectTiagoCommands() {
	static int c=0;
	c++;

	// mao esquerda esticada e afastada do corpo, comandos ativados.
	if (leftHand.x!=0 && leftHand.x < centerW-afa*subSample*2 && leftHand.y>centerH+afa*subSample)
	{
		// Tronco
		int y1 = centerHV[(centerHHead-1) % BUF_SIZE];
		int y2 = centerHV[centerHHead   % BUF_SIZE];
		//printf("%d::Recebendo comandos (%d - %d)=%d\n", c++, y1, y2, y1 - y2);
		if (y1 - y2 > 20)
			printf("%d::TRONCO para BAIXO\n", c);
		if (y1 - y2 < -20)
			printf("%d::TRONCO para CIMA\n", c);


		// Braco // TODO verificar se o rightShoulder.y-middleArmRight.y eh menor ou maior.
		// O y do mais baixo e do mais a direita e o do middleArm forem muito proximos. E o y do ombro longe (caso do braco todo esticado na linha do ombro). E distante da mao
/*		if (middleArmRight.x!=0 && (abs(maxRight.y-maxBottomRight.y)<15 && abs(maxRight.y-middleArmRight.y)<15 && abs(rightShoulder.y-middleArmRight.y)<15 &&
		euclideanDist(middleArmRight, rightHand)>50 )) {
			printf("BRACO ESTICADO HORIZONTAL\n");
		}*/


		// so entra a cada 10c para nao poluir muito o terminal	
		//if (c%10==0)
		{
			float angShoulder, angElbow;
			// Angulo entre ombro e cotovelo
			if (rightHand.x!=0 && rightElbow.x!=0) {
				angShoulder = -atan2f(rightElbow.y-rightShoulder.y, rightElbow.x-rightShoulder.x)*180/CV_PI;
				angShoulder = (((int)angShoulder)/5)*5;
				tiago->setAngShoulder(angShoulder);
				//printf("ANG::COTOVELO::OMBRO::%.1f\n", angShoulder);
			}

			// Angulo entre antebraco e cotovelo
			if (rightHand.x!=0 && rightElbow.x!=0) {
				angElbow = -atan2f(rightHand.y-rightElbow.y, rightHand.x-rightElbow.x)*180/CV_PI;
				angElbow = (((int)angElbow)/5)*5;
				tiago->setAngElbow(angElbow);
				//printf("ANG::COTOVELO:: MAO ::%.1f\n\n", angElbow);
			}

			tiago->moveArmThread();
		}

	}

	// so entra a cada 10c para nao poluir muito o terminal	
	if (c%10==0)
		// se a mao esquerda estiver mais a esquerda do que o ombro, e ambos estiverem acima da linha da cintura
		if (leftHand.x!=0 && leftElbow.x!=0 && leftHand.y < centerH-afa && leftElbow.y < centerH)
		{
			if (leftHand.x - leftElbow.x < -25)
				printf("andar para frente\n");
			else if (leftHand.x - leftElbow.x > 20)
				printf("andar para tras\n");
		}
}












