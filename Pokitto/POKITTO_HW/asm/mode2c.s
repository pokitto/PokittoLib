# mode2c.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode2Clear

# void updateMode2Clear(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"), int clear)
.func updateMode2Clear
PALETTE .req r0
SCRBUF .req r1

# locals
LCD         .req r2
C           .req r3
T           .req r4
X           .req r5
SCANLINE    .req r6
byte        .req r7
Y           .req r8
WRBIT       .req r9
CLEAR       .req r10

.macro MODE2_LOOP
    ldm SCANLINE!, {C}
    str C, [LCD]
    str T, [LCD, 124]
    movs C, 252
    str T, [LCD, C]
    str T, [LCD, 124]
    subs X, 1
    str T, [LCD, C]
.endm

updateMode2Clear:
    push {r4-r7, lr}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    push {r4-r6}
    sub sp, 110*4
    
    lsls T, r2, 4
    orrs r2, T
    mov CLEAR, r2
    
    mov SCANLINE, sp
    ldr T, =1<<12
    ldr LCD, =0xA0002188
    mov WRBIT, T
    ldr T, =0
    mov Y, T
    
    mov C, WRBIT
    movs T, 252
    str C, [LCD, T]
    
    mode2OuterLoop:

        movs X, 110
        mode2InnerLoopA:

            ldrb BYTE, [SCRBUF]
            mov T, CLEAR
            strb T, [SCRBUF]
            
            movs T, 15
            lsrs C, BYTE, 4
            ands BYTE, T
        
            lsls C, 1
            ldrh T, [PALETTE, C]
            lsls T, 3
            str T, [LCD]
            mov C, WRBIT
            str C, [LCD, 124]
            stm SCANLINE!, {T}
            movs T, 252
            str C, [LCD, T]
            str C, [LCD, 124]
            lsls BYTE, 1
            str C, [LCD, T]
            
            ldrh T, [PALETTE, BYTE]
            lsls T, 3
            str T, [LCD]
            mov C, WRBIT
            str C, [LCD, 124]
            stm SCANLINE!, {T}
            movs T, 252
            str C, [LCD, T]
            str C, [LCD, 124]
            adds SCRBUF, 1
            str C, [LCD, T]
            
            subs X, 2
        bne mode2InnerLoopA
    
        mov SCANLINE, sp
        movs X, 110
        mov T, WRBIT
        mode2InnerLoopB:
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
            MODE2_LOOP
        bne mode2InnerLoopB
    
        mov SCANLINE, sp
        movs T, 1
        movs C, 88
        add Y, T
        cmp Y, C
    bne mode2OuterLoop

    add sp, 110*4
    pop {r4-r6}
    mov r8, r4
    mov r9, r5
    mov r10, r6
    pop {r4-r7, pc}
    
.pool
.endfunc
