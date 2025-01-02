#* ******************************************************************************
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2023 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************
#*
#*   ** Project      : ut
#*   ** @addtogroup  : ut
#*   ** @file        : makefile
#*   ** @date        : 20/05/2022
#*   **
#*   ** @brief       : Makefile for UT
#*   **
#*
#* ******************************************************************************
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BIN_DIR := $(ROOT_DIR)/bin
TOP_DIR := $(ROOT_DIR)

SRC_DIRS = $(ROOT_DIR)/src
INC_DIRS := $(ROOT_DIR)/../include
ASSETS_DIR := $(ROOT_DIR)/assets
CEC_RESP_DIR := $(ASSETS_DIR)/cec_responses
HAL_LIB := RCECHal
SKELTON_SRCS := $(ROOT_DIR)/skeletons/src/hdmi_cec_driver.c
VCOMPONENT_SRCS := $(wildcard $(ROOT_DIR)/vcomponent/src/*.c)
VCOMPONENT_OBJS := $(subst src,build,$(VCOMPONENT_SRCS:.c=.o))

ifeq ($(TARGET),)
$(info TARGET NOT SET )
$(info TARGET FORCED TO Linux)
TARGET=linux
SRC_DIRS += $(ROOT_DIR)/skeletons/src
YLDFLAGS += -lpthread -lrt
endif

$(info TARGET [$(TARGET)])

ifeq ($(TARGET),arm)
HAL_LIB_DIR := $(ROOT_DIR)/libs
YLDFLAGS = -Wl,-rpath,$(HAL_LIB_DIR) -L$(HAL_LIB_DIR) -l$(HAL_LIB)  -lpthread -lrt
ifeq ("$(wildcard $(HAL_LIB_DIR)/lib$(HAL_LIB).so)","")
SETUP_SKELETON_LIBS := skeleton
endif
endif

ifeq ($(TARGET),vcomponent)
HAL_LIB_DIR := $(ROOT_DIR)/libs
override TARGET := linux
#YLDFLAGS = -Wl,-rpath,$(HAL_LIB_DIR) -L$(HAL_LIB_DIR) -l$(HAL_LIB)  -lpthread -lrt
SRC_DIRS += $(ROOT_DIR)/vcomponent/src
INC_DIRS += $(ROOT_DIR)/vcomponent/include $(ROOT_DIR)/ut-core/include $(ROOT_DIR)/ut-core/framework/ut-control/include
KCFLAGS = -DVCOMPONENT
export KCFLAGS
endif

.PHONY: clean list all

export YLDFLAGS
export BIN_DIR
export SRC_DIRS
export INC_DIRS
export TARGET
export TOP_DIR
export HAL_LIB_DIR

.PHONY: clean list build skeleton vcomponent

build: $(SETUP_SKELETON_LIBS)
	echo "SETUP_SKELETON_LIBS $(SETUP_SKELETON_LIBS)"
	@echo UT [$@]
	make -C ./ut-core TARGET=${TARGET}
	rm -rf $(BIN_DIR)/lib$(HAL_LIB).so
	rm -rf $(ROOT_DIR)/libs/lib$(HAL_LIB).so
	cp -r $(CEC_RESP_DIR)/* $(BIN_DIR)/

#Build against the real library leads to the SOC library dependency also.SOC lib dependency cannot be specified in the ut Makefile, since it is supposed to be common across may platforms. So in order to over come this situation, creating a template skelton library with empty templates so that the template library wont have any other Soc dependency. And in the real platform mount copy bind with the actual library will work fine.
skeleton:
	echo $(CC)
	$(CC) -fPIC -shared -I$(ROOT_DIR)/../include $(SKELTON_SRCS) -o lib$(HAL_LIB).so
	mkdir -p $(HAL_LIB_DIR)
	cp $(ROOT_DIR)/lib$(HAL_LIB).so $(HAL_LIB_DIR)

vcomponent:
	echo $(CC)
	$(CC) -fPIC -shared -I$(ROOT_DIR)/../include -I$(ROOT_DIR)/vcomponent/include -I$(ROOT_DIR)/ut-core/include -I$(ROOT_DIR)/ut-core/framework/ut-control/include $(VCOMPONENT_OBJS) -o lib$(HAL_LIB).so
	mkdir -p $(HAL_LIB_DIR)
	cp $(ROOT_DIR)/lib$(HAL_LIB).so $(HAL_LIB_DIR)

list:
	@echo UT [$@]
	make -C ./ut-core list

clean:
	@echo UT [$@]
	make -C ./ut-core cleanall
