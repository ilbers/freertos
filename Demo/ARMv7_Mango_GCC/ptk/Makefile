# Copyright (C) 2015-2016 ilbers GmbH
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

include ../Config.mk
include ../platforms/$(PLATFORM)/Config.mk

RTOS_SOURCE     = ../../../Source
DEMO_INCLUDE    = ../../Common/include
PORT            = $(RTOS_SOURCE)/portable/GCC/ARMv7_Mango
LWIP            = ./

BUILD_DIR = ../build

CFLAGS = -O2 -mno-unaligned-access -marm	\
         -mfpu=vfpv4 -g				\
         -I../include				\
         -I$(RTOS_SOURCE)/include		\
         -I$(DEMO_INCLUDE)			\
         -I$(PORT)/include			\
         -I../platforms/$(PLATFORM)/include	\
         -I$(LWIP)/src/include			\
         -I$(LWIP)/src/include/ipv4		\
         -I$(LWIP)/ports/mango/include		\
         $(PTK_CFLAGS)

FONT_SRCS = fonts/10x20.c	\
            fonts/5x7.c		\
            fonts/6x12.c	\
            fonts/8x13.c	\
            fonts/helv_m20.c	\
            fonts/helv_m25.c	\
            fonts/helv_m34.c	\
            fonts/helv_b34.c	\
            fonts/school_m34.c

PTK_SRCS = source/PTK_generic.c     \
           source/PTK_driver_8bpp.c \
           source/PTK_widget.c \

PTK_OBJS := $(PTK_SRCS:.c=.o)
PTK_OBJS += $(FONT_SRCS:.c=.o)

ptk.o: $(PTK_OBJS)
	@-rm -rf $(BUILD_DIR)/ptk
	@-mkdir -p $(BUILD_DIR)/ptk
	@-mv $(BUILD_DIR)/*.o $(BUILD_DIR)/ptk
	@echo [LD] ptk/built_in.o
	@$(LD) $(LDFLAGS) -r -o $(BUILD_DIR)/ptk/built_in.o $(BUILD_DIR)/ptk/*.o

%.o: %.c
	@echo [CC] ptk/$@
	@$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CFLAGS_PLATFORM) -c $< -o $@
	@mv $@ $(BUILD_DIR)

.PHONY: all
all: ptk.o
