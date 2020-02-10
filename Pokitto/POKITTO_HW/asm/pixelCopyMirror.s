# pixelCopyMirror.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global pixelCopyMirror
.type   pixelCopyMirror,%function
# void pixelCopyMirror(uint8_t* dest, uint8_t* src, int count)
.func pixelCopyMirror
DEST .req r0
SRC .req r1
COUNT .req r2
RECOLOR .req r3
TMP .req r5
TMP2 .req r4

.macro COPY
    subs SRC, 1
    ldrb TMP2, [SRC]
    cmp TMP2, 0
    beq 1f
        adds TMP2, RECOLOR
        strb TMP2, [DEST]
    1:
    adds DEST, 1
.endm

pixelCopyMirror:
    push {r4-r6, lr}
    adds SRC, COUNT
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

    // tmp = 8 * 7 - (COUNT&7) * 7
    movs TMP2, 7*2
    muls TMP, TMP2
    movs TMP2, 8*7*2
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
