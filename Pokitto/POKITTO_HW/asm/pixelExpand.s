# pixelExpand.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global pixelExpand
.type   pixelExpand,%function
# void pixelExpand(uint8_t* dest, uint8_t* src, int count, int recolor, int stride)
.func pixelExpand
DEST .req r0
SRC .req r1
COUNT .req r2
RECOLOR .req r3
TMP .req r5
TMP2 .req r4
STRIDE .req r6

.macro COPY OFFSET:req
        lsls TMP2, TMP, 31
        beq 1f
        strb RECOLOR, [DEST, \OFFSET]
1:      lsrs TMP, 1
.endm

pixelExpand:
    push {r4-r6, lr}
    ldr STRIDE, [sp, 16]
    cmp STRIDE, 0
    blt 3f
    beq pcEnd

    2:
        subs COUNT, 1
        blt pcEnd

        ldrb TMP, [SRC]
        adds SRC, 1

        COPY 7
        COPY 6
        COPY 5
        COPY 4
        COPY 3
        COPY 2
        COPY 1
        COPY 0

        adds DEST, STRIDE
    b 2b

    3:
        subs COUNT, 1
        blt pcEnd

        ldrb TMP, [SRC]
        adds SRC, 1

        COPY 0
        COPY 1
        COPY 2
        COPY 3
        COPY 4
        COPY 5
        COPY 6
        COPY 7

        adds DEST, STRIDE
    b 3b

    pcEnd:
    pop {r4-r6, pc}
.pool
.endfunc
