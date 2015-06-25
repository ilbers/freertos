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

#ifndef __ASM_SYS_CTRL_H__
#define __ASM_SYS_CTRL_H__

/* CPSR register bits */
#define CPSR_MODE_MASK		0x0000001f		/* CPU mode bits */
#define CPSR_IT_MASK		0x0600fc00		/* ITSTATE bits */

#define CPSR_MODE_IRQ		0x00000012		/* CPU mode: IRQ */
#define CPSR_MODE_SVC		0x00000013		/* CPU mode: SVC */
#define CPSR_MODE_ABORT		0x00000017		/* CPU mode: Abort */

#define CPSR_JAZELLE		(1 << 24)		/* Jazelle mode */
#define CPSR_ENDIAN		(1 << 9)		/* Big/Little endian */
#define CPSR_ABORT		(1 << 8)		/* Mask aborts */
#define CPSR_IRQ		(1 << 7)		/* Mask IRQs */
#define CPSR_THUMB		(1 << 5)		/* THUMB mode */

#define MPIDR(_r)		p15, 0, _r, c0 , c0, 5	/* Multiprocessor ID */
#define SCTLR(_r)		p15, 0, _r, c1 , c0, 0	/* System Control */
#define ACTLR(_r)		p15, 0, _r, c1 , c0, 1	/* AUX Control */
#define CPACR(_r)		p15, 0, _r, c1 , c0, 2	/* Co-Processor Access Control */
#define TTBR0(_r)		p15, 0, _r, c2 , c0, 0	/* Translation Table Base */
#define TTBR1(_r)		p15, 0, _r, c2 , c0, 1	/* Translation Table Base */
#define TTBCR(_r)		p15, 0, _r, c2 , c0, 2	/* Translation Table Base Control */
#define DACR(_r)		p15, 0, _r, c3 , c0, 0	/* Domain Access Control */
#define DFSR(_r)		p15, 0, _r, c5 , c0, 0	/* Data Fault Status */
#define IFSR(_r)		p15, 0, _r, c5 , c0, 1	/* Instruction Fault Status */
#define DFAR(_r)		p15, 0, _r, c6 , c0, 0	/* Data Fault Address */
#define IFAR(_r)		p15, 0, _r, c6 , c0, 2	/* Intruction Fault Address */
#define ICIALLU(_r) 	 	p15, 0, _r, c7 , c5, 0	/* Instruction Cache Invalidate All to PoU */
#define BPIALL(_r) 	 	p15, 0, _r, c7 , c5, 6	/* invalidate entire branch prediction array */
#define ATS1CPR(_r)		p15, 0, _r, c7 , c8, 0	/* Address Translation Stage-1 */
#define ATS12NSOPR(_r)		p15, 0, _r, c7 , c8, 4	/* PL1 Stages 1+2 Translation */
#define DCCISW(_r)		p15, 0, _r, c7, c14, 2	/* Data Cache Clean and Invalidate by Set/Way */
#define TLBIALLIS(_r)		p15, 0, _r, c8 , c3, 0	/* invalidate inner-shareable */
#define TLBIALL(_r)		p15, 0, _r, c8 , c7, 0	/* invalidate entire TLB */
#define MAIR0(_r)		p15, 0, _r, c10, c2, 0	/* memory attribute indirection */
#define MAIR1(_r)		p15, 0, _r, c10, c2, 1	/* memory attribute indirection */
#define AMAIR0(_r)		p15, 0, _r, c10, c3, 0	/* AUX memory attribute indirection */
#define AMAIR1(_r)		p15, 0, _r, c10, c3, 1	/* AUX memory attribute indirection */
#define VBAR(_r)		p15, 0, _r, c12, c0, 0	/* Vector Base Address */
#define TPIDRPRW(_r)		p15, 0, _r, c13, c0, 4	/* PL1 only Thread ID */
#define TPIDRURO(_r)		p15, 0, _r, c13, c0, 3	/* R/O Thread ID */
#define TPIDRURW(_r)		p15, 0, _r, c13, c0, 2	/* R/W Thread ID */
#define CNTFRQ(_r)		p15, 0, _r, c14, c0, 0	/* Counter Frequency */
#define CNTKCTL(_r)		p15, 0, _r, c14, c1, 0	/* Timer PL1 control */
#define CNTP_CTL(_r)		p15, 0, _r, c14, c2, 1	/* Physical Counter control */
#define CNTV_TVAL(_r)		p15, 0, _r, c14, c3, 0	/* Virtual Counter timer value */
#define CNTV_CTL(_r)		p15, 0, _r, c14, c3, 1	/* Virtual Counter control */

#define CCSIDR(_r)		p15, 1, _r, c0 , c0, 0	/* Cache Size ID */
#define CLIDR(_r)		p15, 1, _r, c0 , c0, 1	/* Cache Level ID */

#define CSSELR(_r)		p15, 2, _r, c0 , c0, 0	/* Cache Size selection */

#define TTBR0_64(_r1, _r2)	p15, 0, _r1, _r2, c2	/* Translation Table Base for long descriptors */
#define PAR(_r1, _r2)		p15, 0, _r1, _r2, c7	/* Physical Address */
#define CNTPCT(_r1, _r2)	p15, 0, _r1, _r2, c14	/* Physical Counter */
#define CNTVCT(_r1, _r2)	p15, 1, _r1, _r2, c14	/* Virtual Counter */
#define CNTV_CVAL(_r1, _r2)	p15, 3, _r1, _r2, c14	/* Virtual Counter Compare Value */
#define CNTVOFF(_r1, _r2)	p15, 4, _r1, _r2, c14	/* Virtual Counter Offset */
#define VTTBR(_r1, _r2)		p15, 6, _r1, _r2, c2	/* Virtualization Translation Table Base */

#define MPIDR_MPF		(1 << 31)		/* Indicates that processor implements the
		    					 * Multiprocessing Extensions register format,
		    					 * should be 1 */
#define MPIDR_UP		(1 << 30)		/* Multiprocessing extension:
							 * 0 - Processor is part of cluster
						    	 * 1 - Processor is uniprocessor */
#define MPIDR_CPUID		(0x3)			/* Indicates the processor number,
    							 * NOTE: no cluster Id support yet */

#define ACTLR_SMP		(1 << 6)		/* Signals if CPU is taking part in
    							 * coherency or not*/

#define PAR_PA_MASK		0xfffff000		/* PAR Physical Address */
#define PAR_F			(1 << 0)		/* Translation failed flag */

#endif /* __ASM_SYS_CTRL_H__ */
