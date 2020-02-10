# mode64c.s by FManga
# This code is from the PokittoLib and is therefore under the BSD License.

.thumb
.syntax unified
.global updateMode64Clear

# void updateMode64Clear(uint32_t* PALETTE asm("r0"), uint8_t* SCRBUF asm("r1"), int clear)
.func updateMode64Clear
PALETTE .req r0
SCRBUF .req r1

# locals
LCD .req r2
WRBIT .req r3
COLOR .req r4
TMP .req r5
CLR .req r6
END .req r7
CLEAR .req r8

updateMode64Clear:
    push {r4-r7, lr}
    mov r4, r8
    push {r4}
    
    lsls TMP, r2, 16
    orrs r2, TMP
    lsls TMP, r2, 8
    orrs r2, TMP
    
    mov CLEAR, r2
    ldr LCD,   =0xA0002188
    ldr WRBIT, =1<<12
    ldr CLR,   =252
    ldr END, =110*176
    adds END, SCRBUF
    
    ldm SCRBUF, {COLOR}
    mov TMP, CLEAR
    stm SCRBUF!, {TMP}
    lsls TMP, COLOR, 24
    mode64loop:
        lsrs COLOR, COLOR, 8
        lsrs TMP, TMP, 23
        ldrh TMP, [PALETTE, TMP]
        lsls TMP, 3
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        lsls TMP, COLOR, 24
        lsrs COLOR, COLOR, 8
        lsrs TMP, TMP, 23
        str WRBIT, [LCD, 124]
        str WRBIT, [LCD, CLR]
        ldrh TMP, [PALETTE, TMP]
        lsls TMP, 3
        str WRBIT, [LCD, 124]
        
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        lsls TMP, COLOR, 24
        lsrs TMP, TMP, 23
        str WRBIT, [LCD, 124]
        str WRBIT, [LCD, CLR]
        ldrh TMP, [PALETTE, TMP]
        lsls TMP, 3
        str WRBIT, [LCD, 124]
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        lsrs COLOR, COLOR, 8
        lsls TMP, COLOR, 1
        str WRBIT, [LCD, 124]
        str WRBIT, [LCD, CLR]
        ldrh TMP, [PALETTE, TMP]
        lsls TMP, 3
        str WRBIT, [LCD, 124]
        
        str TMP, [LCD, 0]
        str WRBIT, [LCD, CLR]
        ldm SCRBUF, {COLOR}
        mov TMP, CLEAR
        stm SCRBUF!, {TMP}
        str WRBIT, [LCD, 124]
        str WRBIT, [LCD, CLR]
        lsls TMP, COLOR, 24
        cmp SCRBUF, END
        str WRBIT, [LCD, 124]
        
    bne mode64loop

    lsrs COLOR, COLOR, 8
    lsrs TMP, TMP, 23
    ldrh TMP, [PALETTE, TMP]
    lsls TMP, 3
    str TMP, [LCD, 0]
    str WRBIT, [LCD, CLR]
    lsls TMP, COLOR, 24
    lsrs COLOR, COLOR, 8
    lsrs TMP, TMP, 23
    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, CLR]
    ldrh TMP, [PALETTE, TMP]
    lsls TMP, 3
    str WRBIT, [LCD, 124]
    
    str TMP, [LCD, 0]
    str WRBIT, [LCD, CLR]
    lsls TMP, COLOR, 24
    lsrs TMP, TMP, 23
    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, CLR]
    ldrh TMP, [PALETTE, TMP]
    lsls TMP, 3
    str WRBIT, [LCD, 124]
    str TMP, [LCD, 0]
    str WRBIT, [LCD, CLR]
    lsrs COLOR, COLOR, 8
    lsls TMP, COLOR, 1
    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, CLR]
    ldrh TMP, [PALETTE, TMP]
    lsls TMP, 3
    str WRBIT, [LCD, 124]
    
    str TMP, [LCD, 0]
    str WRBIT, [LCD, CLR]

    pop {r4}

    str WRBIT, [LCD, 124]
    str WRBIT, [LCD, CLR]

    mov r8, r4

    str WRBIT, [LCD, 124]
    
    pop {r4-r7, pc}
    
.pool
.endfunc
