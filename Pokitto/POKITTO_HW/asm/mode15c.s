# mode15.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode15Clear

# void updateMode15(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func updateMode15Clear
PALETTE .req r0
SCRBUF .req r1
CLEAR .req r2

# locals
LCD .req r3
WRBIT .req r4
COLOR .req r5
TMP .req r6
CLR .req r7
END .req r8

.macro MODE15_LOOP
    ands TMP, COLOR
    lsrs TMP, 2
    ldr TMP, [PALETTE, TMP]
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    lsls TMP, COLOR, 28
    str WRBIT, [LCD, 124]
    lsrs TMP, 26
    ldr TMP, [PALETTE, TMP]
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    movs TMP, 0xF0
    lsrs COLOR, 8
    str WRBIT, [LCD, 124]
.endm

.macro MODE15_ENDLOOP
    ands TMP, COLOR
    lsrs TMP, 2
    ldr TMP, [PALETTE, TMP]
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    lsls TMP, COLOR, 28
    str WRBIT, [LCD, 124]
    lsrs TMP, 26
    ldr TMP, [PALETTE, TMP]
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    ldm SCRBUF, {COLOR}
    movs TMP, 0xF0
	str WRBIT, [LCD, 124]
.endm

updateMode15Clear:
    push {r4-r7, lr}
    mov r4, r8
    push {r4}
    
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    ldr TMP, =0x4BA0
    add TMP, SCRBUF
    mov END, TMP
    
    lsls TMP, CLEAR, 16
    orrs CLEAR, TMP
    lsls TMP, CLEAR, 8
    orrs CLEAR, TMP
    lsls TMP, CLEAR, 4
    orrs CLEAR, TMP
    
    ldm SCRBUF, {COLOR}
    mode15Loop:
    mov TMP, CLEAR
    stm SCRBUF!, {TMP}
    movs TMP, 0xF0
    MODE15_LOOP
    MODE15_LOOP
    MODE15_LOOP
    MODE15_ENDLOOP
    cmp SCRBUF, END
    blt mode15Loop

    pop {r4}
    mov r8, r4
    pop {r4-r7, pc}
    
.pool
.endfunc
