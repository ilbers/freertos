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

build:
	@if [ ! -d Demo/ARMv7_Mango_GCC/build ]; then	\
		mkdir Demo/ARMv7_Mango_GCC/build;	\
	fi
	make -C Demo/ARMv7_Mango_GCC

install:
	cp Demo/ARMv7_Mango_GCC/freertos.Image $(INSTALL_DIR)

clean:
	make -C Demo/ARMv7_Mango_GCC clean
