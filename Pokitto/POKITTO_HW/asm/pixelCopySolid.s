# pixelCopySolid.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global pixelCopySolid
.type   pixelCopySolid,%function
# void pixelCopySolid(uint8_t* dest, uint8_t* src, int count)
.func pixelCopySolid
DEST .req r0
SRC .req r1
COUNT .req r2
RECOLOR .req r3
OFFSET .req r6
TMP .req r5
TMP2 .req r4

.macro COPY
    ldrb TMP2, [SRC, OFFSET]
    adds TMP2, RECOLOR
    strb TMP2, [DEST, OFFSET]
    adds OFFSET, 1
.endm

pixelCopySolid:
    push {r4-r6, lr}
    ldr OFFSET, =0
    
    pcLoop:
    // if( count == 0 ) return;
    cmp COUNT, 0
    beq pcEnd
    
    lsls TMP, COUNT, 32-4
    lsrs TMP, 32-4
    // if COUNT&0xF == 0
    bne notEight
        movs TMP, 16
    notEight:
    // count -= (COUNT&0xF)
    subs COUNT, TMP

    // tmp = 16 * 8 - (COUNT&0xF) * 8
    lsls TMP, 3
    movs TMP2, 16 * 8
    subs TMP, TMP2, TMP
    
    // tmp += pcStart
    ldr TMP2, =pcStart+1
    adds TMP, TMP2
    bx TMP
    pcStart:
        COPY
        COPY
        COPY
        COPY
        COPY
        COPY
        COPY
        COPY

        COPY
        COPY
        COPY
        COPY
        COPY
        COPY
        COPY
        COPY
    b pcLoop
    
    pcEnd:
    pop {r4-r6, pc}
.pool
.endfunc
