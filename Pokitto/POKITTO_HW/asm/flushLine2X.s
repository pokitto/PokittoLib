# flushLine2x.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global flushLine2X

# void flushLine2X(uint16_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func flushLine2X
PALETTE .req r0
SCRBUF .req r1

# locals
LCD .req r2
WRBIT .req r3
COLOR .req r4
TMP .req r5
CLR .req r6
END .req r7

.macro UNROLL offset:req
    lsls TMP, 3
    str TMP, [LCD]
    str WRBIT, [LCD, CLR]
    ldrb TMP, [SCRBUF, \offset]
    str WRBIT, [LCD, 124]

    str WRBIT, [LCD, CLR]
    lsls TMP, 1
    ldrh TMP, [PALETTE, TMP]
    str WRBIT, [LCD, 124]
.endm

flushLine2X:
    push {r4-r7, lr}
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    ldr END, =110
    adds END, SCRBUF
    ldrb TMP, [SCRBUF]
    lsls TMP, 1

    loop:

        ldrh TMP, [PALETTE, TMP]

        UNROLL #1

        lsls TMP, 3
        str TMP, [LCD]
        str WRBIT, [LCD, CLR]
        adds SCRBUF, 2
        ldrb TMP, [SCRBUF]
        str WRBIT, [LCD, 124]

        str WRBIT, [LCD, CLR]
        lsls TMP, 1
        cmp SCRBUF, END   
        str WRBIT, [LCD, 124]
    bne loop

    pop {r4-r7, pc}
    
.pool
.endfunc
