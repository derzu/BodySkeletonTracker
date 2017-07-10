#include <Tiago.h>
#include <stdio.h>
#include <stdlib.h>

#include <Skeleton.h>

Tiago::Tiago() {
	moving = false;
	t = NULL;
	
	bzero(walkAngleQ, sizeof(int)*QUEUE_SIZE);
	walkAngleH = 0;
	
	walkDirection = NONE;
	walkAngle     = NONE;
	lastWalkDirection = -1;
	lastWalkAngle = -1;
	
	tronco=NONE;
	lastTronco=-1;
	
        socketC = new SocketClient(12345, "127.0.0.1");
        socketC->conecta();
        
        initArm();
}

void Tiago::initArm() {
	int r;
	r = system("rosrun play_motion move_joint arm_1_joint 0 0.2");
	r = system("rosrun play_motion move_joint arm_2_joint 0 0.2");
	r = system("rosrun play_motion move_joint arm_3_joint 0 0.2");
	r = system("rosrun play_motion move_joint arm_6_joint 0 0.2");
}

float Tiago::getAngElbow() {
	return angElbow;
}

float Tiago::getAngShoulder() {
	return angShoulder;
}

void Tiago::setMoving(bool m) {
	moving = m;
}

bool Tiago::isMoving() {
	return moving;
}

void Tiago::setAngElbow(float ang) {
	angShoulder = ang;
}

void Tiago::setAngShoulder(float ang) {
	angElbow = ang;
}

void Tiago::mutexLock() {
	mtx.lock();
}

void Tiago::mutexUnlock() {
	mtx.unlock();
}

void Tiago::moveArmThread() {
	if (moving) return;
	mutexLock();
	//if (t)
	//	delete t;
	//t = new std::thread(&Tiago::moveArm, this);

	int threadId = pthread_create( &thread1, NULL, &Tiago::moveArm, (void *)this);
}


void * Tiago::moveArm(void * t) {
	Tiago * tiago = (Tiago*)t;

	if (tiago->isMoving()) return NULL;

	tiago->setMoving(true);

	char comando[100];
	int joint;
	float ang;
	int r;

	/*if (aShoulder>0) {
		if (aElbow>0)
			ang = aElbow-aShoulder; // subtrai o shoulder do ombro
		else
			ang = aElbow-aShoulder; // soma os dois angulos
	}
	else {
		if (aElbow>0)
			ang = aElbow-aShoulder; // soma os dois angulos
		else
			ang = aElbow-aShoulder; // subtrai o shoulder do ombro
	}*/

	joint = 4;		
	ang = tiago->getAngElbow() - tiago->getAngShoulder();
	ang = ang*ELBOW_90/90.; // conversao do angulo para os valores compativeis com o Tiago.
	// O roscore ou gazebo tem que estar rodando previamente.
	sprintf(comando, "rosrun play_motion move_joint arm_%d_joint %.1f 0.2", joint, ang);
	printf("%s\n", comando);
	r = system(comando);

	joint = 2;
	ang = tiago->getAngShoulder();
	ang = ang*SHOULDER_45/45.; // conversao do angulo para os valores compativeis com o Tiago.
	// O roscore ou gazebo tem que estar rodando previamente.
	sprintf(comando, "rosrun play_motion move_joint arm_%d_joint %.1f 0.2", joint, ang);
	printf("%s\n", comando);
	r = system(comando);

	tiago->setMoving(false);
	tiago->mutexUnlock();
}

/*// pega o valor medio de uma regiao 3x3 circundando o ponto x,y. Pontos com valor zero nao sao considerados
int Tiago::getMeanValue(short depthMat[], cv::Point& p) {
	int v;
	int q=0;
	int t=0;
	int w = 640;
	int h = 480;
	
	if (p.x<w-1 && p.x>0 && p.y>0 && p.y<w-1) {
		v = depthMat[(p.y-1)*w + (p.x-1)];printf("v1=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w + p.x];    printf("v2=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y-1)*w + (p.x+1)];printf("v3=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y  )*w + (p.x-1)];printf("v4=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y  )*w + p.x];    printf("v5=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y  )*w + (p.x+1)];printf("v6=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + (p.x-1)];printf("v7=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + p.x];    printf("v8=%d\n", v);
		if (v) { t += v; q++; }
		v = depthMat[(p.y+1)*w + (p.x+1)];printf("v9=%d::%d\n", v, q);
		if (v) { t += v; q++; }
	}
	
	if (q>0)
		return t/q;
	else
		return t;
}*/

// pega o valor medio de uma regiao 5x5 circundando o ponto x,y. Pontos com valor zero nao sao considerados
int Tiago::getMeanValue(short depthMat[], cv::Point& p) {
	int v;
	int q=0;
	int t=0;
	int w = 640;
	int h = 480;
	
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

void Tiago::detectTiagoCommands(SkeletonPoints* sp, int afa, short depthMat[], closest_point::IntPoint3D& closest) {
	static int c=0;
	c++;
	cv::Point closestCv = cv::Point(closest.X, closest.Y);
//return;
	// mao esquerda esticada e afastada do corpo, comandos ativados.
	if (sp->leftHand.x!=0 && sp->leftHand.x < sp->center.x - afa*2 && sp->leftHand.y > sp->center.y + afa)
	{
		// Tronco
		// media dos dois ombros atual
		int y1 = (sp->rightShoulder.y + sp->leftShoulder.y)/2; 
		// ultima media dos dois ombros armazenada
		int y2 = (sp->pointsV[SkeletonPoints::RIGHT_SHOULDER][sp->vHead[SkeletonPoints::RIGHT_SHOULDER] % BUF_SIZE].y + 
			  sp->pointsV[SkeletonPoints::LEFT_SHOULDER][sp->vHead[SkeletonPoints::LEFT_SHOULDER] % BUF_SIZE].y)/2;
		//printf("%d::Recebendo comandos (%d - %d)=%d\n", c++, y1, y2, y1 - y2);
		tronco = NONE;
		if (y1 - y2 >= 18) {
			tronco = DOWN;
		}
		if (y1 - y2 <= -18) {
			tronco = UP;
		}
		
		
		if (tronco!=lastTronco && tronco!=NONE) {
			printf("%d::TRONCO::%s\n", c, tronco==UP? "UP" : "DOWN");
			if (tronco==UP)
				systemThread("rosrun play_motion move_joint torso_lift_joint  1 0.2");
			else
				systemThread("rosrun play_motion move_joint torso_lift_joint -1 0.2");
		}
		
		lastTronco = tronco;

//return;
		// so entra a cada 10c para nao poluir muito o terminal	
		//if (c%10==0)
		{
			float angShoulder, angElbow;
			// Angulo entre ombro e cotovelo
			if (sp->rightHand.x!=0 && sp->rightElbow.x!=0) {
				angShoulder = -atan2f(sp->rightElbow.y-sp->rightShoulder.y, sp->rightElbow.x-sp->rightShoulder.x)*180./CV_PI;
				angShoulder = (((int)angShoulder)/5)*5;
				setAngShoulder(angShoulder);
				//printf("ANG::COTOVELO::OMBRO::%.1f\n", angShoulder);
			}

			// Angulo entre antebraco e cotovelo
			if (sp->rightHand.x!=0 && sp->rightElbow.x!=0) {
				angElbow = -atan2f(sp->rightHand.y-sp->rightElbow.y, sp->rightHand.x-sp->rightElbow.x)*180./CV_PI;
				angElbow = (((int)angElbow)/5)*5;
				setAngElbow(angElbow);
				//printf("ANG::COTOVELO:: MAO ::%.1f\n\n", angElbow);
			}


			moveArmThread();
		}

	}

	// so entra a cada 10c para nao poluir muito o terminal	
	//if (c%10==0)
		// se a mao esquerda estiver mais a esquerda do que o ombro, e ambos estiverem acima da linha da cintura
		if (sp->leftHand.x!=0 && sp->leftElbow.x!=0 && sp->leftHand.y < sp->center.y-afa && sp->leftElbow.y < sp->center.y )
		{
			if (sp->leftHand.x - sp->leftElbow.x < -25)
				walkDirection = FRONT;
			else if (sp->leftHand.x - sp->leftElbow.x > 20)
				walkDirection = BACK;
			else
				walkDirection = NONE;
			//printf("walkDirection::%5s\n", walkDirection==FRONT ? "FRENTE" : walkDirection==BACK ? "TRAS" : "NONE");
				
			float d1 = Skeleton::euclideanDist(sp->rightHand, closestCv);
			float d2 = Skeleton::euclideanDist(sp->leftHand, closestCv);

			walkAngle = -1;		
			if (d1<70)
				walkAngle = LEFT;
			else if (d2<70)
				walkAngle = RIGHT;
			else
				walkAngle = NONE;
			
			//if (walkAngle != -1)
			//	walkAngleQ[walkAngleH++ % QUEUE_SIZE] = walkAngle;
			//walkAngle = getModeVector(walkAngleQ);
		
			//printf("dists::%6.1f::%6.1f::%9s\n", d1, d2, walkAngle==RIGHT ? "DIREITA" : walkAngle==LEFT ? "ESQUERDA" : "NONE");
			//printf("walkAngle::%9s\n\n", walkAngle==RIGHT ? "DIREITA" : walkAngle==LEFT ? "ESQUERDA" : "NONE");
			
			moveBase(walkDirection, walkAngle);

			// TODO tentar descobrir porque essa forma nao esta funcionando.
			//int profRight = getMeanValue(depthMat, sp->rightShoulder);
			//int profLeft  = getMeanValue(depthMat, sp->leftShoulder);
			//int profRight = getMeanValue(depthMat, sp->rightHand);
			//int profLeft  = getMeanValue(depthMat, sp->leftHand);
	
			/*
			int diff =  profRight - profLeft;
			walkAngle = -1;
			if (profRight>0 && profLeft>0) {
				if (diff > 150)
					walkAngle = RIGHT;
				else if (diff < -150)
					walkAngle = LEFT;
				else
					walkAngle = NONE;
				
				if (walkAngle != -1)
					walkAngleQ[walkAngleH++ % QUEUE_SIZE] = walkAngle;
				walkAngle = getModeVector(walkAngleQ);
			}
			if (walkAngle != -1) {
				printf("profundidade: %4d %4d %5d %8s %8s\n", profRight, profLeft, diff, diff > 150 ? "DIREITA" : diff<-150 ? "ESQUERDA" : "NONE",  walkAngle==RIGHT ? "DIREITA" : walkAngle==LEFT ? "ESQUERDA" : "NONE");
			}*/
		}
}

void Tiago::moveBase(int walkDirection, int walkAngle) {
	int r;
	float ang = 0;
	float dir = 0;
	
	char buff[2] = {(char)walkDirection, (char) walkAngle};
	int ret = socketC->enviar(buff, 2);
	if (r<0)
		printf("Erro enviando socket");
	return;
	
	// se for igual nao reenvia o comando.
	if (lastWalkDirection == walkDirection && lastWalkAngle==walkAngle)
		return;
	
	printf("walkDirection::%5s\n", walkDirection==FRONT ? "FRENTE" : walkDirection==BACK ? "TRAS" : "NONE");
	printf("walkAngle::%9s\n\n", walkAngle==RIGHT ? "DIREITA" : walkAngle==LEFT ? "ESQUERDA" : "NONE");

	systemThread("python scripts/parando.py");
	
	if (walkDirection==FRONT)
		systemThread("python scripts/up.py");
	else if (walkDirection==BACK)
		systemThread("python scripts/down.py");
	else if (walkAngle==RIGHT)
		systemThread("python scripts/right.py");
	else if (walkAngle==LEFT)
		systemThread("python scripts/left.py");

/*
	// mata o processo anterior
	systemThread("killall -9 rostopic");
			
	if (walkDirection==FRONT)
		dir =  1.0;
	if (walkDirection==BACK)
		dir = -1.0;
	if (walkAngle==RIGHT)
		ang = -0.5;
	if (walkAngle==LEFT)
		ang =  0.5;
	
	if (dir!=0 || ang!=0) {
		char * command = new char[300];
		sprintf(command, "rostopic pub /mobile_base_controller/cmd_vel geometry_msgs/Twist -r 3 -- \'[%.1f, 0.0, 0.0]\' \'[0.0, 0.0, %.1f]\'", dir, ang);
		//printf("moveBase::command::%s\n", (char*)command);
		systemThread(command);
	}*/
	
	lastWalkDirection = walkDirection;
	lastWalkAngle = walkAngle;
}



void Tiago::systemThread(const char * command) {
	int threadId = pthread_create( &thread2, NULL, &Tiago::systemThread2, (void *)command);
}

void * Tiago::systemThread2(void * command_) {
	char * command = (char*)command_;
	printf("systemThread2::command::%s\n", command);
	int r;
	r = system(command);
	//if (command)
	//	delete command;
}


/**
 * Mode algorithm just for a 3 valued vector.
 **/
int Tiago::getModeVector(int vector[]) {
	int histo[3]={0,0,0}; // NONE, RIGHT, LEFT

	for (int i=0 ; i<QUEUE_SIZE ; i++)
		histo[vector[i]]++;
		
	if (histo[0]>histo[1] && histo[0]>histo[2])
		return 0;
	else if (histo[1]>histo[0] && histo[1]>histo[2])
		return 1;
	else
		return 2;
}

/**
 * Mediana
 **/
int Tiago::getMedianaVector(int vector[]) {
	int m = 1;
	int q=0;

	SkeletonPoints::quick_sort(vector, 0, QUEUE_SIZE);

	for (int i=0 ; i<QUEUE_SIZE ; i++) {
		if (vector[i]!=0) {
			q++;
		}
	}
	if (q>0)
		return vector[q/2+QUEUE_SIZE-q];

	return m;
}



