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

#########################################################################
#                         Application objects                           #
#########################################################################
APP_SRCS = $(APPLICATION)/main.c	\
           $(APPLICATION)/message.c	\
           $(APPLICATION)/dc.c

# ------------------------------------------ #
#               CONFIG_NET_LWIP              #
# ------------------------------------------ #
ifneq (,$(findstring CONFIG_NET_LWIP, $(DCONFIG)))
  APP_SRCS += $(APPLICATION)/network.c
endif

# ------------------------------------------ #
#                CONFIG_FB_PTK               #
# ------------------------------------------ #
ifneq (,$(findstring CONFIG_FB_PTK, $(DCONFIG)))
  APP_SRCS += $(APPLICATION)/fb.c
  APP_SRCS += $(APPLICATION)/math.c
endif
