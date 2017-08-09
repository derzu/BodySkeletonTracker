# BodySkeletonTracker
# Description
Human body skeleton detection and tracking from video camera in real time. It was developed for an Orbbec Astra camera DRGB (Depth-RGB), uses the OpenNI2 driver, it should also works with Asus Xtion and Prime Sense. It works with normal webcam, in the case of background is smooth.

Currently it detects just the joints of head, shoulders, elbows and hands. A new version can be developed to also detect also feets and knees.

# Descrição
Detecção e rastreamento de um esqueleto humano a partir de câmera de vídeo em tempo real. Foi desenvolvido para a câmera DRGB (Depth-RGB) Orbbec Astra, utilizando o driver OpenNI2, funciona também com o Asus Xtion e Prime Sense. Funciona também com webcam normal, caso o background seja liso e branco.

# Video Exemple

[![IMAGE ALT TEXT HERE](https://github.com/derzu/BodySkeletonTracker/blob/master/thumbs.png)](https://www.youtube.com/watch?v=9XYmkTN2RQY)

# Requirements
 - OpenCV library (just for line/circle drawing and window frame showing)
 - OpenNI2 library  
 - It was developed and tested on Ubuntu 14.04. Should work at any Linux, maybe Mac and Windows too.  

# Compile and run
 $ source ~/Downloads/OpenNI-Linux-x64-2.3/OpenNIDevEnvironment     (set your path appropriately)  
 $ cd BodySkeletonTracker  
 $ make  
 $ cd Bin/x64-Release/  
 $ ./BodySkeletonTracker  
