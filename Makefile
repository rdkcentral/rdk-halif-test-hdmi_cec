#* ******************************************************************************
#* Copyright (C) 2022 Sky group of companies, All Rights Reserved
#* * --------------------------------------------------------------------------
#* * THIS SOFTWARE CONTRIBUTION IS PROVIDED ON BEHALF OF SKY PLC. 
#* * BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
#* * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#* * A PARTICULAR PURPOSE ARE DISCLAIMED
#* ******************************************************************************
#*
#*   ** Project      : ut
#*   ** @addtogroup  : ut
#*   ** @file        : makefile
#*   ** @author      : gerald.weatherup@sky.uk
#*   ** @date        : 20/05/2022
#*   **
#*   ** @brief : Makefile for ut
#*   ** 
#*
#* ******************************************************************************
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BIN_DIR := $(ROOT_DIR)/bin
TOP_DIR := $(ROOT_DIR)

SRC_DIRS = $(ROOT_DIR)/src
INC_DIRS := $(ROOT_DIR)/../include
HAL_LIB := RCECHal
SKELTON_SRCS := $(ROOT_DIR)/skeletons/src/hdmi_cec_driver.c

ifeq ($(TARGET),)
$(info TARGET NOT SET )
$(info TARGET FORCED TO Linux)
TARGET=linux
SRC_DIRS += $(ROOT_DIR)/skeletons/src
endif

$(info TARGET [$(TARGET)])

ifeq ($(TARGET),arm)
HAL_LIB_DIR := $(ROOT_DIR)/libs
YLDFLAGS = -Wl,-rpath,$(HAL_LIB_DIR) -L$(HAL_LIB_DIR) -l$(HAL_LIB)  -lpthread -lrt
ifeq ("$(wildcard $(HAL_LIB_DIR)/lib$(HAL_LIB).so)","")
SETUP_SKELETON_LIBS := skeleton
endif
endif

.PHONY: clean list all

export YLDFLAGS
export BIN_DIR
export SRC_DIRS
export INC_DIRS
export TARGET
export TOP_DIR
export HAL_LIB_DIR

.PHONY: clean list build skeleton


build: $(SETUP_SKELETON_LIBS)
	echo "SETUP_SKELETON_LIBS $(SETUP_SKELETON_LIBS)"
	@echo UT [$@]
	make -C ./ut-core

#Build against the real library leads to the SOC library dependency also.SOC lib dependency cannot be specified in the ut Makefile, since it is supposed to be common across may platforms. So in order to over come this situation, creating a template skelton library with empty templates so that the template library wont have any other Soc dependency. And in the real platform mount copy bind with the actual library will work fine.
skeleton:
	echo $(CC)
	$(CC) -fPIC -shared -I$(ROOT_DIR)/../include $(SKELTON_SRCS) -o lib$(HAL_LIB).so
	mkdir -p $(HAL_LIB_DIR)
	cp $(ROOT_DIR)/lib$(HAL_LIB).so $(HAL_LIB_DIR)


list:
	@echo UT [$@]
	make -C ./ut-core list

clean:
	@echo UT [$@]
	make -C ./ut-core clean
