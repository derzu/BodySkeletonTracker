include CommonDefs.mak

BIN_DIR = Bin

INC_DIRS = include

SRC_FILES = src/*.cpp

ifeq ("$(OSTYPE)","Darwin")
	CFLAGS += -DMACOS
	LDFLAGS += -framework -framework
else
	CFLAGS += -DUNIX -DGLX_GLXEXT_LEGACY
	USED_LIBS +=
endif

USED_LIBS += OpenNI2 MWClosestPoint

#if DEPTH camera like Orbbec Astra, or Asus Xtion add this flag DEPTH. If not comment the following line
CFLAGS += -DDEPTH

EXE_NAME = BodySkeletonTracker

#opencv
CFLAGS += $(shell pkg-config --cflags opencv) -g -std=c++11 -pthread 
LDFLAGS += $(shell pkg-config --libs --static opencv)

ifndef OPENNI2_INCLUDE
    $(error OPENNI2_INCLUDE is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
else ifndef OPENNI2_REDIST
    $(error OPENNI2_REDIST is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
endif

INC_DIRS += $(OPENNI2_INCLUDE)

include CommonCppMakefile

.PHONY: copy-redist
copy-redist:
	cp -R $(OPENNI2_REDIST)/* $(OUT_DIR)

$(OUTPUT_FILE): copy-redist

