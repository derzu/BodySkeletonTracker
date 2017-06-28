#include <Tiago.h>
#include <stdio.h>
#include <stdlib.h>

Tiago::Tiago() {
	moving = false;
	t = NULL;
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























