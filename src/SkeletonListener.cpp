#include <SkeletonListener.h>

SkeletonListener::SkeletonListener() {
}

SkeletonListener::~SkeletonListener() {
}

void SkeletonListener::onEvent(SkeletonPoints * sp) {
	printf("Recebi o esqueleto\n");
}
