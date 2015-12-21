# Copyright (C) 2015 ilbers GmbH
#
# Alexander Smirnov <asmirnov@ilbers.de>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License (version 2) as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA

AS         = $(CROSS_COMPILE)as
CC         = $(CROSS_COMPILE)gcc
LD         = $(CROSS_COMPILE)ld
STRIP      = $(CROSS_COMPILE)strip
OBJCOPY    = $(CROSS_COMPILE)objcopy
OBJDUMP    = $(CROSS_COMPILE)objdump

include Config.mk
include platforms/$(PLATFORM)/Config.mk

RTOS_SOURCE     = ../../Source
DEMO_COMMON     = ../Common/Minimal
DEMO_INCLUDE    = ../Common/include
DEMO_SOURCE     = source
PORT            = $(RTOS_SOURCE)/portable/GCC/ARMv7_Mango
APPLICATION     = ../../Example
LWIP            = lwip
PLATFORM_SOURCE = platforms/$(PLATFORM)/source

BUILD_DIR = ./build

CFLAGS = -O2 -mno-unaligned-access -marm				\
         -mfpu=vfpv4 -g							\
         -I./include							\
         -I$(RTOS_SOURCE)/include					\
         -I$(DEMO_INCLUDE)						\
         -I$(PORT)/include						\
         -I./platforms/$(PLATFORM)/include

AFLAGS = -mfpu=vfpv4 -marm						\
         -I $(PORT)/include

OS_COMPONENTS := core.o app.o

#########################################################################
#                      Core FreeRTOS objects                            #
#########################################################################
CORE_SRCS = $(PORT)/asm/head.S						\
            $(PORT)/asm/vector.S					\
            $(PORT)/asm/inval_cache.S					\
            $(PORT)/mm.c						\
            $(PORT)/start.c						\
            $(PORT)/interrupt.c						\
            $(PORT)/sys_print.c						\
            $(PORT)/traps.c						\
            $(PORT)/port.c						\
            $(PORT)/timer.c						\
            $(DEMO_SOURCE)/uart.c					\
            $(DEMO_SOURCE)/mango_core.c					\
            $(RTOS_SOURCE)/list.c					\
            $(RTOS_SOURCE)/queue.c					\
            $(RTOS_SOURCE)/tasks.c					\
            $(RTOS_SOURCE)/portable/MemMang/heap_4.c			\
            $(PLATFORM_SOURCE)/uart_drv.c

CORE_OBJS := $(CORE_SRCS:.c=.o)
CORE_OBJS := $(CORE_OBJS:.S=.o)

core.o: $(CORE_OBJS)
	@-rm -rf $(BUILD_DIR)/core
	@-mkdir -p $(BUILD_DIR)/core
	@-mv $(BUILD_DIR)/*.o $(BUILD_DIR)/core
	@echo [LD] core/FreeRTOS.o
	@$(LD) $(LDFLAGS) -r -o $(BUILD_DIR)/core/FreeRTOS.o $(BUILD_DIR)/core/*.o

#########################################################################
#                         Application objects                           #
#########################################################################
app.o:
	@make -C $(APPLICATION)

# ------------------------------------------ #
#               CONFIG_NET_LWIP              #
# ------------------------------------------ #
ifneq (,$(findstring CONFIG_NET_LWIP, $(DCONFIG)))

CFLAGS +=				\
         -I$(LWIP)/src/include		\
         -I$(LWIP)/src/include/ipv4	\
         -I$(LWIP)/ports/mango/include

#########################################################################
#                            lwIP objects                              #
#########################################################################
LWIP_SRCS = $(LWIP)/src/core/init.c					\
            $(LWIP)/src/core/def.c					\
            $(LWIP)/src/core/dhcp.c					\
            $(LWIP)/src/core/mem.c					\
            $(LWIP)/src/core/memp.c					\
            $(LWIP)/src/core/netif.c					\
            $(LWIP)/src/core/pbuf.c					\
            $(LWIP)/src/core/stats.c					\
            $(LWIP)/src/core/sys.c					\
            $(LWIP)/src/core/udp.c					\
            $(LWIP)/src/core/tcp.c					\
            $(LWIP)/src/core/tcp_in.c					\
            $(LWIP)/src/core/tcp_out.c					\
            $(LWIP)/src/core/ipv4/icmp.c				\
            $(LWIP)/src/core/ipv4/ip.c					\
            $(LWIP)/src/core/ipv4/ip_addr.c				\
            $(LWIP)/src/core/ipv4/inet_chksum.c				\
            $(LWIP)/src/core/ipv4/ip_frag.c				\
            $(LWIP)/src/core/lwip_timers.c				\
            $(LWIP)/src/api/sockets.c					\
            $(LWIP)/src/api/tcpip.c					\
            $(LWIP)/src/api/api_lib.c					\
            $(LWIP)/src/api/api_msg.c					\
            $(LWIP)/src/api/netbuf.c					\
            $(LWIP)/src/netif/etharp.c					\
            $(LWIP)/ports/mango/netif/adapter.c				\
            $(LWIP)/ports/mango/netif/pqueue.c				\
            $(LWIP)/ports/mango/sys_arch.c

LWIP_OBJS := $(LWIP_SRCS:.c=.o)
LWIP_OBJS := $(LWIP_OBJS:.S=.o)

OS_COMPONENTS += lwip.o

lwip.o: $(LWIP_OBJS)
	@-rm -rf $(BUILD_DIR)/lwip
	@-mkdir -p $(BUILD_DIR)/lwip
	@-mv $(BUILD_DIR)/*.o $(BUILD_DIR)/lwip
	@echo [LD] lwip/built_in.o
	@$(LD) $(LDFLAGS) -r -o $(BUILD_DIR)/lwip/built_in.o $(BUILD_DIR)/lwip/*.o

#########################################################################
endif

%.o: %.S
	@echo [AS] $@
	@$(CC) $(AFLAGS) $(AFLAGS_PLATFORM) -c $< -o $@
	@mv $@ $(BUILD_DIR)

%.o: %.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CFLAGS_PLATFORM) -c $< -o $@
	@mv $@ $(BUILD_DIR)

freertos: $(OS_COMPONENTS)
	@echo [LD] FreeRTOS image
	@$(LD) -EL -Map freertos.map -T freertos.lds -o freertos.bin	\
		$(BUILD_DIR)/core/FreeRTOS.o				\
		`find $(BUILD_DIR) -name 'built_in.o'`
	@echo [OBJCOPY] freertos.bin
	@$(OBJCOPY) -O binary -S freertos.bin freertos.Image

.DEFAULT_GOAL := all
all: freertos

.PHONY: clean
clean:
	@echo [CLEAN] $(BUILD_DIR)/*.o freertos.bin freertos.Image freertos.map
	@-rm -rf $(BUILD_DIR)/*
	@-rm -f freertos.bin freertos.Image freertos.map
