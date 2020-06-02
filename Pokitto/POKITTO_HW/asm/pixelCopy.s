# pixelCopy.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global pixelCopy
.type   pixelCopy,%function
# void pixelCopy(uint8_t* dest, uint8_t* src, int count)
.func pixelCopy
DEST .req r0
SRC .req r1
COUNT .req r2
RECOLOR .req r3
OFFSET .req r6
TMP .req r5
TMP2 .req r4

.macro COPY
    ldrb TMP2, [SRC, OFFSET]
    cmp TMP2, 0
    beq 1f
        adds TMP2, RECOLOR
        strb TMP2, [DEST, OFFSET]
    1:
    adds OFFSET, 1
.endm

pixelCopy:
    push {r4-r6, lr}
    ldr OFFSET, =0
    
    pcLoop:
    // if( count == 0 ) return;
    cmp COUNT, 0
    beq pcEnd
    
    lsls TMP, COUNT, 32-3
    lsrs TMP, 32-3
    // if COUNT&7 == 0
    bne notEight
        movs TMP, 8
    notEight:
    // count -= (COUNT&7)
    subs COUNT, TMP

    // tmp = 8 * 12 - (COUNT&7) * 12
    movs TMP2, 12
    muls TMP, TMP2
    movs TMP2, 8*12
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
    b pcLoop
    
    pcEnd:
    pop {r4-r6, pc}
.pool
.endfunc
