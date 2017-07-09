#ifndef TIAGO_H
#define TIAGO_H

//head_1_joint
//head_2_joint
//torso_lift_joint 
//gripper_right_finger_joint (stell)
//gripper_left_finger_joint  (stell)
//hand_index_joint (titanium) // indicador
//hand_mrl_joint   (titanium) // tres dedos
//hand_thumb_joint (titanium) // polegar
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

#include "SkeletonPoints.h"

#define QUEUE_SIZE 10

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

		void detectTiagoCommands(SkeletonPoints* sp, int afa, cv::Mat &frame);
		int  getMeanValue(cv::Mat &depthMat, cv::Point& p);
		int  getMedianaVector(int vector[]);
		
		// Directions Constants
		static const int NONE  = 0;
		static const int RIGHT = 1;
		static const int LEFT  = 2;
	private:
		bool moving;
		float angShoulder, angElbow;
		
		int walkDirection;
		int walkDirectionQ[QUEUE_SIZE]; // vector to smoth the directions
		unsigned char walkDirectionH; // head
		

		std::thread * t;
		std::mutex mtx;
		pthread_t thread1;
};



#endif
