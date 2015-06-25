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

#ifndef __ASM_MMU_H__
#define __ASM_MMU_H__

#define L1_ENTRIES		4				/* 4GB of physical memory */
#define L2_ENTRIES		512				/* 1 GB of memory (512 * 2MB) */
#define L3_ENTRIES		512				/* 2 MB of memory (512 * 4KB) */

/* MAIR0 contains the following attributes:
 *
 * index  value      description
 * ----------------------------------
 * 0_00   0000_0100  Device memory
 * 0_01   0100_0100  In/Out non-cachable
 * 0_10   1010_1010  Write-through
 * 0_11   1111_1111  Write-back alloc
 */
#define MAIR0_ATTR		0xffaa4404

#define M_ATTR_DEVICE		0
#define M_ATTR_NCACHE		1
#define M_ATTR_WTHROUGH		2
#define M_ATTR_WBACK_ALLOC	3

/* Stage 2 memory attributes
 */

/* Stage 2: normal memory */
#define M_S2_ATTR_I_NO_O_NO	0x5				/* b0101 - inner/outer non-cacheable */
#define M_S2_ATTR_I_WT_O_NO	0x6				/* b1010 - inner write-through, outer non-cacheable */
#define M_S2_ATTR_I_WB_O_NO	0x7				/* b1011 - inner write-back, outer non-cacheable */
#define M_S2_ATTR_I_WB_O_WB	0xf				/* b1111 - inner write-back, outer write-back */

/* Stage 2: device memory */
#define M_S2_ATTR_DEVICE	0x1				/* b0001 - device memory */

/* Stage 2: strongly-ordered memory */
#define M_S2_ATTR_ORDERED	0x0				/* b0000 - strongly-ordered memory */

/* Long Descriptor definitions */
#define LD_BIT0(x)		(((x) & 1) << 0)
#define LD_BIT0_MASK(x)		((x) & 1)
#define LD_UNMAPPED		0				/* This descriptor is invalid */
#define LD_MAPPED		1				/* This descriptor is mapped */

#define LD_BIT1(x)		(((x) & 1) << 1)
#define LD_BIT1_MASK(x)		(((x) >> 1) & 1)
#define LD_MEM_BLOCK		0				/* Descriptor points to memory block */
#define LD_TABLE		1				/* Descriptor points to page table */
#define LD_PAGE			1				/* Descriptor points to L3 page */

#define LD_S1_ATTR(x)		(((x) & 0x7) << 2)		/* Stage 1 attributes */
#define LD_S2_ATTR(x)		(((x) & 0xf) << 2)		/* Stage 2 attributes */

#define LD_NS(x)		(((x) & 1) << 5)
#define LD_SECURE		0				/* Secure memory access */
#define LD_NON_SECURE		1				/* Non-secure memory access */

#define LD_AP(x)		(((x) & 3) << 6)
#define LD_AP_RW_PL1		0				/* b00: R/W only at PL1 */
#define LD_AP_RW_ALL		1				/* b01: R/W at any PL */
#define LD_AP_RO_PL1		2				/* b10: R/O only at PL1 */
#define LD_AP_RO_ALL		3				/* b11: R/O at any PL */

#define LD_SH(x)		(((x) & 3) << 8)
#define LD_SH_NON_SHARE		0				/* b00: non-sharable */
#define LD_SH_UNPREDICT		1				/* b01: UNPREDICTABLE */
#define LD_SH_OUT_SHARE		2				/* b10: outer shareable */
#define LD_SH_INN_SHARE		3				/* b11: inner shareable */

#define LD_AF(x)		(((x) & 1) << 10)
#define LD_AF_DISABLE		0				/* Use full-range access permissions */
#define LD_AF_ENABLE		1				/* Use AP[2:1] */

#define LD_nG(x)		(((x) & 1) << 11)
#define LD_nG_GLOB		0				/* Translation is global */
#define LD_nG_NON_GLOB		1				/* Process-specific translation */

/* Stage1: Next-Level page table entry */
#define ENTRY_PAGE_TABLE	(					\
				 LD_BIT0(LD_MAPPED)		|	\
				 LD_BIT1(LD_TABLE)			\
				)

/* Stage1: Memory Block */

#define ENTRY_MEM_BLOCK(_attr)						\
				(					\
				 LD_BIT0(LD_MAPPED)		|	\
				 LD_BIT1(LD_MEM_BLOCK)		|	\
				 LD_S1_ATTR(_attr)		|	\
				 LD_NS(LD_NON_SECURE)		|	\
				 LD_AP(LD_AP_RW_ALL)		|	\
				 LD_SH(LD_SH_INN_SHARE)		|	\
				 LD_AF(LD_AF_ENABLE)		|	\
				 LD_nG(LD_nG_GLOB)			\
				)

/* Stage1: Memory Page */
#define ENTRY_PAGE(_attr)						\
				(					\
				 LD_BIT0(LD_MAPPED)		|	\
				 LD_BIT1(LD_PAGE)		|	\
				 LD_S1_ATTR(_attr)		|	\
				 LD_NS(LD_NON_SECURE)		|	\
				 LD_AP(LD_AP_RW_ALL)		|	\
				 LD_SH(LD_SH_INN_SHARE)		|	\
				 LD_AF(LD_AF_ENABLE)		|	\
				 LD_nG(LD_nG_GLOB)			\
				)

#endif /* __ASM_MMU_H__ */
