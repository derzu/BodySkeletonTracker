#ifndef TIAGO_H
#define TIAGO_H

//arm_1_joint ombro    frente/tras
//arm_2_joint ombro    direita/esquerda 1.1 0 -1.2
//arm_3_joint braco    gira             0 (cotovelo vai para baixo) -1.5 (cotovelo vai para frente/tras) -3 (cotovelo vai para cima)
//arm_4_joint cotovelo direita/esquerda 2.1 0
//arm_5_joint mao/punho
//arm_6_joint mao/punho
//arm_7_joint mao/punho

#define ELBOW_90  1.7
#define ELBOW_MAX 2.1
#define SHOULDER_45  0.8
#define ORIGIN 0

#include <thread>
#include <mutex>
#include <pthread.h>

class Tiago {
	public:
		Tiago();
		static void * moveArm(void * t);
		void moveArmThread();
		bool isMoving();
		float getAngElbow();
		float getAngShoulder();
		void setMoving(bool m);
		void setAngElbow(float ang);
		void setAngShoulder(float ang);
		void mutexLock();
		void mutexUnlock();

	private:
		bool moving;
		float angShoulder, angElbow;

		std::thread * t;
		std::mutex mtx;
		pthread_t thread1;
};



#endif
