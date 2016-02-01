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

#include <asm/mmu.h>
#include <asm/sys_ctrl.h>

#define __HIGH(val)		((val) & 0xf00)
#define __LOW(val)		((val) & 0x0ff)

#define FREERTOS_ENTRY_ADDR	0x40000000

	.arm
	.global	start
start:
	.rept	8
	mov	r0, r0
	.endr

	cpsid	aif

	ldr	r0, =start			/* get relative boot address */
	adr	r1, start			/* get physical boot address */
	sub	r10, r1, r0			/* r10 - physical offset */

	mov	r0, #0x00
	mcr	BPIALL(r0)			/* BPIALL: invalidate entire branch prediction array */

	mov	r0, #0x00
	mcr	TLBIALL(r0)			/* TLBIALL: invalidate entire TLB */

	bl	flush_bss

	ldr	r0, =l1_page_table		/* Boot CPU L1 page table */
	add	r0, r0, r10			/* Physical address of page table */

	mov	r1, #0
	mcrr	TTBR0_64(r0, r1)		/* Set location of PT */

	mov	r2, #L1_ENTRIES			/* Clean L1 Page Table */
	bl	ttb_flush

	/* VA = 0x00000000 : direct mapping of the first GB */
	mov	r2, #0x00000000			/* Get physical memory start */
	orr	r2, r2, #__LOW(ENTRY_MEM_BLOCK(M_ATTR_DEVICE))
	orr	r2, r2, #__HIGH(ENTRY_MEM_BLOCK(M_ATTR_DEVICE))
	mov	r3, #0
	strd	r2, r3, [r0]			/* L1 TTB entry 0 */

	/* L1: map L2 page table */
	ldr	r2, =l2_page_table		/* Boot CPU L2 page tables */
	add	r2, r2, #((FREERTOS_ENTRY_ADDR >> 30) * 4096)
						/* L2 index offset */
	add	r2, r2, r10			/* Physical address of page table */
	lsr	r1, r2, #30
	orr	r2, r2, #__LOW(ENTRY_PAGE_TABLE)
	orr	r2, r2, #__HIGH(ENTRY_PAGE_TABLE)
	lsl	r1, r1, #3
	mov	r3, #0
	strd	r2, r3, [r0, r1]		/* L1 TTB entry 2: L2 page table */

	ldr	r0, =l2_page_table		/* Boot CPU L2 page table */
	add	r0, r0, r10			/* Physical address of page table */

	mov	r2, #(L1_ENTRIES * L2_ENTRIES)	/* Clean L2 Page Tables */
	bl	ttb_flush

	add	r0, r0, #((FREERTOS_ENTRY_ADDR >> 30) * 4096)
						/* L2 index offset */

	/* L2: map text phys_addr(start) -> virt_addr(start) */
	adr	r2, start			/* Get physical address of boot code */
	lsr	r1, r2, #21
	and	r1, r1, #0x0f			/* Get L2 table entry index */
	lsl	r1, r1, #3
	lsr	r2, r2, #21
	lsl	r2, r2, #21
	orr	r2, r2, #__LOW(ENTRY_MEM_BLOCK(M_ATTR_NCACHE))
	orr	r2, r2, #__HIGH(ENTRY_MEM_BLOCK(M_ATTR_NCACHE))
	mov	r3, #0
	strd	r2, r3, [r0, r1]		/* L2 TTB entry 0: virt_addr(start) */

	dsb

	mrc	TTBCR(r4)			/* Read current TTBCR */
	lsr	r4, r4, #3			/* Zero TCZ */
	lsl	r4, r4, #3
	orr	r4, r4, #(1 << 31)		/* Enable EAE */
	mcr	TTBCR(r4)			/* Store current TTBCR */

	mrc	SCTLR(r0)			/* Read current Control Register */
	orr	r0, r0, #0x01			/* Enable MMU */
	orr	r0, r0, #(1 << 13)		/* Use high exception vector */
	mcr	SCTLR(r0)			/* Store current Control Register */

	dsb					/* Flush the ARM pipeline after MMU enabled */
	isb

	ldr	r0, =boot_stack
	add	r0, r0, r10
	mov	sp, r0
	add	sp, sp, #4096

	ldr	r0, =irq_stack
	add	r0, r0, r10
	add	r0, r0, #4096
	msr	sp_irq, r0

	ldr	r0, =abort_stack
	add	r0, r0, r10
	add	r0, r0, #4096
	msr	sp_abt, r0

	/* Enable FPU */
	mrc	CPACR(r0)
	orr	r0, r0, #(3 << 20)		/* Full access for cp10 */
	orr	r0, r0, #(3 << 22)		/* Full access for cp11 */
	bic	r0, r0, #(3 << 30)		
	mcr	CPACR(r0)
	isb
	mov	r0, #(1 << 30)
	vmsr	fpexc, r0

	b	start_kernel

ttb_flush:
	lsr	r2, r2, #1
	mov	r1, r0				/* NOTE: this should be done by cleaning .bss */
	mov	r3, #0
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
1:
	stmia	r1!, {r3-r6}			/* Flush four table entries */
	subs	r2, r2, #1
	bne	1b

	mov	pc, lr
