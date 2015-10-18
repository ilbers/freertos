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

#ifndef __MM_H__
#define __MM_H__

#include <sys.h>
#include <stdint.h>

#define PAGE_MASK		0x00000fff

#define STAGE1			1		/* Stage1 memory mapping */

#define L1_TABLE_INDEX_SHIFT	30		/* First-level table entry indexed by VA[31:30] */
#define L1_TABLE_INDEX_MASK	0x03		/* Entry mask [1:0] bits */
#define L1_MB_ENTRY_ADDR_SHIFT	30		/* Memory block address shift in L1 table entry */
#define L1_MB_ENTRY_ADDR_MASK	0xc0000000U	/* Addr mask [31:30] bits */
#define l1_table_entry(va)	(					\
				 ((va) >> L1_TABLE_INDEX_SHIFT) &	\
				 L1_TABLE_INDEX_MASK			\
				)

#define L2_TABLE_INDEX_SHIFT	21		/* Second-level table entry indexed by VA[29:21] */
#define L2_TABLE_INDEX_MASK	0x01ff		/* Entry mask [0:8] bits */
#define L2_MB_ENTRY_ADDR_SHIFT	21		/* Memory block address shift in L2 table entry */
#define L2_MB_ENTRY_ADDR_MASK	0xffe00000U	/* Addr mask [31:21] bits */
#define l2_table_entry(va)	(					\
				 ((va) >> L2_TABLE_INDEX_SHIFT) &	\
				 L2_TABLE_INDEX_MASK			\
				)

#define L3_TABLE_INDEX_SHIFT	12		/* Third-level table entry indexed by VA[20:12] */
#define L3_TABLE_INDEX_MASK	0x001ff		/* Entry mask [0:8] bits */
#define L3_PG_ENTRY_ADDR_SHIFT	12		/* Page adress shift in L3 table entry */
#define L3_PG_ENTRY_ADDR_MASK	0xfffff000U	/* Addr mask [20:12] bits */
#define l3_table_entry(va)	(					\
				 ((va) >> L3_TABLE_INDEX_SHIFT) &	\
				 L3_TABLE_INDEX_MASK			\
				)

#define LX_PT_ENTRY_ADDR_SHIFT	12		/* Next level PT address shift */
#define LX_PT_ENTRY_ADDR_MASK	0xfffff000U	/* Addr mask [31:12] bits */

#define MEM_BLOCK_1GB		(1 << 30)
#define MEM_BLOCK_2MB		(1 << 21)
#define MEM_BLOCK_4KB		(1 << 12)

/* Mapped memory types */
typedef enum {
	MEM_NORMAL = 0,				/* Normal memory */
	MEM_DEVICE,				/* Device memory */
	MEM_ORDERED,				/* Strongly-orderred memory */
} mem_type_t;

/* Invalidate entire PL0-1 TLB */
#define tlb_invalidate_all()	do {							\
					ISB(sy);					\
					DSB(sy);					\
					WRITE_CP_REG32(0, TLBIALL(R0_inline)); 		\
					ISB(sy);					\
				} while(0)

/* Invalidate branch prediction */
#define bp_invalidate_all()	do {							\
					ISB(sy);					\
					DSB(sy);					\
					WRITE_CP_REG32(0, BPIALL(R0_inline));		\
					ISB(sy);					\
				} while(0)

/* Page table entry */
typedef	struct __attribute__((__packed__)) {
	unsigned int low;
	unsigned int high;
} page_section_t;

/* MM API */
int32_t copy_memory_region(void *dest,
			   const void *source,
			   uint32_t count);
uint64_t va_to_pa(uint32_t va);
void mm_init();
int32_t free_memory_region(uint32_t virt_addr,
			   uint32_t size);
int32_t map_memory_region(uint32_t virt_addr,
			  uint32_t phys_addr,
			  uint32_t size,
			  uint32_t type);
void flush_caches();
void *memset(void *dest, int32_t val, uint32_t count);
uint32_t memcpy(void *dest, void *source, uint32_t count);

#endif /* __MM_H__ */
