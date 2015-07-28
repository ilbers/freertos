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

#include <portmacro.h>
#include <FreeRTOSConfig.h>

#include <interrupt.h>
#include <io.h>
#include <mango.h>
#include <sys_print.h>

static gicd_t gicd;
static gicc_t gicc;

__attribute__(( used )) const uint32_t ulICCPMR	= (GIC_CPU_BASE + GICC_PMR);

/* Interrupt handler tables */
static irq_handler_t irq_table[NR_CPU_IRQS];

static void gic_dist_init()
{
	uint32_t i;
	uint32_t reg;

	gicd.base_addr =  GIC_DIST_BASE;

	/* Disable distributor */
	mem_write32(gicd.base_addr + GICD_CTLR, 0);

	reg = mem_read32(gicd.base_addr + GICD_TYPER);
	gicd.cpus = ((reg >> GICD_TYPER_CPU_SHIFT) & GICD_TYPER_CPU_MASK) + 1;
	gicd.lines = 32 * ((reg & GICD_TYPER_LINES_MASK) + 1);

	/* Route all the global interrupts to this boot CPU */
	for (i = NR_CPU_PRIVATE_IRQS; i < gicd.lines; i += 4)
	{
		mem_write32(gicd.base_addr + GICD_TARGETSR(i / 4), GICD_BOOT_CPU_MASK);
	}

	/* Set all the global interrupts active-low y default */
	for (i = NR_CPU_PRIVATE_IRQS; i < gicd.lines; i += 16)
	{
		mem_write32(gicd.base_addr + GICD_ICFGR(i / 16), 0);
	}

	/* Set default prio for all the global interrupts */
	for (i = NR_CPU_PRIVATE_IRQS; i < gicd.lines; i += 4)
	{
		mem_write32(gicd.base_addr + GICD_IPRIORITYR(i / 4), GICD_IRQ_PRIO_MASK);
	}

	/* Disable all the global iterrupts */
	for (i = NR_CPU_PRIVATE_IRQS; i < gicd.lines; i += 32)
	{
		mem_write32(gicd.base_addr + GICD_ICENABLER(i / 32), 0xffffffff);
	}

	/* Assign default handlers to IRQs */
	for (i = 0; i < NR_CPU_IRQS; i++)
	{
		irq_table[i] = 0;
	}

	/* Enable distributor */
	mem_write32(gicd.base_addr + GICD_CTLR, GICD_CTLR_EN);

	sys_print_msg("[GIC] distributor setup complete\r\n");
}

static void gic_cpu_init()
{
	uint32_t i;

	gicc.base_addr = GIC_CPU_BASE;
	gicc.cpu_id = 0;

	/* Disable private per-CPU PPIs (Private Peripheral Interrupt) */
	mem_write32(gicd.base_addr + GICD_ICENABLER(0), 0xffff0000);
	/* Enable private per-CPU SGIs (Software Generated Interrupts) */
	mem_write32(gicd.base_addr + GICD_ISENABLER(0), 0x0000ffff);

	/* Set default prio for SGIs */
	for (i = 0; i < NR_CPU_PRIVATE_SGIS; i += 4)
	{
		mem_write32(gicd.base_addr + GICD_IPRIORITYR(i / 4), GICD_IPI_PRIO_MASK);
	}

	/* Set default prio for PPIs */
	for (i = NR_CPU_PRIVATE_SGIS; i < NR_CPU_PRIVATE_IRQS; i += 4)
	{
		mem_write32(gicd.base_addr + GICD_IPRIORITYR(i / 4), GICD_IRQ_PRIO_MASK);
	}

	/* Use only 1 priority level */
	gic_set_priority_mask(0xff);

	/* No interrupt group priorities */
	mem_write32(gicc.base_addr + GICC_BPR, 0x07);

	/* Enable CPU interface */
	mem_write32(gicc.base_addr + GICC_CTLR, GICC_CTLR_EN);

	sys_print_msg("[GIC] CPU interface ready\r\n");
}

void enable_local_irqs()
{
	DSB(sy);
	ISB(sy);
	CPSIE(i);
}

void disable_local_irqs()
{
	DSB(sy);
	ISB(sy);
	CPSID(i);
}

uint32_t enable_one_irq(uint32_t nr)
{
	uint32_t reg;

	reg = mem_read32(gicd.base_addr + GICD_ISENABLER(nr / 32));
	if (reg & (1 << (nr % 32)))
	{
		/* Interrupt is already enabled */
		return 1;
	}
	else
	{
		reg = 1 << (nr % 32);
		mem_write32(gicd.base_addr + GICD_ISENABLER(nr / 32), reg);

		return 0;
	}
}

void disable_one_irq(uint32_t nr)
{
	uint32_t reg;

	reg = mem_read32(gicd.base_addr + GICD_ICENABLER(nr / 32));
	if (reg & (1 << (nr % 32)))
	{
		reg = 1 << (nr % 32);
		mem_write32(gicd.base_addr + GICD_ICENABLER(nr / 32), reg);
	}
}

void register_irq_handler(uint32_t nr, irq_handler_t handler)
{
	if (nr < NR_CPU_IRQS)
	{
		irq_table[nr] = handler;
	}
	else
	{
		sys_print_msg("IRQ: invalid nr (%d)\r\n", nr);
	}
}


static uint32_t nest = 0;
void gic_do_interrupt()
{
	uint32_t reg;
	uint32_t nr;

	nest++;

	/* Get the interrupt number */
	reg = mem_read32(gicc.base_addr + GICC_IAR);

	nr = reg & 0x3ff;

	if (nr < NR_CPU_IRQS)
	{
		if (irq_table[nr])
		{
			/* Process IRQ */
			irq_table[nr]();
		}

		/* Drop the priority and deactivate */
		mem_write32(gicc.base_addr + GICC_EOIR, nr);
	}
	else
	{
		sys_print_msg("IRQ: spurious interrupt\r\n");
		sys_print_msg("     nr = %d\r\n", nr);
		sys_print_msg("     PMR = %d\r\n", gic_get_priority_mask());
		sys_print_msg("     nest = %d\r\n", nest);

		mango_debug_call(0);

		for (;;);
	}

	nest--;
}

void irq_init()
{
	/* Setup distributor */
	gic_dist_init();

	/* Setup CPU interface */
	gic_cpu_init();
}

void gic_set_priority_mask(uint32_t mask)
{
	mem_write32(gicc.base_addr + GICC_PMR, mask & 0xff);
	ISB();
	DSB();
}

uint32_t gic_get_priority_mask(void)
{
	return mem_read32(gicc.base_addr + GICC_PMR);
}

void gic_set_irq_priority(uint32_t nr, uint32_t prio)
{
	uint32_t reg;

	reg = mem_read32(gicd.base_addr + GICD_IPRIORITYR(nr / 4));                              
	reg &= ~(0xff << ((nr % 4) * 8));                                                        
	reg |= ((prio & 0xff) << ((nr % 4) * 8));                                                
	mem_write32(gicd.base_addr + GICD_IPRIORITYR(nr / 4), reg); 
}
