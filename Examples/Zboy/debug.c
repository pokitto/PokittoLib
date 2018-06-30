/*
    Some debuging functions...
*/

/*
DIM SHARED DebugLastPaletteDump AS UINTEGER
DIM SHARED DebugNextCpuSpeedDump AS ULONGINT
DIM SHARED DebugLastTextRow AS INTEGER
DIM SHARED DebugLogHistory(1 TO 150) AS STRING


SUB DebugShowCPUspeed()
  DIM AS SINGLE CpuSpeed
  IF TotalCycles > DebugNextCpuSpeedDump THEN
    DebugNextCpuSpeedDump += 4100000
    CpuSpeed = (TotalCycles / (TIMER - EmulationStartTimer)) / 1000000   ' MHz
    LOCATE 43, 2: PRINT FORMAT(CpuSpeed, "0.00"); " MHz (" & INT((CpuSpeed / 4.194304) * 100) & "%)  ";
    'LOCATE 44, 2: PRINT "DropCycles: " & DropCycles;
  END IF
END SUB


SUB InitDebug()
  LOCATE 47, 2: PRINT "Palettes:";
  LOCATE 48, 2: PRINT "BGP:";
  LOCATE 50, 2: PRINT "OB0:";
  LOCATE 52, 2: PRINT "OB1:";
  LOCATE 60, 2: PRINT "Sprite OBJ table #1 [&h8000..&h8FFF]:";
  LOCATE 82, 2: PRINT "Sprite OBJ table #2 [&h8800..&h97FF]:";
END SUB


SUB DebugSpriteDump(TileTableType AS BYTE)
  DIM AS INTEGER x, y, z, t, u, globalYpos, TableAddr
  DIM SpriteBuff(0 TO 127) AS UBYTE
  DIM AS UBYTE UbyteBuff1, UbyteBuff2
  SELECT CASE TileTableType
    CASE 1 ' Tile Table #1 (8000..8FFF)
      TableAddr = &h8000
    CASE 2 ' Tile Table #2 (8800..97FF)
      TableAddr = &h8800
    CASE ELSE
      PRINT "Error #957" : END
  END SELECT
    FOR z = 0 TO 255
      IF z >= &h00 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 000 : t = z - &h00
      IF z >= &h20 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 020 : t = z - &h20
      IF z >= &h40 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 040 : t = z - &h40
      IF z >= &h60 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 060 : t = z - &h60
      IF z >= &h80 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 080 : t = z - &h80
      IF z >= &hA0 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 100 : t = z - &hA0
      IF z >= &hC0 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 120 : t = z - &hC0
      IF z >= &hE0 THEN globalYpos = 490 + ((TileTableType - 1) * 170) + 140 : t = z - &hE0

      FOR u = 0 TO 15 STEP 2
        UbyteBuff1 = VideoRAM(TableAddr + (z * 16) + u)
        UbyteBuff2 = VideoRAM(TableAddr + (z * 16) + u + 1)
        SpriteBuff((u * 4) + 7) =  ((UbyteBuff2 AND &b00000001) OR ((UbyteBuff1 AND &b00000001) SHL 1))
        SpriteBuff((u * 4) + 6) = (((UbyteBuff2 AND &b00000010) SHR 1) OR (((UbyteBuff1 AND &b00000010) SHR 1) SHL 1))
        SpriteBuff((u * 4) + 5) = (((UbyteBuff2 AND &b00000100) SHR 2) OR (((UbyteBuff1 AND &b00000100) SHR 2) SHL 1))
        SpriteBuff((u * 4) + 4) = (((UbyteBuff2 AND &b00001000) SHR 3) OR (((UbyteBuff1 AND &b00001000) SHR 3) SHL 1))
        SpriteBuff((u * 4) + 3) = (((UbyteBuff2 AND &b00010000) SHR 4) OR (((UbyteBuff1 AND &b00010000) SHR 4) SHL 1))
        SpriteBuff((u * 4) + 2) = (((UbyteBuff2 AND &b00100000) SHR 5) OR (((UbyteBuff1 AND &b00100000) SHR 5) SHL 1))
        SpriteBuff((u * 4) + 1) = (((UbyteBuff2 AND &b01000000) SHR 6) OR (((UbyteBuff1 AND &b01000000) SHR 6) SHL 1))
        SpriteBuff((u * 4) + 0) = (((UbyteBuff2 AND &b10000000) SHR 7) OR (((UbyteBuff1 AND &b10000000) SHR 7) SHL 1))
      NEXT u

 '     FOR y = 0 TO 7
 '       FOR x = 0 TO 7
 '         PSET (x + (t*9) + 10, y + globalYpos), ScreenPalette(GbPalette(SpriteBuff(y*8 + x), 1))
 '       NEXT x
 '     NEXT y
      'FOR y = 0 TO 7
      '  FOR x = 0 TO 1
      '    PSET (0 + (x * 4) + (t*9), y + globalYpos), ScreenPalette(GbPalette((MemoryRead(TableAddr + (z * 16) + (y * 2) + x) AND &b11000000) SHR 6, 1))
      '    PSET (1 + (x * 4) + (t*9), y + globalYpos), ScreenPalette(GbPalette((MemoryRead(TableAddr + (z * 16) + (y * 2) + x) AND &b00110000) SHR 4, 1))
      '    PSET (2 + (x * 4) + (t*9), y + globalYpos), ScreenPalette(GbPalette((MemoryRead(TableAddr + (z * 16) + (y * 2) + x) AND &b00001100) SHR 2, 1))
      '    PSET (3 + (x * 4) + (t*9), y + globalYpos), ScreenPalette(GbPalette(MemoryRead(TableAddr + (z * 16) + (y * 2) + x) AND &b00000011, 1))
      '  NEXT x
      'NEXT y

    NEXT z
END SUB


SUB DebugOamDump()
  REM  Dumps the content of the OAM (Object Attribute Map at FE00) on screen
END SUB


SUB DebugPaletteDump()
 ' IF INT(DebugLastPaletteDump) <> INT(TIMER) THEN
 '   DIM tPalette(0 TO 3) AS UINTEGER
 '   DIM AS INTEGER x
 '   DebugLastPaletteDump = TIMER
 '   FOR x = 0 TO 2
 '     tPalette(0) = ScreenPalette(IoRegisters(&hFF47 + x) AND &b00000011)
 '     tPalette(1) = ScreenPalette((IoRegisters(&hFF47 + x) AND &b00001100) SHR 2)
 '     tPalette(2) = ScreenPalette((IoRegisters(&hFF47 + x) AND &b00110000) SHR 4)
 '     tPalette(3) = ScreenPalette((IoRegisters(&hFF47 + x) AND &b11000000) SHR 6)
 '     LINE (40, 380 + x * 15)-STEP(10,10), tPalette(0), BF
 '     LINE (52, 380 + x * 15)-STEP(10,10), tPalette(1), BF
 '     LINE (64, 380 + x * 15)-STEP(10,10), tPalette(2), BF
 '     LINE (76, 380 + x * 15)-STEP(10,10), tPalette(3), BF
 '   NEXT x
 ' END IF
END SUB


SUB PrintDebug(BYREF DebugLog AS STRING)
  #IFDEF DEBUGMODE
    DebugLog = HEX(Register.PC, 4) & ": " & DebugLog
    DebugLastTextRow += 1
    IF DebugLastTextRow > 110 THEN DebugLastTextRow = 1
    DebugLogHistory(DebugLastTextRow) = LEFT(DebugLog + "                                        ", 40)
    #IFDEF DEBUGTRACEFILE
      OPEN "trace.txt" FOR APPEND AS #78
      PRINT #78, DebugLog
      CLOSE #78
    #ENDIF
  #ENDIF
END SUB


SUB DebugOnscreen()
  DIM AS INTEGER y
  FOR y = 1 TO 110
    LOCATE y, 50
    PRINT DebugLogHistory(y)
  NEXT y
  LOCATE DebugLastTextRow+1, 50: PRINT "--------------------------------"
END SUB


SUB DebugShowCpuRegisters()
  LOCATE 37, 20: PRINT "A:  "; HEX(Register.A, 2);  ' DEBUG
  LOCATE 38, 20: PRINT "B:  "; HEX(Register.B, 2);  ' DEBUG
  LOCATE 39, 20: PRINT "C:  "; HEX(Register.C, 2);  ' DEBUG
  LOCATE 40, 20: PRINT "D:  "; HEX(Register.D, 2);  ' DEBUG
  LOCATE 41, 20: PRINT "E:  "; HEX(Register.E, 2);  ' DEBUG
  LOCATE 42, 20: PRINT "F:  "; HEX(Register.F, 2);  ' DEBUG
  LOCATE 43, 20: PRINT "H:  "; HEX(Register.H, 2);  ' DEBUG
  LOCATE 44, 20: PRINT "L:  "; HEX(Register.L, 2);  ' DEBUG
  LOCATE 45, 20: PRINT "AF: "; HEX(ReadRegAF(), 4); ' DEBUG
  LOCATE 46, 20: PRINT "BC: "; HEX(ReadRegBC(), 4); ' DEBUG
  LOCATE 47, 20: PRINT "DE: "; HEX(ReadRegDE(), 4); ' DEBUG
  LOCATE 48, 20: PRINT "HL: "; HEX(ReadRegHL(), 4); ' DEBUG
  LOCATE 49, 20: PRINT "SP: "; HEX(Register.SP, 4); ' DEBUG
  LOCATE 50, 20: PRINT "PC: "; HEX(Register.PC, 4); ' DEBUG
  LOCATE 51, 20: PRINT "z:  "; GetFlagZ();  ' DEBUG
  LOCATE 52, 20: PRINT "h:  "; GetFlagH();  ' DEBUG
  LOCATE 53, 20: PRINT "n:  "; GetFlagN();  ' DEBUG
  LOCATE 54, 20: PRINT "c:  "; GetFlagC();  ' DEBUG
  LOCATE 55, 20: PRINT "LY: "; HEX(IoRegisters(&hFF44), 2)  ' DEBUG
  LOCATE 56, 20: PRINT "LYC:"; HEX(IoRegisters(&hFF45), 2)  ' DEBUG
  LOCATE 57, 20: PRINT "IME:" & InterruptsState             ' DEBUG
END SUB

*/

void DebugShowCpuRegisters() {
}

void DebugShowCPUspeed() {
}

/*void PrintDebug(char *DebugLog) {
}*/

void DebugOnScreen() {
}
