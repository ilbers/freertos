/*
 * Copyright (C) 2015-2016 ilbers GmbH
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * Based on picoTK
 * Copyright (C) Thomas Gallenkamp <tgkamp@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define ESC_BLUE	"\e[31m"
#define ESC_GREEN	"\e[32m"
#define ESC_CYAN	"\e[33m"
#define ESC_RED		"\e[34m"
#define ESC_MAGENTA	"\e[35m"
#define ESC_YELLOW	"\e[36m"
#define ESC_WHITE	"\e[37m"
#define ESC_BRIGHT	"\e[38m"
#define ESC_BLACK	"\e[30m"
#define ESC_BG_WHITE	"\e[47m"
#define ESC_BG_BLACK	"\e[40m"
#define ESC_UP		"\e[A"
#define ESC_DOWN	"\e[B"
#define ESC_RIGHT	"\e[C"
#define ESC_LEFT	"\e[D"

char termtxt[]=
	ESC_BLUE
	"  LotOS Virtualization\r\n"
	"       Framework\r\n\n"
	ESC_WHITE
	ESC_BRIGHT
	"LotOS "
	ESC_WHITE
	"is a framework\r\n"
	"integrating the most\r\n"
	"popular open-source\r\n"
	"operating systems with\r\n"
	ESC_BRIGHT
	"Mango Hypervisor"
	ESC_WHITE
	". Based\r\n"
	"on Yocto, this\r\n"
	"framework provides a\r\n"
	"way to build a complete\r\n"
	"flash image with Linux\r\n"
	"and FreeRTOS running in\r\n"
	ESC_BRIGHT
	"Mango"
	ESC_WHITE
	" virtual machines.\r\n\n"
	"With "
	ESC_BRIGHT
	"LotOS"
	ESC_WHITE
	", you\r\n"
	"concentrate on your\r\n"
	"business-critical\r\n"
	"software; the\r\n"
	"pre-packaged\r\n"
	"infrastructure just\r\n"
	"works. Of course,\r\n"
	"the framework is\r\n"
	"customizable and\r\n"
	"extendable to your\r\n"
	"needs.\r\n\n"
	ESC_BRIGHT
	"Main features:\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"Full resource\r\n"
	"isolation for guest\r\n"
	"operating systems\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"Bounded latency\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"Fast partition\r\n"
	"start\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"Cross-partition\r\n"
	"communication data\r\n"
	"channel and TCP/IP\r\n"
	"networking\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"Software watchdog\r\n"
	ESC_BLUE
	" * "
	ESC_WHITE
	"No costly operating\r\n"
	"system modifications\r\n\n"
	"Please contact us for\r\n"
	"information\r\n\n"
	ESC_BRIGHT
	"ilbers GmbH\r\n"
	"<lotos@ilbers.de>\r\n\n";
