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

	.text
	.arm

	.set	SYS_MODE, 0x1f
	.set	SVC_MODE, 0x13
	.set	IRQ_MODE, 0x12

	.extern	ulICCPMR

.macro portSAVE_CONTEXT
	/* Save the LR and SPSR onto the system mode stack
	 * before switching to system mode to save the remaining
	 * system mode registers.
	 */
	srsdb	sp!, #SYS_MODE
	cps	#SYS_MODE
	push	{r0-r12, r14}

	/* Push the critical nesting count. */
	ldr	r2, ulCriticalNestingConst
	ldr	r1, [r2]
	push	{r1}

	/* Does the task have a floating point context that needs saving?
	 * If ulPortTaskHasFPUContext is 0 then no.
	 */
	ldr	r2, ulPortTaskHasFPUContextConst
	ldr	r3, [r2]
	cmp	r3, #0

	/* Save the floating point context, if any. */
	fmrxne	r1, fpscr
	vpushne	{d0-d15}
	vpushne	{d16-d31}
	pushne	{r1}

	/* Save ulPortTaskHasFPUContext itself. */
	push	{r3}

	/* Save the stack pointer in the TCB. */
	ldr	r0, pxCurrentTCBConst
	ldr	r1, [r0]
	str	sp, [r1]

	.endm

.macro portRESTORE_CONTEXT
	/* Set the SP to point to the stack of the task being restored. */
	ldr	r0, pxCurrentTCBConst
	ldr	r1, [r0]
	ldr	sp, [r1]

	/* Is there a floating point context to restore?  If the restored
	 * ulPortTaskHasFPUContext is zero then no.
	 */
	ldr	r0, ulPortTaskHasFPUContextConst
	pop	{r1}
	str	r1, [r0]
	cmp	r1, #0

	/* Restore the floating point context, if any. */
	popne	{r0}
	vpopne	{d16-d31}
	vpopne	{d0-d15}
	vmsrne	fpscr, r0

        /* Restore the critical section nesting depth. */
	ldr	r0, ulCriticalNestingConst
	pop	{r1}
	str	r1, [r0]

	/* Ensure the priority mask is correct for the critical
	 * nesting depth.
	 */
	ldr	r2, ulICCPMRConst
	ldr	r2, [r2]
	cmp	r1, #0
	moveq	r4, #255
	ldrne	r4, ulMaxAPIPriorityMaskConst
	ldrne	r4, [r4]
	str	r4, [r2]

	/* Restore all system mode registers other than the SP
	 * (which is already being used).
	 */
	pop	{r0-r12, r14}

	/* Return to the task code, loading CPSR on the way. */
	rfeia	sp!

	.endm

	.global vPortRestoreTaskContext
	.type vPortRestoreTaskContext, %function
vPortRestoreTaskContext:
	/* Switch to system mode. */
	cps	#SYS_MODE
	portRESTORE_CONTEXT

	.align	4
	.global	vector
vector:						/* ARM exceptions vector */
	ldr	pc, =handler
	ldr	pc, =undefined_instruction
	ldr	pc, =svc_handler
	ldr	pc, =prefetch_abort_handler
	ldr	pc, =data_abort_handler
	ldr	pc, =handler
	ldr	pc, =irq_handler
	ldr	pc, =firq_handler

	.align	2
svc_handler:
	/* Save the context of the current task and select a new task to run */
	portSAVE_CONTEXT
	ldr	r0, =vTaskSwitchContext
	blx	r0
	portRESTORE_CONTEXT

	.align 2
undefined_instruction:
	b	.

	.align 2
firq_handler:
	b	.

	.align	2
data_abort_handler:				/* Data abort handler */
	b	do_data_abort

	.align	2
prefetch_abort_handler:				/* Prefetch abort handler */
	b	do_prefetch_abort

	.align	2
irq_handler:
	cpsid	i

	sub	lr, lr, #4
	push	{lr}

	mrs	lr, spsr
	push	{lr}

	cps	#SVC_MODE
	push    {r0-r4, r12}

	/* Increment nesting count.  r3 holds the address of
	 * ulPortInterruptNesting for future use.  r1 holds the
	 * original ulPortInterruptNesting value for future use.
	 */
	ldr	r3, ulPortInterruptNestingConst
	ldr	r1, [r3]
	add	r4, r1, #1
	str	r4, [r3]

        /* Ensure bit 2 of the stack pointer is clear.
	 * r2 holds the bit 2 value for future use.
	 */
	mov	r2, sp
	and	r2, r2, #4
	sub	sp, sp, r2

	/* Call the interrupt handler. */
	push	{r0-r3, lr}
	ldr	r1, vApplicationIRQHandlerConst
	blx	r1
	pop	{r0-r3, lr}
	add	sp, sp, r2

	/* Restore the old nesting count. */
	str	r1, [r3]

	/* A context switch is never performed if the nesting count
	 * is not 0.
	 */
	cmp	r1, #0
	bne	exit_without_switch

	/* Did the interrupt request a context switch?
	 * r1 holds the address of ulPortYieldRequired and r0 the
	 * value of ulPortYieldRequired for futureu use.
	 */
	ldr	r1, =ulPortYieldRequired
	ldr	r0, [r1]
	cmp	r0, #0
	bne	switch_before_exit

exit_without_switch:
	/* No context switch.  Restore used registers, LR_irq
	 * and SPSR before returning.
	 */
	pop	{r0-r4, r12}
	cps	#IRQ_MODE
	pop	{lr}
	msr	spsr_cxsf, lr
	pop	{lr}
	movs	pc, lr

switch_before_exit:
	/* A context swtich is to be performed.
	 * Clear the context switch pending flag.
	 */
	mov	r0, #0
	str	r0, [r1]

	/* Restore used registers, LR-irq and SPSR before
	 * saving the context to the task stack.
	 */
	pop	{r0-r4, r12}
	cps	#IRQ_MODE
	pop	{lr}
	msr	spsr_cxsf, lr
	pop	{lr}
	portSAVE_CONTEXT

	/* Call the function that selects the new task to execute.
	 * vTaskSwitchContext() if vTaskSwitchContext() uses LDRD or STRD
	 * instructions, or 8 byte aligned stack allocated data.  LR does
	 * not need saving as a new LR will be loaded by
	 * portRESTORE_CONTEXT anyway.
	 */
	ldr	r0, vTaskSwitchContextConst
	blx	r0

	/* Restore the context of, and branch to, the task
	 * selected to execute next.
	 */
	portRESTORE_CONTEXT

	.align	2
handler:					/* Default busy loop handler */
	b	.

ulICCPMRConst:			.word ulICCPMR
pxCurrentTCBConst:		.word pxCurrentTCB
ulCriticalNestingConst:		.word ulCriticalNesting
vTaskSwitchContextConst:	.word vTaskSwitchContext
ulMaxAPIPriorityMaskConst:	.word ulMaxAPIPriorityMask
vApplicationIRQHandlerConst:	.word do_irq
ulPortInterruptNestingConst:	.word ulPortInterruptNesting
ulPortTaskHasFPUContextConst:	.word ulPortTaskHasFPUContext
