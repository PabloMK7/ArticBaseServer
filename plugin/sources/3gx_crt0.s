/*--------------------------------------------------------------------------------
	This Source Code Form is subject to the terms of the Mozilla Public License,
	v. 2.0. If a copy of the MPL was not distributed with this file, You can
	obtain one at https://mozilla.org/MPL/2.0/.
--------------------------------------------------------------------------------*/

@---------------------------------------------------------------------------------
@ 3DS processor selection
@---------------------------------------------------------------------------------
	.cpu mpcore
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
	.section ".crt0","ax"
	.global _start, __service_ptr, __apt_appid, __ctru_heap_size, __ctru_linear_heap_size, __system_arglist, __system_runflags
@---------------------------------------------------------------------------------
	.align 2
	.arm
@---------------------------------------------------------------------------------
_start:
@---------------------------------------------------------------------------------
	b startup
	.ascii "_prm"
__service_ptr:
	.word 0 @ Pointer to service handle override list -- if non-NULL it is assumed that we have been launched from a homebrew launcher
__apt_appid:
	.word 0x300 @ Program APPID
__ctru_heap_size:
	.word 24*1024*1024 @ Default heap size (24 MiB)
__ctru_linear_heap_size:
	.word 24*1024*1024 @ Default linear heap size (24 MiB)
__system_arglist:
	.word 0 @ Pointer to argument list (argc (u32) followed by that many NULL terminated strings)
__system_runflags:
	.word 0 @ Flags to signal runtime restrictions to ctrulib
startup:
	@ Restore plugin loader state, including the pushed stack registers
	add		sp, sp, #4
	ldmfd   sp!, {r0}
    msr     cpsr, r0
    ldmfd   sp!, {r0-r12}
	mov		lr, #0

	@ Save return address
	mov r4, lr

	@ Clear the BSS section
	ldr r0, =__bss_start__
	ldr r1, =__bss_end__
	sub r1, r1, r0
	bl  ClearMem

	@ System initialization
	mov r0, r4
	bl initSystem

	@ Set up argc/argv arguments for main()
	ldr r0, =__system_argc
	ldr r1, =__system_argv
	ldr r0, [r0]
	ldr r1, [r1]

	@ Jump to user code
	ldr r3, =main
	ldr lr, =exit
	bx  r3

@---------------------------------------------------------------------------------
@ Clear memory to 0x00 if length != 0
@  r0 = Start Address
@  r1 = Length
@---------------------------------------------------------------------------------
ClearMem:
@---------------------------------------------------------------------------------
	mov  r2, #3     @ Round down to nearest word boundary
	add  r1, r1, r2 @ Shouldn't be needed
	bics r1, r1, r2	@ Clear 2 LSB (and set Z)
	bxeq lr         @ Quit if copy size is 0

	mov	r2, #0
ClrLoop:
	stmia r0!, {r2}
	subs  r1, r1, #4
	bne   ClrLoop

	bx lr