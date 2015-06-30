/*
 * Copyright (C) 2015 ilbers GmbH
 *
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License** as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
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

#include <stdint.h>
#include <stdarg.h>
#include <mango.h>
#include <sys_print.h>

static char buff[PRINT_BUFF_SIZE];

static int format_print_message(char *buff, const char *fmt, va_list argp)
{
	int d;
	char c;
	unsigned long u;

	unsigned int base;
	unsigned int len = 0;
	unsigned short negative;

	char x2c[]="0123456789abcdef";
	char ascii[10]; /* Max 32-bit integer is: 4294967295 (0xffffffff), 10-digits */
	char *s;

	s = buff;

	while ((c = *fmt++) != 0)
	{
		unsigned int fill = 0;
		unsigned int count = 0;

		if (c == '%')
		{
			s = 0;
			negative = 0;

			c = *fmt++;

			if ((c >= '0') && (c <= '9'))
			{
				fill = c - '0';
				c = *fmt++;
			}

			switch(c)
			{
			case 'd':
				d = va_arg(argp, signed int);
				if (d < 0)
				{
					negative = 1;
					u = -d;
				}
				else
				{
					u = d;
				}

				base = 10;
				break;

			case 'l':
				u = va_arg(argp, signed long);
				if (u < 0)
					negative = 1;

				base = 10;
				break;

			case 'u':
				u = va_arg(argp, unsigned long);

				base = 10;
				break;

			case 'x':
				u = va_arg(argp, unsigned long);

				base = 0x10;
				break;

			case 'c':
				s = "?";
				s[0] = va_arg(argp, int);
				break;

			case 's':
				s = va_arg(argp, char *);
				if (s == 0)
					s = "null";

				break;

			case '%':
				s = "%";
				break;

			default:
				s = "%?";
				s[1] = c;

				break;
			}

			if (s == 0)
			{
				s = ascii + sizeof(ascii);
				*s = 0;

				do
				{
					*--s = x2c[u % base];
					u /= base;
					count++;
				} while (u > 0);

				while (count < fill)
				{
					*--s = '0';
					count++;
				}
			}

			if (negative)
			{
				*--s = '-';
			}

			while (*s != 0)
			{
				buff[len++] = *s++;
			}
		}
		else
		{
			if (c == '/')
			{
				c = *fmt++;
				switch(c)
				{
				case 'n':
					c = 0x0a;
					break;

				case 'r':
					c = 0x0d;
					break;

				default:
					buff[len++] = '/';
					break;
				}
			}

			buff[len++] = c;
		}
	}

	return len;
}

uint32_t sys_print_msg(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args,fmt);
	len = format_print_message(buff, fmt, args);
	va_end(args);

	buff[len] = 0;

	uart_tx(buff, len);

	return len;
}

uint32_t mango_print_msg(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args,fmt);
	len = format_print_message(buff, fmt, args);
	va_end(args);

	buff[len] = 0;

	mango_console_write((uint8_t *)buff, len);

	return len;
}

char sys_get_char()
{
	char c;

	uart_rx(&c, 1);

	return c;
}
