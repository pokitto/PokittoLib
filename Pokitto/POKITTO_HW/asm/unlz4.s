.syntax             unified
.cpu                cortex-m0
.thumb

.func               unlz4
.global             unlz4, unlz4_len
.type               unlz4, %function
.type               unlz4_len, %function
        
CData0          .req r0
DestData1       .req r1
CopySrc         .req r2
Tmp3            .req r3
CopyLen4        .req r4
EndCData        .req r5
Token6          .req r6

/*
Block Format:
[Token byte]
[Literal length]
[Literal data]
[Match offset]
[Match length]
*/
        
.thumb_func
unlz4:
	ldrb r2, [CData0]             /* get length of compressed data */
	ldrb Tmp3, [CData0, #1]
	lsls Tmp3, #8
	adds r2, Tmp3
	adds CData0, #2            /* advance source pointer */
        
.thumb_func
unlz4_len:
        push {r4-r6, lr}          /* save r4,  EndCData,  r6 and return-address */
        
        adds EndCData, r2, CData0            /* point EndCData to end of compressed data */
        
getToken:
        ldrb Token6, [CData0]             /* get token */
        adds CData0, #1            /* advance source pointer */
        lsrs CopyLen4, Token6, #4            /* get literal length,  keep token in r6 */
        beq  getOffset           /* jump forward if there are no literals */
        bl   getLength           /* get length of literals */
        movs CopySrc, CData0               /* point r2 to literals */
        bl   copyData            /* copy literals (r2=src,  DestData1=dst,  r4=len) */
        movs CData0, CopySrc               /* update source pointer */

getOffset:
        subs  Tmp3, EndCData, CData0
        cmp  Tmp3, 1
        ble  done
        ldrb Tmp3, [CData0, #0]          /* get match offset's low byte */
        subs CopySrc, DestData1, Tmp3            /* subtract from destination; this will become the match position */
        ldrb Tmp3, [CData0, #1]          /* get match offset's high byte */
        lsls Tmp3, #8            /* shift to high byte */
        subs CopySrc, Tmp3            /* subtract from match position */
        adds CData0, #2            /* advance source pointer */
        lsls CopyLen4, Token6, #28           /* get rid of token's high 28 bits */
        lsrs CopyLen4, #28           /* move the 4 low bits back where they were */
        bl   getLength           /* get length of match data */
        adds CopyLen4, #4            /* minimum match length is 4 bytes */
        bl   copyData            /* copy match data (r2=src,  DestData1=dst,  r4=len) */
        cmp  CData0, EndCData               /* check if we've reached the end of the compressed data */
        blt  getToken            /* if not,  go get the next token */
done:   
        pop  {r4-r6, pc}          /* restore r4,  EndCData and r6,  then return */
        
.thumb_func
getLength:
        cmp  CopyLen4, #0x0f            /* if length is 15,  then more length info follows */
        bne  gotLength           /* jump forward if we have the complete length */
getLengthLoop:
        ldrb Tmp3, [CData0]             /* read another byte */
        adds CData0, #1            /* advance source pointer */
        adds CopyLen4, Tmp3            /* add byte to length */
        cmp  Tmp3, #0xff            /* check if end reached */
        beq  getLengthLoop       /* if not,  go round loop */
gotLength:
        bx   lr                  /* return */
        
.thumb_func
copyData:
        rsbs CopyLen4, #0            /* index = -length */
        subs CopySrc, CopyLen4            /* point to end of source */
        subs DestData1, CopyLen4            /* point to end of destination */
        
copyDataLoop:
        ldrb Tmp3, [CopySrc, CopyLen4]          /* read byte from source_end[-index] */
        strb Tmp3, [DestData1, CopyLen4]          /* store byte in destination_end[-index] */
        adds CopyLen4, #1            /* increment index */
        bne  copyDataLoop        /* keep going until index wraps to 0 */
        bx   lr                  /* return */
        
.size               unlz4, .-unlz4
        
.endfunc
