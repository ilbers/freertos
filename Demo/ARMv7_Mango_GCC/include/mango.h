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

#ifndef __MANGO_H__
#define __MANGO_H__

/* Mango API */
uint32_t mango_dc_open(uint32_t ch, uint32_t dest);
uint32_t mango_dc_close(uint32_t ch);
uint32_t mango_dc_write(uint32_t ch, const uint8_t *p, uint32_t len);
uint32_t mango_dc_read(uint32_t ch, uint8_t *p, uint32_t len);

uint32_t mango_get_partition_id(void);

uint32_t mango_watchdog_start(void);
uint32_t mango_watchdog_stop(void);
uint32_t mango_watchdog_ping(void);
uint32_t mango_watchdog_set_timeout(uint32_t timeout);

#endif /* __MANGO_H__ */

