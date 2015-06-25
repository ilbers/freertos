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

#ifndef __SYS_H__
#define __SYS_H__

/*
 * Assembler intruction wrappers:
 */

/* Data Synchronization Barrier */
#define DSB(option)	asm volatile("dsb "#option : : : "memory")

/* Data Synchronization Barrier */
#define DMB(option)	asm volatile("dmb "#option : : : "memory")

/* Instruction Synchronization Barrier */
#define ISB(option)	asm volatile("isb "#option)

/* Send Event */
#define SEV()		asm volatile("sev")

/* Wait For Interrupt */
#define WFI()		asm volatile("wfi" : : : "memory")

/* Wait For Event */
#define WFE()		asm volatile("wfe" : : : "memory")

/* Interrupt Enable */
#define CPSIE(option)	asm volatile("cpsie "#option : : : "memory")

/* Interrupt Disable */
#define CPSID(option)	asm volatile("cpsid "#option : : : "memory")

/*
 * Data segment wrappers:
 */

/* Data segment being shared between CPU cores */
#define __shared	__attribute__((__section__(".shared.data")))

#endif /* __SYS_H__ */
