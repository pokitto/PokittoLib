# flushLine.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global flushLine

# void flushLine(uint16_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func flushLine
PALETTE .req r0
SCRBUF .req r1

# locals
LCD .req r2
WRBIT .req r3
COLOR .req r4
TMP .req r5
CLR .req r6
END .req r7

.macro UNROLL
    lsls TMP, 1
    ldrh TMP, [PALETTE, TMP]
    lsls TMP, 3
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    lsrs COLOR, 8
    uxtb TMP, COLOR
    str WRBIT, [LCD, 124]
.endm

flushLine:
    push {r4-r7, lr}
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    ldr END, =220
    adds END, SCRBUF
    
    ldm SCRBUF!, {COLOR}
    loop:

        uxtb TMP, COLOR
        UNROLL
        UNROLL

        lsls TMP, 1
        ldrh TMP, [PALETTE, TMP]
        lsls TMP, 3
        str TMP, [LCD]
        str WRBIT, [LCD, CLR]
        lsrs COLOR, 8
        lsls COLOR, 1
        ldrh TMP, [PALETTE, COLOR]
        str WRBIT, [LCD, 124]
    
        lsls TMP, 3
        str TMP, [LCD]
        str WRBIT, [LCD, CLR]
        cmp SCRBUF, END   
        ldm SCRBUF!, {COLOR}
        str WRBIT, [LCD, 124]

    bne loop

    pop {r4-r7, pc}
    
.pool
.endfunc
