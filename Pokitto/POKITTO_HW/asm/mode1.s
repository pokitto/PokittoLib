# mode1.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode1

# void updateMode1(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"))
.func updateMode1
PALETTE .req r0
SCRBUF .req r1

# locals
LCD .req r2
WRBIT .req r3
COLOR .req r4
TMP .req r5
PAIR .req r6
CLR .req r7
END .req r8

.macro MODE1_LOOP
    adds TMP, PALETTE
    ldm TMP, {TMP, PAIR}
    str TMP, [LCD, 0]
    str WRBIT, [LCD, CLR]
    str WRBIT, [LCD, 124]
    str PAIR, [LCD, 0]
    str WRBIT, [LCD, CLR]
    movs TMP, 0x0F
    ands TMP, COLOR
    str WRBIT, [LCD, 124]
    
    lsls TMP, 3
    adds TMP, PALETTE
    ldm TMP, {TMP, PAIR}
    str TMP, [LCD, 0]
    movs TMP, 252
    str WRBIT, [LCD, TMP]
    str WRBIT, [LCD, 124]
    str PAIR, [LCD, 0]
    str WRBIT, [LCD, TMP]
    lsrs COLOR, 8
    movs TMP, 0xF0
    ands TMP, TMP, COLOR
    lsrs TMP, TMP, 1
    str WRBIT, [LCD, 124]
.endm

updateMode1:
    push {r4-r7, lr}
    mov r4, r8
    push {r4}
    
    ldr r4, =0x25D4
    adds r4, SCRBUF
    mov END, r4
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    
    ldm SCRBUF!, {COLOR}     // load 4 bytes (16 pixels)
    movs TMP, 0xF0
    ands TMP, COLOR
    lsrs TMP, 1
    mode1Loop:
        MODE1_LOOP
        MODE1_LOOP
        MODE1_LOOP
        adds TMP, PALETTE
        ldm TMP, {TMP, PAIR}
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        movs TMP, 0x0F
        str WRBIT, [LCD, 124]
        str PAIR, [LCD, 0]
        str WRBIT, [LCD, CLR]
        ands TMP, TMP, COLOR
        str WRBIT, [LCD, 124]
    
        lsls TMP, 3
        adds TMP, PALETTE
        ldm TMP, {TMP, PAIR}
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        movs TMP, 0xF0
        str WRBIT, [LCD, 124]
        str PAIR, [LCD, 0]
        str WRBIT, [LCD, CLR]
    
        ldm SCRBUF!, {COLOR} // load next 4 bytes
        ands TMP, TMP, COLOR
        lsrs TMP, TMP, 1
        str WRBIT, [LCD, 124]

        cmp END, SCRBUF
    bne mode1Loop // if scrbuf < end, loop

    pop {r4}
    mov r8, r4
    pop {r4-r7, pc}
    
.pool
.endfunc
