/*
 * Copyright (C) 2015 ilbers GmbH
 *
 * Alexander Smirnov <asmirnov@ilbers.de>
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

#ifndef __IO_H__
#define __IO_H__

#include <sys.h>
#include <stdint.h>

static inline void mem_write8(uint32_t addr, uint8_t data)
{
	volatile uint8_t *paddr = (uint8_t *)addr;

	DSB(st);

	*paddr = data;
}

static inline uint8_t mem_read8(uint32_t addr)
{
	volatile uint8_t *paddr = (uint8_t *)addr;

	DSB(st);

	return *paddr;
}

static inline void mem_write32(uint32_t addr, uint32_t data)
{
	volatile uint32_t *paddr = (uint32_t *)addr;

	DSB(st);

	*paddr = data;
}

static inline volatile uint32_t mem_read32(uint32_t addr)
{
	volatile uint32_t *paddr = (uint32_t *)addr;

	DSB(st);

	return *paddr;
}

/* Co-processor I/O operations */
#define R0_inline		%0
#define R1_inline		%1

#define __READ_CP_REG32(_r...)					\
({								\
	register uint32_t _val;					\
	asm volatile("mrc "#_r : "=r"(_val));			\
	_val;							\
})

#define __WRITE_CP_REG32(_v, _r...)				\
{								\
	register uint32_t _val = _v;				\
	asm volatile("mcr "#_r : : "r"(_val));			\
}

#define __READ_CP_REG64(_r...)					\
({								\
	register uint32_t _val1;				\
	register uint32_t _val2;				\
	uint64_t _val;						\
	asm volatile("mrrc "#_r : "=r"(_val1), "=r"(_val2));	\
	_val = (uint64_t)_val1 | ((uint64_t)_val2 << 32);	\
	(uint64_t)_val;						\
})

#define __WRITE_CP_REG64(_v, _r...)				\
{								\
	register uint32_t _val1 = (_v) & 0xffffffff;		\
	register uint32_t _val2 = (((_v) >> 32) & 0xffffffff);	\
	asm volatile("mcrr "#_r : : "r"(_val1), "r"(_val2));	\
}

#define READ_CP_REG32(_r)	__READ_CP_REG32(_r)
#define WRITE_CP_REG32(_v, _r)	__WRITE_CP_REG32(_v, _r)
#define READ_CP_REG64(_r)	__READ_CP_REG64(_r)
#define WRITE_CP_REG64(_v, _r)	__WRITE_CP_REG64(_v, _r)

#endif /* __IO_H__ */
