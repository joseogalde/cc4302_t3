
obj-m := monitor.o
monitor-objs := monitor-impl.o kmutex.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

#include $(KDIR)/.config

ccflags-y := -Wall

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

monitor-impl.o kmutex.o: kmutex.h

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
