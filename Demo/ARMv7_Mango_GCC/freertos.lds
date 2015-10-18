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

ENTRY(start)
OUTPUT_ARCH(arm)
PHDRS
{
    text PT_LOAD ;
}
SECTIONS
{
    . = 0x40000000;

    _start = .;

    .text : {
        *(.text)
    } :text = 0x9090

    .data : {
        *(.data)
    } :text

     _bss_start = .;
    .bss : {
        *(.bss)
    } :text
    _bss_end = .;

    _end = .;
}
