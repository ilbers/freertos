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

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

#define GICD_CTLR		0x0000			/* Distributor control register */
#define GICD_TYPER		0x0004			/* Interrupt controller type register */
#define GICD_ISENABLER(n)	(0x0100 + (n * 4))	/* Interrupt set-enable register */
#define GICD_ICENABLER(n)	(0x0180 + (n * 4))	/* Interrupt clear-enable register */
#define GICD_IPRIORITYR(n)	(0x0400 + (n * 4))	/* Interrupt priority register */
#define GICD_TARGETSR(n)	(0x0800 + (n * 4))	/* Interrupt processor targets register */
#define GICD_ICFGR(n)		(0x0c00 + (n * 4))	/* Interrupt configuration register */
#define GICD_SGIR		0x0f00			/* Generate SGI register */

#define GICD_CTLR_EN		(1 << 0)		/* Distributor enable bit */
#define GICD_TYPER_CPU_SHIFT	5			/* CPU bits shift */
#define GICD_TYPER_CPU_MASK	0x07
#define GICD_TYPER_LINES_MASK	0x07

#define GICD_SGIR_CPU_SHIFT	24			/* Target CPUs list shift */
#define GICD_SGIR_CPU_ABT	0x01			/* b01 - all but this CPU */

#define GICC_CTLR		0x00			/* Cpu interface control register */
#define GICC_PMR		0x04			/* Priority mask register */
#define GICC_BPR		0x08			/* Binary point register */
#define GICC_IAR		0x0c			/* Interrupt Acknowledge register */
#define GICC_EOIR		0x10			/* End Of Interrupt register */
#define GICC_DIR		0x1000			/* Deactivate interrupt */

#define GICC_CTLR_EN		(1 << 0)		/* Enable bit */
#define GICC_CTLR_EOI		(1 << 9)		/* Control EOIR behavior */

#define NR_CPU_PRIVATE_SGIS	16			/* Number of private SGIs */
#define NR_CPU_PRIVATE_PPIS	16			/* Number of private PPIs */
#define NR_CPU_PRIVATE_IRQS	( NR_CPU_PRIVATE_SGIS +	\
				  NR_CPU_PRIVATE_PPIS	\
				)			/* Number of private per-CPU interrupts */

#define NR_CPU_IRQS		(NR_CPU_PRIVATE_IRQS +	\
				NR_CPU_SPIS)		/* Number of IRQ implemented */

#define GICD_BOOT_CPU_MASK	0x01010101		/* 0x01 = b0000_0001 - CPU 0 is active */

#define GICD_IRQ_PRIO		0xa0			/* Default prio for IRQ */
#define GICD_IPI_PRIO		0x90			/* Default prio for IPI  */
#define GICD_IRQ_PRIO_MASK	( GICD_IRQ_PRIO		| \
				 (GICD_IRQ_PRIO << 8)	| \
				 (GICD_IRQ_PRIO << 16)	| \
				 (GICD_IRQ_PRIO << 24)	  \
				) 			/* Default IRQ prio mask to be written to IPRIORYTY reg */

#define GICD_IPI_PRIO_MASK	( GICD_IPI_PRIO		| \
				 (GICD_IPI_PRIO << 8)	| \
				 (GICD_IPI_PRIO << 16)	| \
				 (GICD_IPI_PRIO << 24)	  \
				)			/* Default IPI prio mask to be written to IPRIORYTY reg */

#define disable_local_irqs_save(flags)					\
	do {								\
		asm volatile ("mrs %0, cpsr\r\n"			\
			      : "=r" (flags) :: "memory", "cc");	\
		disable_local_irqs();					\
	} while(0);

#define enable_local_irqs_load(flags)					\
	do {								\
		asm volatile ("msr cpsr_c, %0\r\n"			\
			      :: "r" (flags) : "memory", "cc");		\
	} while(0);

/* GICD data structure */
typedef struct {
	uint32_t base_addr;
	uint8_t  cpus;
	uint8_t  lines;
} gicd_t;

/* GICC data structure */
typedef struct {
	uint32_t base_addr;
	uint32_t cpu_id;
} gicc_t;

/* Interrupt handler */
typedef void (*irq_handler_t)(void);

/* Interrupts API */
void enable_local_irqs();
void disable_local_irqs();
void irq_init();
void gic_do_interrupt();
void register_irq_handler(uint32_t nr, irq_handler_t handler);
uint32_t enable_one_irq(uint32_t nr);
void disable_one_irq(uint32_t nr);
void gic_set_priority_mask(uint32_t mask);
uint32_t gic_get_priority_mask(void);
void gic_set_irq_priority(uint32_t nr, uint32_t prio);

#endif /* __INTERRUPT_H__ */
