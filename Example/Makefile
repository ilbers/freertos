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

RTOS_SOURCE  = ../Source
DEMO         = ../Demo/ARMv7_Mango_GCC
DEMO_INCLUDE = ../Demo/Common/include
PORT         = ../Source/portable/GCC/ARMv7_Mango

BUILD_DIR    = $(DEMO)/build

include $(DEMO)/Config.mk
include $(DEMO)/platforms/$(PLATFORM)/Config.mk

CFLAGS = -O2 -mno-unaligned-access -marm -g				\
         -I$(DEMO)/include						\
         -I$(RTOS_SOURCE)/include					\
         -I$(DEMO_INCLUDE)						\
         -I$(PORT)/include						\
         -I$(DEMO)/platforms/$(PLATFORM)/include

AFLAGS = -marm								\
         -I $(PORT)/include

# ------------------------------------------ #
#               CONFIG_NET_LWIP              #
# ------------------------------------------ #
ifneq (,$(findstring CONFIG_NET_LWIP, $(DCONFIG)))
  LWIP = $(DEMO)/lwip

  CFLAGS +=					\
           -I$(LWIP)/src/include		\
           -I$(LWIP)/src/include/ipv4		\
           -I$(LWIP)/ports/mango/include
endif

# Include demo scenario
include $(APPLICATION)/Makefile

APP_OBJS := $(APP_SRCS:.c=.o)
APP_OBJS := $(APP_OBJS:.S=.o)

.DEFAULT_GOAL := app.o
app.o: $(APP_OBJS)
	@-rm -rf $(BUILD_DIR)/app
	@-mkdir -p $(BUILD_DIR)/app
	@-mv $(BUILD_DIR)/*.o $(BUILD_DIR)/app
	@echo [LD] app/built_in.o
	@$(LD) $(LDFLAGS) -r -o $(BUILD_DIR)/app/built_in.o $(BUILD_DIR)/app/*.o

%.o: %.S
	@echo [AS] app/$@
	@$(CC) $(AFLAGS) $(AFLAGS_PLATFORM) -c $< -o $@
	@mv $@ $(BUILD_DIR)

%.o: %.c
	@echo [CC] app/$@
	@$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CFLAGS_PLATFORM) -c $< -o $@
	@mv $@ $(BUILD_DIR)
