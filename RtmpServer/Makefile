#++++++++++++++++++++++++++++++++++++++++++#
#平台定义
ifeq ($(PLATFORM),)
PLATFORM=i386-linux
endif

#++++++++++++++++++++++++++++++++++++++++++#
#得到顶层目录【.rules目录的父目录】绝对路径
TOPDIR=$(shell sh -c '\
        dir=`pwd|sed s/[/]$$//`; \
        while test $$dir; do \
                if test -d $$dir/.rules; then \
                        echo $$dir; \
                        exit; \
                fi; \
                dir=`echo $$dir|sed s/[/][^/]*$$//`; \
        done \
')


# 用户设定

#++++++++++++++++++++++++++++++++++++++++++#
#EXECUTABLE 是最终目标文件名，自己设定
#eg : EXECUTABLE := mushroom.exe

#MAKE_STATIC_LIB := 1
MAKE_SHARED_LIB := 1
EXECUTABLE := libRtspServer.so

ifeq ($(RELEASE),1)
	BUILD_MODE_DIR := release.$(PLATFORM)
else
	BUILD_MODE_DIR := debug.$(PLATFORM)
endif


#lmxctds Added:
ifeq ($(PLATFORM),mipsel-linux)
SYS_INCLUDE_PATH := -I/home/idt/include
else
SYS_INCLUDE_PATH :=
endif


#++++++++++++++++++++++++++++++++++++++++++#
#INCLUDE_PATH 是用户创建include 文件的搜索附加路径
INCLUDE_PATH := \
				-I. \
				-I./include \
				-I../include \
				-I$(TOPDIR)/pubEx/librtsp/include \
				-I$(TOPDIR)/pubEx/librtsp/ \
				-I$(TOPDIR)/pub/mdpub/include \
				-I${TOPDIR}/pub/mdpub/libhttp/include \



#++++++++++++++++++++++++++++++++++++++++++#
#DEFINES 用户自定义宏
DEFINES := -DOS_INT64_TYPE -DOS_LINUX -DRTSPSERVER_EXPORTS


#++++++++++++++++++++++++++++++++++++++++++#

#{{{{连接库包含路径设定++++++++++++++++++++#
#LIBRARY_PATH是项目在此路径下搜索包含的动态库(.so)，如：-L./ -L./source
#SYS_LIBRARY_PATH为系统lib文件路径的环境变量
LIBRARY_PATH := $(SYS_LIBRARY_PATH)


#}}}}连接库包含设定++++++++++++++++++++++++#

#++++++++++++++++++++++++++++++++++++++++++#
#LIBS 是本目标程序需要连接的动态库(.so)列表（例如 alleg, stdcx, iostr 等等），
#带-l前缀，只写该动态库的link名，例如stdc++表示会link在LIBRARY_PATH路径下的 libstdc++.so
#eg : LIBS := alleg
LIBS := -lstdc++ -ldl -lpthread -lm -lnspr4

#USERLIBS本目标程序需要连接的静态库(.a)，
#不带-l前缀，且需要写出该静态库的全路径，例如 ../xxx/debug.$(PLATFORM)/xxx.a



#++++++++++++++++++++++++++++++++++++++++++#
# 默认SOURCE为makefile目录的../source下的所有c/c++源文件
SOURCE := $(wildcard ./*.c) \
		  $(wildcard ./*.cpp) \
		  $(wildcard ./source/*.cpp) \
		  $(wildcard ./source/*.c) \
		  
				
SOURCE += $(TOPDIR)/pub/mdpub/source/aio-socket-epoll.c
SOURCE += $(TOPDIR)/pub/mdpub/source/aio-tcp-transport.c
SOURCE += $(TOPDIR)/pub/mdpub/source/aio-udp-transport.c
SOURCE += $(TOPDIR)/pub/mdpub/source/base64.c
SOURCE += $(TOPDIR)/pub/mdpub/source/cookie.cpp
SOURCE += $(TOPDIR)/pub/mdpub/source/http-parser.c
SOURCE += $(TOPDIR)/pub/mdpub/source/http-upload.c
SOURCE += $(TOPDIR)/pub/mdpub/source/ntp-time.c
SOURCE += $(TOPDIR)/pub/mdpub/source/ip-route.c
SOURCE += $(TOPDIR)/pub/mdpub/source/sysreboot.c
SOURCE += $(TOPDIR)/pub/mdpub/source/sysntpconfig.c
SOURCE += $(TOPDIR)/pub/mdpub/source/sysnetconfig.c
SOURCE += $(TOPDIR)/pub/mdpub/source/systimeconfig.c
SOURCE += $(TOPDIR)/pub/mdpub/source/systimer.c
SOURCE += $(TOPDIR)/pub/mdpub/source/thread-pool.c
SOURCE += $(TOPDIR)/pub/mdpub/source/time64.c
SOURCE += $(TOPDIR)/pub/mdpub/source/url.c
SOURCE += $(TOPDIR)/pub/mdpub/source/urlcodec.c
SOURCE += $(TOPDIR)/pub/mdpub/source/tools.c
SOURCE += $(TOPDIR)/pub/mdpub/source/dlog.cpp
SOURCE += $(TOPDIR)/pub/mdpub/string/strlcpy.c
SOURCE += $(TOPDIR)/pub/mdpub/source/md5.c


_SOURCE := ./StdAfx.cpp

# 源文件最终集合 = [SOURCE集] - [_SOURCE集]
SOURCE := $(filter-out $(_SOURCE),$(SOURCE))


#++++++++++++++++++++++++++++++++++++++++++#
# C++预编译设置，C语言不支持预编译
#PRECOMPILE_HEADER := ./StdAfx.h

#++++++++++++++++++++++++++++++++++++++++++#

GCC_VER_GTE44 := $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \< 4.4 | bc )
ifeq ($(GCC_VER_GTE44),1)
CFLAGS += -march=i586
endif

-include local.$(PLATFORM).defs
include $(TOPDIR)/.rules/rules.$(PLATFORM)

GCC_VER_GTE44 := $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \>= 4.4 | bc )
ifeq ($(GCC_VER_GTE44),1)
CXXFLAGS += -std=c++0x
endif


#++++++++++++++++++++++++++++++++++++++++++#
POST_BUILD_RULE:

	mkdir -p $(TOPDIR)/BasicModuleLib/$(BUILD_MODE_DIR)/RtspServer
	cp -f ./$(BUILD_MODE_DIR)/*.so $(TOPDIR)/BasicModuleLib/$(BUILD_MODE_DIR)/RtspServer
