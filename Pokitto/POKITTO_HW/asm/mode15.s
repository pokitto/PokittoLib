# mode15.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode15

# void updateMode15(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func updateMode15
PALETTE .req r0
SCRBUF .req r1

# locals
LCD .req r2
WRBIT .req r3
COLOR .req r4
TMP .req r5
CLR .req r6
END .req r7

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
    ldm SCRBUF!, {COLOR}
    movs TMP, 0xF0
	str WRBIT, [LCD, 124]
.endm

updateMode15:
    push {r4-r7, lr}
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    ldr END, =0x4BA0
    adds END, SCRBUF
    
    ldm SCRBUF!, {COLOR}
    mode15Loop:
    movs TMP, 0xF0
    MODE15_LOOP
    MODE15_LOOP
    MODE15_LOOP
    MODE15_ENDLOOP
    cmp SCRBUF, END
    ble mode15Loop

    pop {r4-r7, pc}
    
.pool
.endfunc
