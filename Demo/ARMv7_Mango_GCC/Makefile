# Copyright (C) 2015 ilbers GmbH
#
# Alexander Smirnov <asmirnov@ilbers.de>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License** as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
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

RTOS_SOURCE  = ../../Source
DEMO_COMMON  = ../Common/Minimal
DEMO_INCLUDE = ../Common/include
DEMO_SOURCE  = source
PORT         = $(RTOS_SOURCE)/portable/GCC/ARMv7_Mango
APPLICATION  = ../../Example

BUILD_DIR = ./build

CFLAGS = -O2 -Wall -Werror -mcpu=cortex-a7 -mno-unaligned-access -marm	\
         -mfpu=vfpv4 -g							\
         -I./include							\
         -I$(RTOS_SOURCE)/include					\
         -I $(DEMO_INCLUDE)						\
         -I $(PORT)/include

AFLAGS = -mcpu=cortex-a7 -mfpu=vfpv4 -marm				\
         -I $(PORT)/include

SRCS = $(PORT)/asm/head.S						\
       $(PORT)/asm/vector.S						\
       $(PORT)/asm/inval_cache.S					\
       $(PORT)/mm.c							\
       $(PORT)/start.c							\
       $(PORT)/interrupt.c						\
       $(PORT)/sys_print.c						\
       $(PORT)/traps.c							\
       $(PORT)/port.c							\
       $(PORT)/timer.c							\
       $(DEMO_SOURCE)/uart.c						\
       $(RTOS_SOURCE)/list.c						\
       $(RTOS_SOURCE)/queue.c						\
       $(RTOS_SOURCE)/tasks.c						\
       $(RTOS_SOURCE)/portable/MemMang/heap_4.c				\
									\
       $(APPLICATION)/main.c

OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

freertos: built_in.o
	@echo [LD] FreeRTOS image
	@$(LD) -EL -Map freertos.map -T freertos.lds $(BUILD_DIR)/built_in.o -o freertos.bin
	@echo [OBJCOPY] freertos.bin
	@$(OBJCOPY) -O binary -S freertos.bin freertos.Image

built_in.o: $(OBJS)
	@-rm -f $(BUILD_DIR)/built_in.o
	@echo [LD] $@
	@$(LD) $(LDFLAGS) -r -o $@ $(BUILD_DIR)/*.o
	@mv $@ $(BUILD_DIR)

%.o: %.S
	@echo [AS] $@
	@$(CC) $(AFLAGS) -c $< -o $@
	@mv $@ $(BUILD_DIR)

%.o: %.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) -c $< -o $@
	@mv $@ $(BUILD_DIR)

.PHONY: all
all: freertos

.PHONY: clean
clean:
	@echo [CLEAN] $(BUILD_DIR)/*.o freertos.bin freertos.Image freertos.map
	@-rm -f $(BUILD_DIR)/*.o
	@-rm -f freertos.bin freertos.Image freertos.map
