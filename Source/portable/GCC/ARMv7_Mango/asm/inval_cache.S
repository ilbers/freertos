/*
 * Copyright (C) 2014-2015 ilbers GmbH
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
 *
 */
#include <asm/sys_ctrl.h>

	.globl	flush_caches

	.macro	flush_dcache level
	dsb
	isb

	/* Figure out the cache size information */
	mov	r0, \level
	sub	r0, r0, #1
	lsl	r0, r0, #1

	mcr	CSSELR(r0)		/* Cache size selection */
	mrc	CCSIDR(r0)		/* Cache size ID */

	lsr	r9, r0, #13
	movw	r7, 0x7fff
	and	r9, r9, r7
	lsr	r8, r0, #3
	movw	r7, 0x3ff
	and	r8, r8, r7
	mov	r0, #0
	mov	r1, #0
1:
	lsl	r2, r0, #30
	lsl	r3, r1, #6
	mov	r4, \level
	sub	r4, r4, #1
	lsl	r4, r4, #1
	orr	r5, r2, r3
	orr	r5, r5, r4
	mcr	DCCISW(r5)		/* Data cache clean and invalidate */
	add	r0, r0, #1
	cmp	r0, r8
	bne	1b
	mov	r0, #0
	add	r1, r1, #1
	cmp	r1, r9
	bne	1b
	dsb
.endm

flush_caches:
	push	{r0-r9, lr}

	mcr	ICIALLU(r0)		/* Flush instruction cache */

	flush_dcache #1			/* Flush L1 data cache */
	flush_dcache #2			/* Flush L2 data cache */

	pop	{r0-r9, lr}
	bx	lr
