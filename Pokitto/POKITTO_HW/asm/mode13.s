# mode13.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode13

# void updateMode13(uint16_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func updateMode13
PALETTE .req r0
SCRBUF .req r1
OFFSET .req r9

# locals
WRBIT .req r2
LCD .req r3
C .req r4
T .req r5
X .req r6
Y .req r8
SCANLINE .req r7

.macro MODE13_INNER_LOOP_A
    add T, OFFSET
    uxtb C, T 
    lsls C, 1
    ldrh T, [PALETTE, C]
    lsls T, 3
    str T, [LCD]
    movs C, 252
    str WRBIT, [LCD, C]
    stm SCANLINE!, {T}
    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, C]
    adds SCRBUF, 1
    ldrb T, [SCRBUF]
    str WRBIT, [LCD, 124]
.endm

.macro MODE13_INNER_LOOP_B
    // This can be made 1 cycle faster (x -= 10 instead of x--), but there will be noise
    str C, [LCD]
    str WRBIT, [LCD, T]
    ldr C, [SCANLINE]
    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, T]
    adds SCANLINE, 4
    subs X, 1
    str WRBIT, [LCD, 124]
.endm

updateMode13:
    push {r4-r7, lr}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    push {r4-r6}
    sub sp, 110*4

    movs LCD, 0
    mov Y, LCD
    ldr LCD,   =0xA0002188
    mov OFFSET, r2
    ldr WRBIT, =1<<12
    mov SCANLINE, sp
    
    mode13OuterLoop:
    
        movs X, 110
        ldrb T, [SCRBUF]
        mode13InnerLoopA:
        	 MODE13_INNER_LOOP_A
        	 MODE13_INNER_LOOP_A
        subs X, 2
        bne mode13InnerLoopA
        
        mov SCANLINE, sp
        
        movs X, 110
        movs T, 252
        ldm SCANLINE!, {C}
        mode13InnerLoopB:
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        	 MODE13_INNER_LOOP_B
        bne mode13InnerLoopB
        
        mov SCANLINE, sp
        movs T, 1
        movs C, 88
        add Y, T
        cmp Y, C
    blt mode13OuterLoop

    add sp, 110*4
    pop {r4-r6}
    mov r8, r4
    mov r9, r5
    mov r10, r6
    pop {r4-r7, pc}
    
.pool
.endfunc
