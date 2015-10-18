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
#include <interrupt.h>
#include <io.h>
#include <mm.h>
#include <sys_print.h>

#define L3_POOL_SIZE	32

unsigned int boot_stack[1024]  __attribute__((__aligned__(1 << 12)));
unsigned int irq_stack[1024]   __attribute__((__aligned__(1 << 12)));
unsigned int abort_stack[1024] __attribute__((__aligned__(1 << 12)));

page_section_t l1_page_table[L1_ENTRIES]               __attribute__((__aligned__(1 << 14)));
page_section_t l2_page_table[L1_ENTRIES][L2_ENTRIES]   __attribute__((__aligned__(1 << 14)));
page_section_t l3_page_table[L3_POOL_SIZE][L3_ENTRIES] __attribute__((__aligned__(1 << 14)));

/* Bit-mask to manage L3 tables availability */
static uint32_t l3_ptable_pool;

/* Offset between data VA and PA */
static uint32_t va_pa_offset;

static uint32_t get_l3_ptable()
{
	uint8_t i;
	uint32_t pt_addr = 0;

	for (i = 0; i < L3_POOL_SIZE; i++)
	{
		if ((l3_ptable_pool & (1 << i)) == 0)
		{
			pt_addr = va_to_pa((uint32_t)l3_page_table[i]);
			l3_ptable_pool |= 1 << i;
			break;
		}
	}

	return pt_addr;
}

uint64_t va_to_pa(uint32_t va)
{
	uint64_t par_old, par;

	par_old = READ_CP_REG64(PAR(R0_inline, R1_inline));
	WRITE_CP_REG32(va, ATS1CPR(R0_inline));

	/* Flush pipe-line */
	ISB(sy);

	par = READ_CP_REG64(PAR(R0_inline, R1_inline));
	WRITE_CP_REG64(par_old, PAR(R0_inline, R1_inline));

	/* Check if convertion was successful */
	if (par & PAR_F)
	{
		par = 0;
	}
	else
	{
		par &= PAR_PA_MASK;
		par |= (va & PAGE_MASK);
	}

	return par;
}

static int32_t map_l1_l2_page_table(uint32_t virt_addr)
{
	uint16_t l1 = l1_table_entry(virt_addr);
	int32_t ret = 0;

	/* Map L1->L2 entry if it's unmapped */
	if (LD_BIT0(l1_page_table[l1].low) == LD_UNMAPPED)
	{
		l1_page_table[l1].low = va_to_pa((uint32_t)l2_page_table[l1]) & LX_PT_ENTRY_ADDR_MASK;
		l1_page_table[l1].low |= ENTRY_PAGE_TABLE;
		l1_page_table[l1].high = 0;
	}
	else
	if (LD_BIT1(l1_page_table[l1].low) == LD_MEM_BLOCK)
	{
		sys_print_msg("Failed to map L1->L2 table, L1 entry %d is a memory block\n\r", l1);
		ret = -1;
	}

	return ret;
}

static int32_t map_l1_mem_block(uint32_t virt_addr, uint32_t phys_addr, uint32_t attr)
{
	uint16_t l1 = l1_table_entry(virt_addr);
	int32_t ret = 0;

	/* Map L1->MB if it's unmapped */
	if (LD_BIT0(l1_page_table[l1].low) == LD_UNMAPPED)
	{
		l1_page_table[l1].low = phys_addr & L1_MB_ENTRY_ADDR_MASK;
		l1_page_table[l1].low |= ENTRY_MEM_BLOCK(attr);
		l1_page_table[l1].high = 0;
	}
	else
	{
		sys_print_msg("Failed to map L1 memory block, address 0x%8x already mapped\r\n",
		      virt_addr & L1_MB_ENTRY_ADDR_MASK);
		ret = -1;
	}

	return ret;
}

static int32_t map_l2_l3_page_table(uint32_t virt_addr)
{
	uint16_t l1 = l1_table_entry(virt_addr);
	uint16_t l2 = l2_table_entry(virt_addr);
	uint32_t l3_addr;
	int32_t ret = 0;

	/* Map L2->L3 entry if it's unmapped */
	if (LD_BIT0(l2_page_table[l1][l2].low) == LD_UNMAPPED)
	{
		l3_addr = get_l3_ptable();
		if (l3_addr)
		{
			l2_page_table[l1][l2].low = l3_addr & LX_PT_ENTRY_ADDR_MASK;
			l2_page_table[l1][l2].low |= ENTRY_PAGE_TABLE;
			l2_page_table[l1][l2].high = 0;
		}
		else
		{
			sys_print_msg("Failed to map L2->L3 table, no free L3 tables left\r\n");
			ret = -1;
		}
	}
	else
	if (LD_BIT0(l2_page_table[l1][l2].low) == LD_MEM_BLOCK)
	{
		sys_print_msg("Failed to map L2->L3 table, L2 entry %d is memory block\r\n", l2);
		ret = -1;
	}

	return ret;
}

static int32_t map_l2_mem_block(uint32_t virt_addr, uint32_t phys_addr, uint32_t attr)
{
	uint16_t l1 = l1_table_entry(virt_addr);
	uint16_t l2 = l2_table_entry(virt_addr);
	int32_t ret = 0;

	/* Map L2->MB entry if it's unmapped */
	if (LD_BIT0(l2_page_table[l1][l2].low) == LD_UNMAPPED)
	{
		l2_page_table[l1][l2].low = phys_addr & L2_MB_ENTRY_ADDR_MASK;
		l2_page_table[l1][l2].low |= ENTRY_MEM_BLOCK(attr);
		l2_page_table[l1][l2].high = 0;
	}
	else
	{
		sys_print_msg("Failed to map L2 memory block, address 0x%8x already mapped\r\n",
			      virt_addr & L2_MB_ENTRY_ADDR_MASK);
		ret = -1;
	}

	return ret;
}

static int32_t map_l3_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t attr)
{
	uint16_t l1 = l1_table_entry(virt_addr);
	uint16_t l2 = l2_table_entry(virt_addr);
	uint16_t l3 = l3_table_entry(virt_addr);
	page_section_t *p_l3_ptable;
	uint32_t l3_addr;
	int32_t ret = 0;

	l3_addr = l2_page_table[l1][l2].low & LX_PT_ENTRY_ADDR_MASK;
	l3_addr -= va_pa_offset;

	p_l3_ptable = (page_section_t *)l3_addr;

	if (LD_BIT0(p_l3_ptable[l3].low) == LD_UNMAPPED)
	{
		p_l3_ptable[l3].low = phys_addr & L3_PG_ENTRY_ADDR_MASK;
		p_l3_ptable[l3].low |= ENTRY_PAGE(attr);
		p_l3_ptable[l3].high = 0;
	}
	else
	{
		sys_print_msg("Failed to map L3 page, address 0x%8x already mapped\r\n",
			      virt_addr & L3_PG_ENTRY_ADDR_MASK);
		ret = -1;
	}

	return ret;
}

static int32_t map_fixed_memory_region(uint32_t virt_addr,
					uint32_t phys_addr,
					uint32_t size,
					uint32_t attr)
{
	int32_t ret = 0;

	switch (size)
	{
	case MEM_BLOCK_1GB:
		ret = map_l1_mem_block(virt_addr, phys_addr, attr);

		break;

	case MEM_BLOCK_2MB:
		ret = map_l1_l2_page_table(virt_addr);
		if (ret)
		{
			break;
		}

		ret = map_l2_mem_block(virt_addr, phys_addr, attr);

		break;

	case MEM_BLOCK_4KB:
		ret = map_l1_l2_page_table(virt_addr);
		if (ret)
		{
			break;
		}

		ret = map_l2_l3_page_table(virt_addr);
		if (ret)
		{
			break;
		}

		ret = map_l3_page(virt_addr, phys_addr, attr);

		break;

	default:
		break;
	}

	return ret;
}

static uint32_t get_memory_attributes(uint32_t type)
{
	uint32_t attr = 0;

	switch (type)
	{
	case MEM_NORMAL:
		/* NOTE: at the moment no caches supported */
		attr = M_ATTR_NCACHE;

		break;

	case MEM_DEVICE:
		/* Device memory */
		attr = M_ATTR_DEVICE;

		break;

	case MEM_ORDERED:
		/* NOTE: not implemented yet */

		break;

	default:
		sys_print_msg("Invalid memory type id = 0x%x\r\n", type);

		break;
	}

	return attr;
}
	
int32_t map_memory_region(uint32_t virt_addr,
			  uint32_t phys_addr,
			  uint32_t size,
			  uint32_t type)
{
	uint32_t attr = get_memory_attributes(type);
	uint32_t pa = phys_addr;
	uint32_t va = virt_addr;
	uint32_t irq_flags;
	int32_t ret = 0;

	disable_local_irqs_save(irq_flags);

	while (va < virt_addr + size)
	{
		if (((pa & ~L1_MB_ENTRY_ADDR_MASK) == 0) &&
		    ((va & ~L1_MB_ENTRY_ADDR_MASK) == 0) &&
		    ((size - (va - virt_addr)) >= MEM_BLOCK_1GB))
		{
			ret = map_fixed_memory_region(va, pa, MEM_BLOCK_1GB, attr);
			if (ret == 0)
			{
				pa += MEM_BLOCK_1GB;
				va += MEM_BLOCK_1GB;
			}
			else
			{
				break;
			}
		}
		else
		if (((pa & ~L2_MB_ENTRY_ADDR_MASK) == 0) &&
		    ((va & ~L2_MB_ENTRY_ADDR_MASK) == 0) &&
		    ((size - (va - virt_addr)) >= MEM_BLOCK_2MB))
		{
			ret = map_fixed_memory_region(va, pa, MEM_BLOCK_2MB, attr);
			if (ret == 0)
			{
				pa += MEM_BLOCK_2MB;
				va += MEM_BLOCK_2MB;
			}
			else
			{
				break;
			}
		}
		else
		if (((pa & ~L3_PG_ENTRY_ADDR_MASK) == 0) &&
		    ((va & ~L3_PG_ENTRY_ADDR_MASK) == 0) &&
		    ((size - (va - virt_addr)) >= MEM_BLOCK_4KB))
		{
			ret = map_fixed_memory_region(va, pa, MEM_BLOCK_4KB, attr);
			if (ret == 0)
			{
				pa += MEM_BLOCK_4KB;
				va += MEM_BLOCK_4KB;
			}
			else
			{
				break;
			}
		}
		else
		{
			sys_print_msg("Failed to map VA(0x%8x) -> PA(0x%8x), bad alignment\r\n",
				      virt_addr, phys_addr);
		}
	}

	return ret;
}

int32_t free_memory_region(uint32_t virt_addr,
			   uint32_t size)
{
	uint32_t va = virt_addr;
	uint32_t l1, l2;
	uint32_t irq_flags;
	int32_t ret = 0;

	disable_local_irqs_save(irq_flags);

	while (va < virt_addr + size)
	{
		l1 = l1_table_entry(va);
		l2 = l2_table_entry(va);

		if (LD_BIT0_MASK(l1_page_table[l1].low) &&
		    LD_BIT1_MASK(l1_page_table[l1].low) == LD_MEM_BLOCK)
		{
			if ((size - (va - virt_addr)) >= MEM_BLOCK_1GB)
			{
				l1_page_table[l1].low = 0;
				l1_page_table[l1].high = 0;
				va += MEM_BLOCK_1GB;

				continue;
			}
		}
		else
		if (LD_BIT0_MASK(l2_page_table[l1][l2].low) &&
		    LD_BIT1_MASK(l2_page_table[l1][l2].low) == LD_MEM_BLOCK)
		{
			if ((size - (va - virt_addr)) >= MEM_BLOCK_2MB)
			{
				l2_page_table[l1][l2].low = 0;
				l2_page_table[l1][l2].high = 0;
				va += MEM_BLOCK_2MB;

				continue;
			}
		}

		sys_print_msg("MM: can't free memory block at 0x%8x\r\n", va);
		ret = -1;
		break;
	}

	enable_local_irqs_load(irq_flags);

	return ret;
}

int32_t copy_memory_region(void *dest, const void *source, uint32_t count)
{
	volatile uint8_t* ptrSrc = (uint8_t*)source + count;
	volatile uint8_t* ptrDest = (uint8_t*)dest + count;
	uint32_t loops = count;

	/* Copy bytes to align the data by 32-bit word size */
	while (loops % sizeof(uint32_t))
	{
		loops--;
		*(--ptrDest) = *(--ptrSrc);
	}

	/* Copy data by 32-bits words */
	while (loops)
	{
		loops -= sizeof(uint32_t);
		ptrDest -= sizeof(uint32_t);
		ptrSrc -= sizeof(uint32_t);
		*((uint32_t*)ptrDest) = *((uint32_t*)ptrSrc);
	}

	return count;
}

uint32_t memcpy(void *dest, void *source, uint32_t count)
{
	return copy_memory_region(dest, source, count);
}

void *memset(void *dest, int32_t val, uint32_t count)
{
	volatile uint8_t* ptrDest = (uint8_t*)dest;

	while (count--)
	{
		*ptrDest++= val;
	}

	return dest;
}

extern uint32_t _bss_start, _bss_end;

void flush_bss()
{

	uint8_t *p = (void *)&_bss_start;

	while ((void *)p < (void *)&_bss_end)
	{
		*p++ = 0;
	}
}

void mm_init()
{
	int i, j;

	/* Flush pool mask */
	l3_ptable_pool = 0;

	/* Flush L3 page table pool */
	for (i = 0; i < L3_POOL_SIZE; i++)
	{
		for (j = 0; j < L3_ENTRIES; j++)
		{
			l3_page_table[i][j].low = 0;
			l3_page_table[i][j].high = 0;
		}
	}

	va_pa_offset = 0;

	flush_caches();
	tlb_invalidate_all();
	bp_invalidate_all();

	sys_print_msg("[MM] memory system ready\r\n");
}
