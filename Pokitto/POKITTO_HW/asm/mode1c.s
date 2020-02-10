# mode1c.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode1Clear

# void updateMode1Clear(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"), int clearColor asm("r2"))
.func updateMode1Clear
PALETTE .req r0
SCRBUF .req r1
CLEAR  .req r9

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

updateMode1Clear:
    push {r4-r7, lr}
    mov r4, r8
    mov r5, r9
    push {r4, r5}
    
    lsls tmp, r2, 16
    orrs r2, tmp
    lsls tmp, r2, 8
    orrs r2, tmp
    lsls tmp, r2, 4
    orrs r2, tmp
    lsls tmp, r2, 2
    orrs r2, tmp
    mov CLEAR, r2
    
    ldr r4, =0x25D4
    adds r4, SCRBUF
    mov END, r4
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    
    mov TMP, CLEAR
    ldm SCRBUF, {COLOR}     // load 4 bytes (16 pixels)
    mode1Loop:
        stm SCRBUF!, {TMP}      // clear 16 pixels
        movs TMP, 0xF0
        ands TMP, TMP, COLOR
        lsrs TMP, TMP, 1
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
        ldm SCRBUF, {COLOR} // load next 4 bytes
        str WRBIT, [LCD, 124]
        str PAIR, [LCD, 0]
        str WRBIT, [LCD, CLR]
        mov TMP, CLEAR
        cmp END, SCRBUF
        str WRBIT, [LCD, 124]
    bne mode1Loop // if scrbuf < end, loop

    pop {r4, r5}
    mov r8, r4
    mov r9, r5
    pop {r4-r7, pc}
    
.pool
.endfunc
