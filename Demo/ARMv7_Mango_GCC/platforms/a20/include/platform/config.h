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

#ifndef __A20_PLATFORM_CONFIG_H__
#define __A20_PLATFORM_CONFIG_H__

/* A20 settings */
#define GIC_DIST_BASE				0x01c81000
#define GIC_CPU_BASE				0x01c82000
#define NR_CPU_SPIS				128		/* Shared peripheral interrupts */

#define configFPU_NUM_REGISTERS			32
#define configFPU_BYTES_PER_REGISTER		8

#endif /* __A20_PLATFORM_CONFIG_H__ */
