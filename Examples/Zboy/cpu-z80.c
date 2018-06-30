/*
  Emulation core of the Zilog 80 CPU clone used in GameBoy consoles
  Part of the zBoy project
  Copyright (C) Mateusz Viste 2010,2011,2012,2013
*/


struct CpuRegisters { /* Note: IX and IY have been removed from the GB Z80 clone */
  /* 8 bit registers */
  uint8_t A;   /* Accumulator */
  union{
    struct{
      uint8_t B;   /* Commonly used as a 8bit counter */
      uint8_t C;
    };
  };
  union{
    struct{
      uint8_t D;
      uint8_t E;
    };
  };
  uint8_t F;   /* Flags */
  union{
    struct{
      uint8_t H;
      uint8_t L;
    };
    uint16_t HL;
  };
  /*I AS UBYTE */  /* Interrupt vector register */
  /*R AS UBYTE */  /* Refresh register / might be used to generate random numbers */
  /* 16 bit register - Note: to write/read composite register, use dedicated functions! (eg. ReadRegAF) */
  uint16_t PC;        /* Program counter (holds memory adress currently executed code). No function can change PC */
  uint16_t SP;        /* The stack pointer. Holds current address of the top of the stack */
};

struct CpuRegisters Register;

int InterruptsState = 0;    /*  1 = ON  0 = OFF   (this is the IME register) */
int HaltBug = 0;            /* used to emulate the "HALT bug" (using HLT when IME is 0) -> The Smurfs won't work without that! */
int HaltState = 0;          /* 0 = CPU is running   1 = CPU is HALTed (wait until next interrupt) */

#include "cpu-microcode.c"  /* add microinstruction set */

#ifdef DEBUGMODE
  uint8_t DebugCpuPause = 0;   /* If set, pause at each CPU step */
#endif


void PrintFatalError(char *msg) {
  SetUserMsg(msg); /* Set up the message */
  UserMessageFramesLeft = 5184000; /* Force the message to stay for that much frames (@60Hz it would be 24h) */
  /* Load a smallish program that will disable interrupts and loop for ever */
  InterruptsState = 0; /* Disable interrupts */
  Register.PC = 0xFF90; /* Force an immediate jump to FF90h */
  MemoryInternalHiRAM[0xFF90] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF91] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF92] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF93] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF94] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF95] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF96] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF97] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF98] = 0x00; /* NOP */
  MemoryInternalHiRAM[0xFF99] = 0xC3; /* \            */
  MemoryInternalHiRAM[0xFF9A] = 0x90; /* -- JP FF90h  */
  MemoryInternalHiRAM[0xFF9B] = 0xFF; /* /            */
}


inline void PopPCfromStack(void) {
  uint8_t tUbyteBuff1, tUbyteBuff2;
  PopFromStack(&tUbyteBuff1, &tUbyteBuff2);
  Register.PC = ((tUbyteBuff1 << 8) | tUbyteBuff2);
}

inline void PushToStack16(int x) { /* Push to stack a 16bit value */
  PushToStack((x >> 8), (x & bx11111111));
}


/* Routine to process interrupts */
inline void CheckInterrupts(void) {
  /* IME: Interrupt Master Enable (InterruptsState)
     IE: Interrupt Enable (0xFFFF)
     IF: Interrupt flag   (0xFF0F) */
  static uint8_t IrqTempState;
  if ((InterruptsState == 0) && (HaltState == 0)) return;
  IrqTempState = (MemoryInternalHiRAM[0xFFFF] & IoRegisters[0xFF0F] & bx00011111);    /* IE AND IF, but only on 5 bits (because there are 5 INTs to monitor) */
  if (IrqTempState == 0) return; /* No interrupt to handle */
  if (HaltState == 1) {
    HaltState = 0;   /* If the CPU was HALTed, resume it, but DON'T touch interrupts if the CPU is in DI state! */
    Register.PC++;
    if (InterruptsState == 0) return; /* If interrupts are disabled, quit here. We don't want to handle them if they are off, just resume the HALT */
  }
  PushToStack16(Register.PC);            /* Save current address */
  InterruptsState = 0;   /*  Reset the IME flag   * this is MANDATORY! * */
  if ((IrqTempState & INT_VBLANK) != 0) {         /* V-Blank */
      PrintDebug("INT: VBLANK\n");
      Register.PC = 0x40;
      IoRegisters[0xFF0F] &= ~INT_VBLANK;  /* Reset the trigerred if flag */
    } else if ((IrqTempState & INT_LCDC) != 0) {  /* LCDC */
      PrintDebug("INT: LCDC\n");
      Register.PC = 0x48;
      IoRegisters[0xFF0F] &= ~INT_LCDC;  /* Reset the trigerred if flag */
    } else if ((IrqTempState & INT_TIMA) != 0) {    /* TIMER */
      PrintDebug("INT: TIMER\n");
      Register.PC = 0x50;
      IoRegisters[0xFF0F] &= ~INT_TIMA;  /* Reset the trigerred if flag */
    } else if ((IrqTempState & INT_SERIAL) != 0) {  /* SERIAL */
      PrintDebug("INT: SERIAL\n");
      Register.PC = 0x58;
      IoRegisters[0xFF0F] &= ~INT_SERIAL;  /* Reset the trigerred if flag */
    } else if ((IrqTempState & INT_JOYPAD) != 0) {  /* Joypad */
      PrintDebug("INT: JOYPAD\n");
      Register.PC = 0x60;
      IoRegisters[0xFF0F] &= ~INT_JOYPAD;  /* Reset the trigerred if flag */
  }
}

int OP_0x00(){   /* NOP (No operation) */
  PrintDebug("NOP");  /* DEBUG */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x01(){   /* LD BC,nn  (load nn into BC) */
  PrintDebug("LD BC,nn"); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2))); */ /* DEBUG */
  WriteRegBC(MemoryRead(Register.PC + 2), MemoryRead(Register.PC + 1));
  Register.PC += 3;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x02(){   /* LD (BC),A */   /* Load value of A into address at BC */
  PrintDebug("LD (BC),A");  /* DEBUG */
  MemoryWrite(ReadRegBC(), Register.A);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x03(){   /* INC BC (register BC+=1) */
  PrintDebug("INC BC");  /* DEBUG */
  IncReg16(&Register.B, &Register.C);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x04(){   /* INC B (register B+=1) */
  PrintDebug("INC B");  /* DEBUG */
  IncReg8(&Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x05(){   /* DEC B (register B-=1) */
  PrintDebug("DEC B");  /* DEBUG */
  DecReg8(&Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x06(){   /* LD B,n (load n into B) */
  PrintDebug("LD B,n");  /* DEBUG */
  Register.B = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;
}
int OP_0x07(){   /* RLCA (Rotate reg A left, old bit 7 written to Carry flag) */
  PrintDebug("RLCA");  /*  DEBUG */
  RLCA();
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x08(){   /* LD (nn),SP  (puts SP value at nn) */
  /*MemoryWrite16(DwordVal(MemoryRead(Register.PC + 1),MemoryRead(Register.PC + 2)), Register.SP); */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD (nn),SP"); /* & HEX(DwordVal(UbyteBuff1,UbyteBuff2)) & ",SP"); */
  MemoryWrite(DwordVal(UbyteBuff1,UbyteBuff2), (Register.SP & bx11111111));
  MemoryWrite(DwordVal(UbyteBuff1,UbyteBuff2) + 1, (Register.SP >> 8) & bx11111111);
  Register.PC += 3;
  return 20;   /* that many CPU cycles should be spent on this instruction */
}
int OP_0x09(){   /* ADD HL,BC */
  PrintDebug("ADD HL,BC");
  AddToHL(ReadRegBC());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0A(){   /* LD A,(BC)  (put value from address BC to A) */
  PrintDebug("LD A,(BC)");  /* DEBUG */
  Register.A = MemoryRead(ReadRegBC());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0B(){   /* DEC BC (register BC-=1) */
  PrintDebug("DEC BC");  /* DEBUG */
  DecReg16(&Register.B, &Register.C);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0C(){   /* INC C (register C+=1) */
  PrintDebug("INC C");  /* DEBUG */
  IncReg8(&Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0D(){   /* DEC C (register C-=1) */
  PrintDebug("DEC C");  /* DEBUG */
  DecReg8(&Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0E(){   /* LD C,n (load n into C) */
  PrintDebug("LD C,n");  /* DEBUG */
  Register.C = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x0F(){   /* RRCA (Rotate reg A right, old bit 0 written to Carry flag) */
  PrintDebug("RRCA");  /*  DEBUG */
  RRCA();
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x10(){   /* STOP (opcode 10 00) */
  QuitEmulator = 1;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x11(){   /* LD DE,nn  (load nn into DE) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD DE,nn"); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2))); */  /* DEBUG */
  WriteRegDE(UbyteBuff2, UbyteBuff1);
  Register.PC += 3;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x12(){   /* LD (DE),A  */  /* Load value of A into address at DE */
  PrintDebug("LD (DE),A");  /* DEBUG */
  MemoryWrite(ReadRegDE(), Register.A);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x13(){   /* INC DE (register DE+=1) */
  PrintDebug("INC DE");  /* DEBUG */
  IncReg16(&Register.D, &Register.E);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x14(){   /* INC D (register D+=1) */
  PrintDebug("INC D");  /* DEBUG */
  IncReg8(&Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x15(){   /* DEC D (register D-=1) */
  PrintDebug("DEC D");  /* DEBUG */
  DecReg8(&Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x16(){   /* LD D,n (load n into D) */
  PrintDebug("LD D,n");  /* DEBUG */
  Register.D = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x17(){  /* RLA */
  PrintDebug("RLA");  /* DEBUG */
  RotateLeftCarry(&Register.A);
  ResetFlagZ(); /* It looks like the only diff between 'RL A' and 'RLA' is that the latter always resets Z, while the former sets it accordingly to the result */
  Register.PC += 1;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x18(){   /* JR n (jump to PC+n) */
  Register.PC += 2;  /* First increment the PC, and then jump */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC - 1);
  PrintDebug("JR n"); /* & UbyteToByte(UbyteBuff1)) */ /* DEBUG */
  Register.PC += UbyteToByte(UbyteBuff1);
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x19(){   /* ADD HL,DE */
  PrintDebug("ADD HL,DE");
  AddToHL(ReadRegDE());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1A(){   /* LD A,(DE)  (put value from address DE to A) */
  PrintDebug("LD A,(DE)");
  Register.A = MemoryRead(ReadRegDE());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1B(){   /* DEC DE (register DE-=1) */
  PrintDebug("DEC DE");  /* DEBUG */
  DecReg16(&Register.D, &Register.E);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1C(){   /* INC E (register E+=1) */
  PrintDebug("INC E");  /* DEBUG */
  IncReg8(&Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1D(){   /* DEC E (register E-=1) */
  PrintDebug("DEC E");  /* DEBUG */
  DecReg8(&Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1E(){   /* LD E,n (load n into E) */
  PrintDebug("LD E,n");  /* DEBUG */
  Register.E = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x1F(){  /* RRA */
  PrintDebug("RRA");  /* DEBUG */
  RotateRightCarry(&Register.A);
  ResetFlagZ(); /* It looks like the only diff between 'RR A' and 'RRA' is that the latter always resets Z, while the former sets it accordingly to the result */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x20(){   /* JR NZ,n (jump to PC+n if Z flag is false) */
  PrintDebug("JR NZ,n");  /* DEBUG */
  Register.PC += 2;  /* First increment the PC, and then we'll see */
  if (GetFlagZ() == 0) {
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC - 1);
    Register.PC += UbyteToByte(UbyteBuff1);
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x21(){   /* LD HL,nn  (load nn into HL) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD HL,%04Xh", DwordVal(UbyteBuff1, UbyteBuff2)); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2))) */  /* DEBUG */
  WriteRegHL(UbyteBuff2, UbyteBuff1);
  Register.PC += 3;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x22(){   /* LD (HL+),A (put A into memory addr HL, and increments HL) */
  PrintDebug("LD (HL+),A");   /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.A);
  IncReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x23(){   /* INC HL (register HL+=1) */
  PrintDebug("INC HL");  /* DEBUG */
  IncReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x24(){   /* INC H (register H+=1) */
  PrintDebug("INC H");  /* DEBUG */
  IncReg8(&Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x25(){   /* DEC H (register H-=1) */
  PrintDebug("DEC H");  /* DEBUG */
  DecReg8(&Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x26(){   /* LD H,n (load n into H) */
  PrintDebug("LD H,n");  /* DEBUG */
  Register.H = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x27(){   /* DAA */
  PrintDebug("DAA");  /* DEBUG */
  AdjustDAA();
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x28(){   /* JR Z,n (jump to PC+n if Z flag is true) */
  PrintDebug("JR Z,n");  /* DEBUG */
  Register.PC += 2;  /* First increment the PC, and then we'll see */
  if (GetFlagZ() == 1) {
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC - 1);
    Register.PC += UbyteToByte(UbyteBuff1);
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x29(){   /* ADD HL,HL */
  PrintDebug("ADD HL,HL");
  AddToHL(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2A(){   /* LD A,(HL+) (Put value @HL into A, and increment HL by 1) */
  PrintDebug("LD A,(HL+)");    /* DEBUG */
  Register.A = MemoryRead(ReadRegHL());
  IncReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2B(){   /* DEC HL (register HL-=1) */
  PrintDebug("DEC HL");  /* DEBUG */
  DecReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2C(){   /* INC L (register L+=1) */
  PrintDebug("INC L");  /* DEBUG */
  IncReg8(&Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2D(){   /* DEC L (register L-=1) */
  PrintDebug("DEC L");  /* DEBUG */
  DecReg8(&Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2E(){   /* LD L,n (load n into L) */
  PrintDebug("LD L,n");  /* DEBUG */
  Register.L = MemoryRead(Register.PC + 1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x2F(){   /* CPL (complements the A register, that is flips all bits) */
  PrintDebug("CPL");
  SetFlagN();
  SetFlagH();
  Register.A = (Register.A ^ bx11111111);  /* Flip all bits */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x30(){   /* JR NC,n (jump to PC+n if C flag is false) */
  PrintDebug("JR NC,n");  /* DEBUG */
  Register.PC += 2;  /* First increment the PC, and then we'll see */
  if (GetFlagC() == 0) {
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC - 1);
    Register.PC += UbyteToByte(UbyteBuff1);
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x31(){   /* LD SP,nn */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD SP,%04X", DwordVal(UbyteBuff1, UbyteBuff2)); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2))) */  /* DEBUG */
  Register.SP = DwordVal(UbyteBuff1, UbyteBuff2);
  Register.PC += 3;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x32(){   /* LD (HL-),A   (put A into address at HL, then decrement HL) */
  PrintDebug("LD (HL-),A");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.A);
  DecReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x33(){   /* INC SP (register SP+=1) */
  PrintDebug("INC SP");  /* DEBUG */
  if (Register.SP == 65535) {
    Register.SP = 0;
  } else {
    Register.SP += 1;
  }
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x34(){   /* INC (HL) (value at (HL) is incremented) */
  PrintDebug("INC (HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  IncReg8(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x35(){   /* DEC (HL) -> (HL) -= 1 */
  PrintDebug("DEC (HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());  /* Save old value at (HL) */
  DecReg8(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x36(){   /* LD (HL),n */   /* Load value of n into address at HL */
  PrintDebug("LD (HL),n");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x37(){   /* SCF (Set Carry Flag) */
  PrintDebug("SCF");  /* DEBUG */
  ResetFlagN();
  ResetFlagH();
  SetFlagC();
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x38(){   /* JR C,n (jump to PC+n if C flag is true) */
  PrintDebug("JR C,n");  /* DEBUG */
  Register.PC += 2;  /* First increment the PC, and then we'll see */
  if (GetFlagC() == 1) {
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC - 1);
    Register.PC += UbyteToByte(UbyteBuff1);
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x39(){   /* ADD HL,SP */
  PrintDebug("ADD HL,SP");
  AddToHL(Register.SP);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3A(){   /* LD A,(HL-)   (put value at address HL into A, and decrement HL) */
  PrintDebug("LD A,(HL-)");
  Register.A = MemoryRead(ReadRegHL());
  DecReg16(&Register.H, &Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3B(){   /* DEC SP (register SP-=1) */
  PrintDebug("DEC SP");  /* DEBUG */
  if (Register.SP == 0) {
    Register.SP = 65535;
  } else {
    Register.SP -= 1;
  }
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3C(){   /* INC A (register A+=1) */
  PrintDebug("INC A");  /* DEBUG */
  IncReg8(&Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3D(){   /* DEC A (register A-=1) */
  PrintDebug("DEC A");  /* DEBUG */
  DecReg8(&Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3E(){   /* LD A,n  (put n into A) */
  Register.A = MemoryRead(Register.PC + 1);
  PrintDebug("LD A,n"); /* & Register.A) */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x3F(){   /* CCF  (Complement Carry Flag) */
  PrintDebug("CCF");
  ResetFlagN();
  ResetFlagH();
  if (GetFlagC() == 0) {
    SetFlagC();
  } else {
    ResetFlagC();
  }
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x40(){   /* LD B,B     Put value B into B */
  PrintDebug("LD B,B");  /* DEBUG */
  /*Register.B = Register.B; */   /* commented out - useless */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x41(){   /* LD B,C       Put value C into B */
  PrintDebug("LD B,C");  /* DEBUG */
  Register.B = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x42(){   /* LD B,D     Put value D into B */
  PrintDebug("LD B,D");  /* DEBUG */
  Register.B = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x43(){   /* LD B,E     Put value E into B */
  PrintDebug("LD B,E");  /* DEBUG */
  Register.B = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x44(){   /* LD B,H      Put value H into B */
  PrintDebug("LD B,H");  /* DEBUG */
  Register.B = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x45(){   /* LD B,L     Put value L into B */
  PrintDebug("LD B,L");  /* DEBUG */
  Register.B = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x46(){   /* LD B,(HL)  (put value from address HL to B) */
  PrintDebug("LD B,(HL)");
  Register.B = MemoryRead(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x47(){   /* LD B,A      Put value A into B */
  PrintDebug("LD B,A");  /* DEBUG */
  Register.B = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x48(){   /* LD C,B     Put value B into C */
  PrintDebug("LD C,B");  /* DEBUG */
  Register.C = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x49(){   /* LD C,C      Put value C into C */
  PrintDebug("LD C,C");  /* DEBUG */
  Register.C = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4A(){   /* LD C,D     Put value D into C */
  PrintDebug("LD C,D");  /* DEBUG */
  Register.C = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4B(){   /* LD C,E     Put value E into C */
  PrintDebug("LD C,E");  /* DEBUG */
  Register.C = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4C(){   /* LD C,H     Put value H into C */
  PrintDebug("LD C,H");  /* DEBUG */
  Register.C = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4D(){   /* LD C,L     Put value L into C */
  PrintDebug("LD C,L");  /* DEBUG */
  Register.C = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4E(){   /* LD C,(HL)  (put value from address HL to C) */
  PrintDebug("LD C,(HL)");
  Register.C = MemoryRead(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x4F(){   /* LD C,A     Put value A into C */
  PrintDebug("LD C,A");  /* DEBUG */
  Register.C = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x50(){   /* LD D,B     Put value B into D */
  PrintDebug("LD D,B");  /* DEBUG */
  Register.D = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x51(){   /* LD D,C      Put value C into D */
  PrintDebug("LD D,C");  /* DEBUG */
  Register.D = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x52(){   /* LD D,D     Put value D into D */
  PrintDebug("LD D,D");  /* DEBUG */
  /*Register.D = Register.D; */ /* Commented out - useless */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x53(){   /* LD D,E      Put value E into D */
  PrintDebug("LD D,E");  /* DEBUG */
  Register.D = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x54(){   /* LD D,H      Put value H into D */
  PrintDebug("LD D,H");  /* DEBUG */
  Register.D = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x55(){   /* LD D,L     Put value L into D */
  PrintDebug("LD D,L");  /* DEBUG */
  Register.D = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x56(){   /* LD D,(HL)  (put value from address HL to D) */
  PrintDebug("LD D,(HL)");
  Register.D = MemoryRead(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x57(){   /* LD D,A      Put value A into D */
  PrintDebug("LD D,A");  /* DEBUG */
  Register.D = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x58(){   /* LD E,B     Put value B into E */
  PrintDebug("LD E,B");  /* DEBUG */
  Register.E = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x59(){   /* LD E,C     Put value C into E */
  PrintDebug("LD E,C");  /* DEBUG */
  Register.E = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5A(){   /* LD E,D     Put value D into E */
  PrintDebug("LD E,D");  /* DEBUG */
  Register.E = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5B(){   /* LD E,E     Put value E into E */
  PrintDebug("LD E,E");  /* DEBUG */
  Register.E = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5C(){   /* LD E,H     Put value H into E */
  PrintDebug("LD E,H");  /* DEBUG */
  Register.E = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5D(){   /* LD E,L     Put value L into E */
  PrintDebug("LD E,L");  /* DEBUG */
  Register.E = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5E(){   /* LD E,(HL)  (put value from address HL to E) */
  PrintDebug("LD E,(HL)");
  Register.E = MemoryRead(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x5F(){   /* LD E,A     Put value A into E */
  PrintDebug("LD E,A");  /* DEBUG */
  Register.E = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x60(){   /* LD H,B     Put value B into H */
  PrintDebug("LD H,B");  /* DEBUG */
  Register.H = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x61(){   /* LD H,C     Put value C into H */
  PrintDebug("LD H,C");  /* DEBUG */
  Register.H = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x62(){   /* LD H,D     Put value D into H */
  PrintDebug("LD H,D");  /* DEBUG */
  Register.H = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x63(){   /* LD H,E     Put value E into H */
  PrintDebug("LD H,E");  /* DEBUG */
  Register.H = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x64(){   /* LD H,H      Put value H into H */
  PrintDebug("LD H,H");  /* DEBUG */
  /*Register.H = Register.H */   /* Commented out, because there is no point in doing that */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x65(){   /* LD H,L     Put value L into H */
  PrintDebug("LD H,L");  /* DEBUG */
  Register.H = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x66(){   /* LD H,(HL)  (put value from address HL to H) */
  PrintDebug("LD H,(HL)");
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  Register.H = UbyteBuff1;   /* Don't try to work on H directly! It would change also HL! */
  /*Register.H = MemoryRead(ReadRegHL()) */
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x67(){   /* LD H,A      Put value A into H */
  PrintDebug("LD H,A");  /* DEBUG */
  Register.H = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x68(){   /* LD L,B     Put value B into L */
  PrintDebug("LD L,B");  /* DEBUG */
  Register.L = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x69(){   /* LD L,C     Put value C into L */
  PrintDebug("LD L,C");  /* DEBUG */
  Register.L = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6A(){   /* LD L,D     Put value D into L */
  PrintDebug("LD L,D");  /* DEBUG */
  Register.L = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6B(){   /* LD L,E     Put value E into L */
  PrintDebug("LD L,E");  /* DEBUG */
  Register.L = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6C(){   /* LD L,H     Put value H into L */
  PrintDebug("LD L,H");  /* DEBUG */
  Register.L = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6D(){   /* LD L,L      Put value L into L */
  PrintDebug("LD L,L");  /* DEBUG */
  /*Register.L = Register.L */   /* Commented out, because there is no point in doing such operation */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6E(){   /* LD L,(HL)  (put value from address HL to L) */
  PrintDebug("LD L,(HL)");
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  Register.L = UbyteBuff1;    /* Don't try to work directly on L! It would also change HL! */
  /*Register.L = MemoryRead(ReadRegHL()) */
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x6F(){   /* LD L,A      Put value A into L */
  PrintDebug("LD L,A");  /* DEBUG */
  Register.L = Register.A;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x70(){   /* LD (HL),B    Load value of B into address at HL */
  PrintDebug("LD (HL),B");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.B);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x71(){   /* LD (HL),C    Load value of C into address at HL */
  PrintDebug("LD (HL),C");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.C);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x72(){   /* LD (HL),D    Load value of D into address at HL */
  PrintDebug("LD (HL),D");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.D);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x73(){   /* LD (HL),E   Load value of E into address at HL */
  PrintDebug("LD (HL),E");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.E);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x74(){   /* LD (HL),H   Load value of H into address at HL */
  PrintDebug("LD (HL),H");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.H);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x75(){   /* LD (HL),L   Load value of L into address at HL */
  PrintDebug("LD (HL),L");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.L);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x76(){   /* HALT   HALTs the CPU until next interrupt */
  /*  Note: I don't increment PC here, to make the game loop on the HALT op.
      PC will be incremented if an interrupt occurs. */
  HaltState = 1;
  if (InterruptsState == 0) {
    PrintDebug("Halt [INTs are OFF]");
    HaltBug = 1;
  } else {
    PrintDebug("Halt [INTs are ON]");
  }
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x77(){   /* LD (HL),A   Load value of A into address at HL */
  PrintDebug("LD (HL),A");  /* DEBUG */
  MemoryWrite(ReadRegHL(), Register.A);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x78(){   /* LD A,B  (put B into A) */
  PrintDebug("LD A,B");   /* DEBUG */
  Register.A = Register.B;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x79(){   /* LD A,C  (put C into A) */
  PrintDebug("LD A,C");   /* DEBUG */
  Register.A = Register.C;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7A(){   /* LD A,D  (put D into A) */
  PrintDebug("LD A,D");   /* DEBUG */
  Register.A = Register.D;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7B(){   /* LD A,E  (put E into A) */
  PrintDebug("LD A,E");   /* DEBUG */
  Register.A = Register.E;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7C(){   /* LD A,H  (put H into A) */
  PrintDebug("LD A,H");   /* DEBUG */
  Register.A = Register.H;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7D(){   /* LD A,L  (put L into A) */
  PrintDebug("LD A,L");   /* DEBUG */
  Register.A = Register.L;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7E(){   /* LD A,(HL)  (put value from address HL to A) */
  PrintDebug("LD A,(HL)");
  Register.A = MemoryRead(ReadRegHL());
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x7F(){   /* LD A,A  (put A into A) */
  PrintDebug("LD A,A");   /* DEBUG */
  /*Register.A = Register.A */  /* Commented out, because there is no point in doing such operation */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x80(){  /* ADD A,B  (A+=B) */
  PrintDebug("ADD A,B");
  AddToA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x81(){  /* ADD A,C  (A+=C) */
  PrintDebug("ADD A,C");
  AddToA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x82(){  /* ADD A,D  (A+=D) */
  PrintDebug("ADD A,D");
  AddToA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x83(){  /* ADD A,E  (A+=E) */
  PrintDebug("ADD A,E");
  AddToA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x84(){  /* ADD A,H  (A+=H) */
  PrintDebug("ADD A,H");
  AddToA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x85(){  /* ADD A,L  (A+=L) */
  PrintDebug("ADD A,L");
  AddToA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x86(){  /* ADD A,(HL)  (A+=[HL]) */
  PrintDebug("ADD A,(HL)");
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  AddToA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x87(){  /* ADD A,A  (A+=A) */
  PrintDebug("ADD A,A");
  AddToA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x88(){   /* ADC A,B */
  PrintDebug("ADC A,B");   /* DEBUG */
  AdcA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x89(){   /* ADC A,C */
  PrintDebug("ADC A,C");   /* DEBUG */
  AdcA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8A(){   /* ADC A,D */
  PrintDebug("ADC A,D");   /* DEBUG */
  AdcA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8B(){   /* ADC A,E */
  PrintDebug("ADC A,E");   /* DEBUG */
  AdcA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8C(){   /* ADC A,H */
  PrintDebug("ADC A,H");   /* DEBUG */
  AdcA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8D(){   /* ADC A,L */
  PrintDebug("ADC A,L");   /* DEBUG */
  AdcA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8E(){   /* ADC A,(HL) */
  PrintDebug("ADC A,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  AdcA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x8F(){   /* ADC A,A */
  PrintDebug("ADC A,A");   /* DEBUG */
  AdcA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x90(){  /* SUB B */
  PrintDebug("SUB B");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x91(){  /* SUB C */
  PrintDebug("SUB C");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x92(){  /* SUB D */
  PrintDebug("SUB D");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x93(){  /* SUB E */
  PrintDebug("SUB E");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x94(){  /* SUB H */
  PrintDebug("SUB H");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x95(){  /* SUB L */
  PrintDebug("SUB L");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x96(){  /* SUB (HL)  (sub content at address HL from A) */
  PrintDebug("SUB (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SubValFromReg8(&Register.A, &UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x97(){  /* SUB A */
  PrintDebug("SUB A");  /* DEBUG */
  SubValFromReg8(&Register.A, &Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x98(){  /* SBC A,B */
  PrintDebug("SBC A,B");  /* DEBUG */
  SbcA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x99(){  /* SBC A,C */
  PrintDebug("SBC A,C");  /* DEBUG */
  SbcA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9A(){  /* SBC A,D */
  PrintDebug("SBC A,D");  /* DEBUG */
  SbcA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9B(){  /* SBC A,E */
  PrintDebug("SBC A,E");  /* DEBUG */
  SbcA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9C(){  /* SBC A,H */
  PrintDebug("SBC A,H");  /* DEBUG */
  SbcA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9D(){  /* SBC A,L */
  PrintDebug("SBC A,L");  /* DEBUG */
  SbcA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9E(){  /* SBC A,(HL) */
  PrintDebug("SBC A,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SbcA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0x9F(){  /* SBC A,A */
  PrintDebug("SBC A,A");  /* DEBUG */
  SbcA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA0(){   /* AND B (A = A AND B) */
  PrintDebug("AND B");  /* DEBUG */
  AndA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA1(){   /* AND C (A = A AND C) */
  PrintDebug("AND C");  /* DEBUG */
  AndA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA2(){   /* AND D (A = A AND D) */
  PrintDebug("AND D");  /* DEBUG */
  AndA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA3(){   /* AND E (A = A AND E) */
  PrintDebug("AND E");  /* DEBUG */
  AndA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA4(){   /* AND H (A = A AND H) */
  PrintDebug("AND H");  /* DEBUG */
  AndA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA5(){   /* AND L (A = A AND L) */
  PrintDebug("AND L");  /* DEBUG */
  AndA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA6(){   /* AND (HL) (A = A AND [HL]) */
  PrintDebug("AND (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  AndA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA7(){   /* AND A (A = A AND A) */
  PrintDebug("AND A");  /* DEBUG */
  AndA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA8(){   /* XOR B (A = A XOR B) */
  PrintDebug("XOR B");  /* DEBUG */
  XorA(Register.B); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xA9(){   /* XOR C (A = A XOR C) */
  PrintDebug("XOR C");  /* DEBUG */
  XorA(Register.C); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAA(){   /* XOR D (A = A XOR D) */
  PrintDebug("XOR D");  /* DEBUG */
  XorA(Register.D); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAB(){   /* XOR E (A = A XOR E) */
  PrintDebug("XOR E");  /* DEBUG */
  XorA(Register.E); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAC(){   /* XOR H (A = A XOR H) */
  PrintDebug("XOR H");  /* DEBUG */
  XorA(Register.H); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAD(){   /* XOR L (A = A XOR L) */
  PrintDebug("XOR L");  /* DEBUG */
  XorA(Register.L); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAE(){   /* XOR (HL)  (A = A XOR [HL]) */
  PrintDebug("XOR (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  XorA(UbyteBuff1); /* Xor with A, result in A */
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xAF(){   /* XOR A (A = A XOR A) */
  PrintDebug("XOR A");  /* DEBUG */
  XorA(Register.A); /* Xor with A, result in A */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB0(){   /* OR B  (A = A OR B) */
  PrintDebug("OR B");   /* DEBUG */
  OrA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB1(){   /* OR C  (A = A OR C) */
  PrintDebug("OR C");   /* DEBUG */
  OrA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB2(){   /* OR D  (A = A OR D) */
  PrintDebug("OR D");   /* DEBUG */
  OrA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB3(){   /* OR E  (A = A OR E) */
  PrintDebug("OR E");   /* DEBUG */
  OrA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB4(){   /* OR H  (A = A OR H) */
  PrintDebug("OR H");   /* DEBUG */
  OrA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB5(){   /* OR L  (A = A OR L) */
  PrintDebug("OR L");   /* DEBUG */
  OrA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB6(){   /* OR (HL)  (A = A OR (HL)) */
  PrintDebug("OR (HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  OrA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB7(){   /* OR A  (A = A OR A) */
  PrintDebug("OR A");   /* DEBUG */
  OrA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB8(){   /* CP B (compare A with B) */
  PrintDebug("CP B");  /* DEBUG */
  CmpA(Register.B);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xB9(){   /* CP C (compare A with C) */
  PrintDebug("CP C");   /* DEBUG */
  CmpA(Register.C);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBA(){   /* CP D (compare A with D) */
  PrintDebug("CP D");   /* DEBUG */
  CmpA(Register.D);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBB(){   /* CP E (compare A with E) */
  PrintDebug("CP E");   /* DEBUG */
  CmpA(Register.E);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBC(){   /* CP H (compare A with H) */
  PrintDebug("CP H");   /* DEBUG */
  CmpA(Register.H);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBD(){   /* CP L (compare A with L) */
  PrintDebug("CP L");   /* DEBUG */
  CmpA(Register.L);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBE(){   /* CP (HL) (compare A with value at address [HL]) */
  PrintDebug("CP (HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  CmpA(UbyteBuff1);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xBF(){   /* CP A (compare A with A) */
  PrintDebug("CP A");   /* DEBUG */
  CmpA(Register.A);
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC0(){   /* RET NZ    RET if Z = 0 */
  PrintDebug("RET NZ");
  if (GetFlagZ() == 0) {
    PopPCfromStack();
  } else {
    Register.PC++;
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC1(){   /* POP BC (Write the value in stack into BC) */
  PrintDebug("POP BC");  /* DEBUG */
  PopFromStack(&Register.B, &Register.C);
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC2(){   /* JP NZ,nn (jump if Z=0) */
  PrintDebug("JP NZ,nn");   /* DEBUG */
  if (GetFlagZ() == 0) {  /* Jump */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  } else {   /* Don't jump */
    Register.PC += 3;
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC3(){   /* JP nn (Unconditional jump) */
  PrintDebug("JP nn");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC4(){   /* CALL NZ,nn (call if Z=0) */
  PrintDebug("CALL NZ,nn");   /* DEBUG */
  if (GetFlagZ() == 0) {  /* Call */
    PushToStack16(Register.PC + 3);           /* First save the current value of the PC (+3 bytes) */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  } else {   /* Don't call */
    Register.PC += 3;
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC5(){   /* PUSH BC */
  PrintDebug("PUSH BC");  /* DEBUG */
  PushToStack(Register.B, Register.C);
  Register.PC++;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC6(){  /* ADD A,n  (A+=n) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("ADD An"); /*," & HEX(UbyteBuff1, 2)) */
  AddToA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC7(){   /* RST 00h */
  PrintDebug("RST 00h");   /* DEBUG */
  PushToStack16(Register.PC + 1);            /* Save current address (+1) */
  Register.PC = 0x00;  /* Jump to 00h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC8(){   /* RET Z   RET if Z = 1 */
  PrintDebug("RET Z");
  if (GetFlagZ() == 0) {
    Register.PC++;
  } else {
    PopPCfromStack();
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xC9(){   /* RET */
  PrintDebug("RET");
  PopPCfromStack();
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xCA(){   /* JP Z,nn (jump if Z=1) */
  PrintDebug("JP Z,nn");   /* DEBUG */
  if (GetFlagZ() == 0) {  /* Don't jump */
    Register.PC += 3;
  } else {   /* Jump */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xCB();
int OP_0xCC(){   /* CALL Z,nn (jump if Z=1) */
  PrintDebug("CALL Z,nn");   /* DEBUG */
  if (GetFlagZ() == 0) {   /* Don't jump */
    Register.PC += 3;
  } else {  /* Jump */
    PushToStack16(Register.PC + 3);   /* First save the current value of the program counter (+3 bytes) */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xCD(){   /* CALL (calls a subroutine) */
  PushToStack16(Register.PC + 3);    /* First save the current value of the program counter (+3 bytes) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("CALL nn"); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2))) */ /* DEBUG */
  Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xCE(){   /* ADC A,n */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("ADC A,n"); /* & HEX(UbyteBuff1,2)) */   /* DEBUG */
  AdcA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xCF(){   /* RST 08h */
  PrintDebug("RST 08h");   /* DEBUG */
  PushToStack16(Register.PC + 1);           /* Save current address (+1) */
  Register.PC = 0x08;  /* Jump to 08h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD0(){   /* RET NC */   /* RET if C = 0 */
  PrintDebug("RET NC");
  if (GetFlagC() == 0) {
    PopPCfromStack();
  } else {
    Register.PC++;
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD1(){   /* POP DE (Write the value in stack into DE) */
  PrintDebug("POP DE");  /* DEBUG */
  PopFromStack(&Register.D, &Register.E);
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD2(){   /* JP NC,nn (jump if C=0) */
  PrintDebug("JP NC,nn");   /* DEBUG */
  if (GetFlagC() == 0) {  /* Jump */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  } else {   /* Don't jump */
    Register.PC += 3;
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD4(){   /* CALL NC,nn (jump if Z=0) */
  PrintDebug("CALL NC,nn");   /* DEBUG */
  if (GetFlagC() == 0) {  /* Jump */
    PushToStack16(Register.PC + 3);    /* First save the current value of the program counter (+3 bytes) */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  } else {   /* Don't jump */
    Register.PC += 3;
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD5(){   /* PUSH DE */
  PrintDebug("PUSH DE");  /* DEBUG */
  PushToStack(Register.D, Register.E);
  Register.PC++;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD6(){  /* SUB n from A */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("SUB n"); /* & HEX(UbyteBuff1)) */  /* DEBUG */
  SubValFromReg8(&Register.A, &UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD7(){   /* RST 10h */
  PrintDebug("RST 10h");   /* DEBUG */
  PushToStack16(Register.PC + 1);     /* Save current address (+1) */
  Register.PC = 0x10;  /* Jump to 10h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD8(){   /* RET C */   /* RET if C = 1 */
  PrintDebug("RET C");
  if (GetFlagC() == 0) {
    Register.PC++;
  } else {
    PopPCfromStack();
  }
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xD9(){   /* RETI  (RET + EI) */
  PrintDebug("RETI");   /* DEBUG */
  PopPCfromStack();           /* RET (return from a call or int) */
  InterruptsState = 1;        /* EI (enable interrupts) */
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xDA(){   /* JP C,nn (jump if C=1) */
  PrintDebug("JP C,nn");   /* DEBUG */
  if (GetFlagC() == 0) {  /* Don't jump */
    Register.PC += 3;
  } else {   /* Jump */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xDC(){   /* CALL C,nn (jump if Z=1) */
  PrintDebug("CALL C,nn");   /* DEBUG */
  if (GetFlagC() == 1) {  /* Jump */
    PushToStack16(Register.PC + 3);    /* First save the current value of the program counter (+3 bytes) */
  uint8_t   UbyteBuff1 = MemoryRead(Register.PC + 1);
    uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
    Register.PC = DwordVal(UbyteBuff1, UbyteBuff2);
  } else {   /* Don't jump */
    Register.PC += 3;
  }
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xDE(){  /* SBC A,n */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("SBC A,n"); /* & UbyteBuff1) */  /* DEBUG */
  SbcA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xDF(){   /* RST 18h */
  PrintDebug("RST 18h");   /* DEBUG */
  PushToStack16(Register.PC + 1);     /* Save current address (+1) */
  Register.PC = 0x18;  /* Jump to 18h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE0(){   /* LD ($FF00+n),A */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("LD ($FF00+n)"); /* & HEX(UbyteBuff1, 2) & "),A"); */  /* DEBUG */
  MemoryWrite(0xFF00 + UbyteBuff1, Register.A);
  Register.PC += 2;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE1(){   /* POP HL (Write the value in stack into HL) */
  PrintDebug("POP HL");  /* DEBUG */
  PopFromStack(&Register.H, &Register.L);
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE2(){   /* LD (FF00h+C),A (put A into addr $FF00+C) */
  PrintDebug("LD (FF00h+C),A"); /* & HEX(0xFF00 + Register.C) & "),A"); */  /* DEBUG */
  MemoryWrite(0xFF00 + Register.C, Register.A);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE5(){   /* PUSH HL */
  PrintDebug("PUSH HL");  /* DEBUG */
  PushToStack(Register.H, Register.L);
  Register.PC++;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE6(){   /* A AND n */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("A AND n (n=0x%02X)", UbyteBuff1); /* & UbyteBuff1) */
  AndA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE7(){   /* RST 20h */
  PrintDebug("RST 20h");   /* DEBUG */
  PushToStack16(Register.PC + 1);    /* Save current address (+1) */
  Register.PC = 0x20;  /* Jump to 20h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE8(){   /* ADD SP,n */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("ADD SP,n"); /* & UbyteToByte(UbyteBuff1)) */ /* DEBUG */
  AddToSP(UbyteToByte(UbyteBuff1));
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xE9(){   /* JP (HL) / Jumps to address contained in HL (same as "LD PC,HL") */
  PrintDebug("JP (HL)");  /* DEBUG */
  Register.PC = ReadRegHL();
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xEA(){   /* LD (nn),A   Load value of A into address nn */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD (nn),A"); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2)) & "),A"); */  /* DEBUG */
  MemoryWrite(DwordVal(UbyteBuff1, UbyteBuff2), Register.A);
  Register.PC += 3;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xEE(){   /* XOR n  (A = A XOR n) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("XOR n"); /* & HEX(UbyteBuff1, 2)) */ /* DEBUG */
  XorA(UbyteBuff1); /* Xor with A, result in A */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xEF(){   /* RST 28h */
  PrintDebug("RST 28h");   /* DEBUG */
  PushToStack16(Register.PC + 1);    /* Save current address (+1) */
  Register.PC = 0x28;  /* Jump to 28h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF0(){   /* LD A,($FF00+n) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("LD A,($FF00+n)"); /* & HEX(UbyteBuff1, 2) & ")"); */  /* DEBUG */
  Register.A = MemoryRead(0xFF00 | UbyteBuff1);
  Register.PC += 2;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF1(){   /* POP AF (Write the value in stack into AF) */
  PrintDebug("POP AF");  /* DEBUG */
  PopFromStack(&Register.A, &Register.F);
  Register.F &= bx11110000; /* Make sure that lower 4 bits of the F register are always reset */
  Register.PC++;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF2(){   /* LD A,(FF00h+C)  (put value at address FF00+C into A) */
  PrintDebug("LD A,(FF00+C)");    /* DEBUG */
  Register.A = MemoryRead(0xFF00 | Register.C);
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF3(){   /* DI (Disables Interrupts) */
  PrintDebug("DI - Disables interrupts");  /* DEBUG */
  InterruptsState = 0;   /* Sets IME (Interrupts Master Enable) to 0 */
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF5(){   /* PUSH AF */
  PrintDebug("PUSH AF");  /* DEBUG */
  PushToStack(Register.A, Register.F);
  Register.PC++;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF6(){   /* OR n (A = A OR n) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("OR n"); /* & HEX(UbyteBuff1, 2)) */  /* DEBUG */
  OrA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF7(){   /* RST 30h */
  PrintDebug("RST 30h");   /* DEBUG */
  PushToStack16(Register.PC + 1);     /* Save current address (+1) */
  Register.PC = 0x30;  /* Jump to 30h */
  return 32;     /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF8(){   /* LD HL,SP+n */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  int8_t ByteBuff = UbyteToByte(UbyteBuff1);
  PrintDebug("LD HL,SP+n"); /* & ByteBuff) */
  uint16_t UintBuff = (Register.SP + ByteBuff);
  if ((UintBuff & 0xFF) < (Register.SP & 0xFF)) {
    SetFlagC();
  } else {
    ResetFlagC();
  }
  if ((UintBuff & 0xF) < (Register.SP & 0xF)) {
    SetFlagH();
  } else {
    ResetFlagH();
  }
  ResetFlagZ();
  ResetFlagN();
  WriteRegHL((UintBuff >> 8), (UintBuff & bx11111111));
  Register.PC += 2;
  return 12;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xF9(){   /* LD SP,HL  (put HL into SP) */
  PrintDebug("LD SP,HL");
  Register.SP = ReadRegHL();
  Register.PC++;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xFA(){   /* LD A,nn  (put value from address nn to A) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  uint8_t UbyteBuff2 = MemoryRead(Register.PC + 2);
  PrintDebug("LD A,nn"); /* & HEX(DwordVal(UbyteBuff1, UbyteBuff2), 4)); */
  Register.A = MemoryRead(DwordVal(UbyteBuff1, UbyteBuff2));
  Register.PC += 3;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xFB(){   /* EI (Enables Interrupts) */
  PrintDebug("EI - Enables interrupts");  /* DEBUG */
  InterruptsState = 1;
  Register.PC++;
  return 4;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xFE(){   /* CP n (compare A with n) */
  uint8_t UbyteBuff1 = MemoryRead(Register.PC + 1);
  PrintDebug("CP A,n"); /* & HEX(UbyteBuff1, 2)); */ /* DEBUG */
  CmpA(UbyteBuff1);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP_0xFF(){   /* RST 38h */
  PrintDebug("RST 38h");   /* DEBUG */
  PushToStack16(Register.PC + 1);     /* Save current address (+1) */
  Register.PC = 0x38;  /* Jump to 38h */
  return 32;  /* that many CPU cycles should be spent on this instruction */
}



int OP2_0x00(){  /* RLC B */
  PrintDebug("RLC B");  /* DEBUG */
  RotateRLC(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x01(){  /* RLC C */
  PrintDebug("RLC C");  /* DEBUG */
  RotateRLC(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x02(){  /* RLC D */
  PrintDebug("RLC D");  /* DEBUG */
  RotateRLC(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x03(){  /* RLC E */
  PrintDebug("RLC E");  /* DEBUG */
  RotateRLC(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x04(){  /* RLC H */
  PrintDebug("RLC H");  /* DEBUG */
  RotateRLC(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x05(){  /* RLC L */
  PrintDebug("RLC L");  /* DEBUG */
  RotateRLC(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x06(){  /* RLC (HL) */
  PrintDebug("RLC (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  RotateRLC(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x07(){  /* RLC A */
  PrintDebug("RLC A");  /* DEBUG */
  RotateRLC(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x08(){  /* RRC B */
  PrintDebug("RRC B");  /* DEBUG */
  RotateRRC(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x09(){  /* RRC C */
  PrintDebug("RRC C");  /* DEBUG */
  RotateRRC(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0A(){  /* RRC D */
  PrintDebug("RRC D");  /* DEBUG */
  RotateRRC(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0B(){  /* RRC E */
  PrintDebug("RRC E");  /* DEBUG */
  RotateRRC(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0C(){  /* RRC H */
  PrintDebug("RRC H");  /* DEBUG */
  RotateRRC(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0D(){  /* RRC L */
  PrintDebug("RRC L");  /* DEBUG */
  RotateRRC(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0E(){  /* RRC (HL) */
  PrintDebug("RRC (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  RotateRRC(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x0F(){  /* RRC A */
  PrintDebug("RRC A");  /* DEBUG */
  RotateRRC(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x10(){  /* RL B */
  PrintDebug("RL B");  /* DEBUG */
  RotateLeftCarry(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x11(){  /* RL C */
  PrintDebug("RL C");  /* DEBUG */
  RotateLeftCarry(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x12(){  /* RL D */
  PrintDebug("RL D");  /* DEBUG */
  RotateLeftCarry(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x13(){  /* RL E */
  PrintDebug("RL E");  /* DEBUG */
  RotateLeftCarry(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x14(){  /* RL H */
  PrintDebug("RL H");  /* DEBUG */
  RotateLeftCarry(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x15(){  /* RL L */
  PrintDebug("RL L");  /* DEBUG */
  RotateLeftCarry(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x16(){  /* RL (HL) */
  PrintDebug("RL (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  RotateLeftCarry(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x17(){  /* RL A */
  PrintDebug("RL A");  /* DEBUG */
  RotateLeftCarry(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x18(){  /* RR B */
  PrintDebug("RR B");  /* DEBUG */
  RotateRightCarry(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x19(){  /* RR C */
  PrintDebug("RR C");  /* DEBUG */
  RotateRightCarry(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1A(){  /* RR D */
  PrintDebug("RR D");  /* DEBUG */
  RotateRightCarry(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1B(){  /* RR E */
  PrintDebug("RR E");  /* DEBUG */
  RotateRightCarry(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1C(){  /* RR H */
  PrintDebug("RR H");  /* DEBUG */
  RotateRightCarry(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1D(){  /* RR L */
  PrintDebug("RR L");  /* DEBUG */
  RotateRightCarry(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1E(){  /* RR (HL) */
  PrintDebug("RR (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  RotateRightCarry(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x1F(){  /* RR A */
  PrintDebug("RR A");  /* DEBUG */
  RotateRightCarry(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x20(){  /* SLA B */
  PrintDebug("SLA B");  /* DEBUG */
  ShiftSLA(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x21(){  /* SLA C */
  PrintDebug("SLA C");  /* DEBUG */
  ShiftSLA(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x22(){  /* SLA D */
  PrintDebug("SLA D");  /* DEBUG */
  ShiftSLA(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x23(){  /* SLA E */
  PrintDebug("SLA E");  /* DEBUG */
  ShiftSLA(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x24(){  /* SLA H */
  PrintDebug("SLA H");  /* DEBUG */
  ShiftSLA(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x25(){  /* SLA L */
  PrintDebug("SLA L");  /* DEBUG */
  ShiftSLA(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x26(){  /* SLA (HL) */
  PrintDebug("SLA (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ShiftSLA(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x27(){  /* SLA A */
  PrintDebug("SLA A");  /* DEBUG */
  ShiftSLA(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x28(){  /* SRA B */
  PrintDebug("SRA B");  /* DEBUG */
  ShiftSRA(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x29(){  /* SRA C */
  PrintDebug("SRA C");  /* DEBUG */
  ShiftSRA(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2A(){  /* SRA D */
  PrintDebug("SRA D");  /* DEBUG */
  ShiftSRA(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2B(){  /* SRA E */
  PrintDebug("SRA E");  /* DEBUG */
  ShiftSRA(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2C(){  /* SRA H */
  PrintDebug("SRA H");  /* DEBUG */
  ShiftSRA(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2D(){  /* SRA L */
  PrintDebug("SRA L");  /* DEBUG */
  ShiftSRA(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2E(){  /* SRA (HL) */
  PrintDebug("SRA (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ShiftSRA(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x2F(){  /* SRA A */
  PrintDebug("SRA A");  /* DEBUG */
  ShiftSRA(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x30(){  /* SWAP B (swaps high and low nibbles of B) */
  PrintDebug("SWAP B");  /* DEBUG */
  SwapUbyte(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x31(){  /* SWAP C (swaps high and low nibbles of C) */
  PrintDebug("SWAP C");  /* DEBUG */
  SwapUbyte(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x32(){  /* SWAP D (swaps high and low nibbles of D) */
  PrintDebug("SWAP D");  /* DEBUG */
  SwapUbyte(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x33(){  /* SWAP E (swaps high and low nibbles of E) */
  PrintDebug("SWAP E");  /* DEBUG */
  SwapUbyte(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x34(){  /* SWAP H (swaps high and low nibbles of H) */
  PrintDebug("SWAP H");  /* DEBUG */
  SwapUbyte(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x35(){  /* SWAP L (swaps high and low nibbles of L) */
  PrintDebug("SWAP L");  /* DEBUG */
  SwapUbyte(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x36(){  /* SWAP (HL) (swaps high and low nibbles of value at address HL) */
  PrintDebug("SWAP (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SwapUbyte(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x37(){  /* SWAP A (swaps high and low nibbles of A) */
  PrintDebug("SWAP A");  /* DEBUG */
  SwapUbyte(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x38(){  /* SRL B  (Shift Right B) */
  PrintDebug("SRL B");  /* DEBUG */
  ShiftRightSRL(&Register.B);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x39(){  /* SRL C  (Shift Right C) */
  PrintDebug("SRL C");  /* DEBUG */
  ShiftRightSRL(&Register.C);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3A(){  /* SRL D  (Shift Right D) */
  PrintDebug("SRL D");  /* DEBUG */
  ShiftRightSRL(&Register.D);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3B(){  /* SRL E  (Shift Right E) */
  PrintDebug("SRL E");  /* DEBUG */
  ShiftRightSRL(&Register.E);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3C(){  /* SRL H  (Shift Right H) */
  PrintDebug("SRL H");  /* DEBUG */
  ShiftRightSRL(&Register.H);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3D(){  /* SRL L  (Shift Right L) */
  PrintDebug("SRL L");  /* DEBUG */
  ShiftRightSRL(&Register.L);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3E(){  /* SRL (HL)  (Shift Right value at address HL) */
  PrintDebug("SRL (HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ShiftRightSRL(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);   /* Shift right and set some flags */
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x3F(){  /* SRL A  (Shift Right A) */
  PrintDebug("SRL A");  /* DEBUG */
  ShiftRightSRL(&Register.A);  /* Shift right and set some flags */
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x40(){ /* BIT 0,B (Test bit 0 of register B) */
  PrintDebug("BIT 0,B");  /* DEBUG */
  TestBit0(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x41(){ /* BIT 0,C (Test bit 0 of register C) */
  PrintDebug("BIT 0,C");  /* DEBUG */
  TestBit0(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x42(){ /* BIT 0,D (Test bit 0 of register D) */
  PrintDebug("BIT 0,D");  /* DEBUG */
  TestBit0(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x43(){ /* BIT 0,E (Test bit 0 of register E) */
  PrintDebug("BIT 0,E");  /* DEBUG */
  TestBit0(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x44(){ /* BIT 0,H (Test bit 0 of register H) */
  PrintDebug("BIT 0,H");  /* DEBUG */
  TestBit0(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x45(){ /* BIT 0,L (Test bit 0 of register L) */
  PrintDebug("BIT 0,L");  /* DEBUG */
  TestBit0(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x46(){ /* BIT 0,(HL) (Test bit 0 of value at address HL) */
  PrintDebug("BIT 0,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit0(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x47(){ /* BIT 0,A (Test bit 0 of register A) */
  PrintDebug("BIT 0,A");  /* DEBUG */
  TestBit0(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x48(){ /* BIT 1,B (Test bit 1 of register B) */
  PrintDebug("BIT 1,B");  /* DEBUG */
  TestBit1(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x49(){ /* BIT 1,C (Test bit 1 of register C) */
  PrintDebug("BIT 1,C");  /* DEBUG */
  TestBit1(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4A(){ /* BIT 1,D (Test bit 1 of register D) */
  PrintDebug("BIT 1,D");  /* DEBUG */
  TestBit1(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4B(){ /* BIT 1,E (Test bit 1 of register E) */
  PrintDebug("BIT 1,E");  /* DEBUG */
  TestBit1(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4C(){ /* BIT 1,H (Test bit 1 of register H) */
  PrintDebug("BIT 1,H");  /* DEBUG */
  TestBit1(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4D(){ /* BIT 1,L (Test bit 1 of register L) */
  PrintDebug("BIT 1,L");  /* DEBUG */
  TestBit1(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4E(){ /* BIT 1,(HL) (Test bit 1 of value at address HL) */
  PrintDebug("BIT 1,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit1(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x4F(){ /* BIT 1,A (Test bit 1 of register A) */
  PrintDebug("BIT 1,A");  /* DEBUG */
  TestBit1(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x50(){ /* BIT 2,B (Test bit 2 of register B) */
  PrintDebug("BIT 2,B");  /* DEBUG */
  TestBit2(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x51(){ /* BIT 2,C (Test bit 2 of register C) */
  PrintDebug("BIT 2,C");  /* DEBUG */
  TestBit2(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x52(){ /* BIT 2,D (Test bit 2 of register D) */
  PrintDebug("BIT 2,D");  /* DEBUG */
  TestBit2(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x53(){ /* BIT 2,E (Test bit 2 of register E) */
  PrintDebug("BIT 2,E");  /* DEBUG */
  TestBit2(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x54(){ /* BIT 2,H (Test bit 2 of register H) */
  PrintDebug("BIT 2,H");  /* DEBUG */
  TestBit2(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x55(){ /* BIT 2,L (Test bit 2 of register L) */
  PrintDebug("BIT 2,L");  /* DEBUG */
  TestBit2(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x56(){ /* BIT 2,(HL) (Test bit 2 of value at [HL]) */
  PrintDebug("BIT 2,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit2(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x57(){ /* BIT 2,A (Test bit 2 of register A) */
  PrintDebug("BIT 2,A");  /* DEBUG */
  TestBit2(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x58(){ /* BIT 3,B (Test bit 3 of register B) */
  PrintDebug("BIT 3,B");  /* DEBUG */
  TestBit3(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x59(){ /* BIT 3,C (Test bit 3 of register C) */
  PrintDebug("BIT 3,C");  /* DEBUG */
  TestBit3(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5A(){ /* BIT 3,D (Test bit 3 of register D) */
  PrintDebug("BIT 3,D");  /* DEBUG */
  TestBit3(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5B(){ /* BIT 3,E (Test bit 3 of register E) */
  PrintDebug("BIT 3,E");  /* DEBUG */
  TestBit3(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5C(){ /* BIT 3,H (Test bit 3 of register H) */
  PrintDebug("BIT 3,H");  /* DEBUG */
  TestBit3(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5D(){ /* BIT 3,L (Test bit 3 of register L) */
  PrintDebug("BIT 3,L");  /* DEBUG */
  TestBit3(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5E(){ /* BIT 3,(HL) (Test bit 3 of value at [HL]) */
  PrintDebug("BIT 3,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit3(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x5F(){ /* BIT 3,A (Test bit 3 of register A) */
  PrintDebug("BIT 3,A");  /* DEBUG */
  TestBit3(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x60(){ /* BIT 4,B (Test bit 4 of register B) */
  PrintDebug("BIT 4,B");  /* DEBUG */
  TestBit4(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x61(){ /* BIT 4,C (Test bit 4 of register C) */
  PrintDebug("BIT 4,C");  /* DEBUG */
  TestBit4(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x62(){ /* BIT 4,D (Test bit 4 of register D) */
  PrintDebug("BIT 4,D");  /* DEBUG */
  TestBit4(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x63(){ /* BIT 4,E (Test bit 4 of register E) */
  PrintDebug("BIT 4,E");  /* DEBUG */
  TestBit4(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x64(){ /* BIT 4,H (Test bit 4 of register H) */
  PrintDebug("BIT 4,H");  /* DEBUG */
  TestBit4(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x65(){ /* BIT 4,L (Test bit 4 of register L) */
  PrintDebug("BIT 4,L");  /* DEBUG */
  TestBit4(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x66(){ /* BIT 4,(HL) (Test bit 4 of value at [HL]) */
  PrintDebug("BIT 4,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit4(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x67(){ /* BIT 4,A (Test bit 4 of register A) */
  PrintDebug("BIT 4,A");  /* DEBUG */
  TestBit4(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x68(){ /* BIT 5,B (Test bit 5 of register B) */
  PrintDebug("BIT 5,B");  /* DEBUG */
  TestBit5(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x69(){ /* BIT 5,C (Test bit 5 of register C) */
  PrintDebug("BIT 5,C");  /* DEBUG */
  TestBit5(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6A(){ /* BIT 5,D (Test bit 5 of register D) */
  PrintDebug("BIT 5,D");  /* DEBUG */
  TestBit5(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6B(){ /* BIT 5,E (Test bit 5 of register E) */
  PrintDebug("BIT 5,E");  /* DEBUG */
  TestBit5(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6C(){ /* BIT 5,H (Test bit 5 of register H) */
  PrintDebug("BIT 5,H");  /* DEBUG */
  TestBit5(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6D(){ /* BIT 5,L (Test bit 5 of register L) */
  PrintDebug("BIT 5,L");  /* DEBUG */
  TestBit5(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6E(){ /* BIT 5,(HL) (Test bit 5 of value at [HL]) */
  PrintDebug("BIT 5,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit5(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x6F(){ /* BIT 5,A (Test bit 5 of register A) */
  PrintDebug("BIT 5,A");  /* DEBUG */
  TestBit5(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x70(){ /* BIT 6,B (Test bit 6 of register B) */
  PrintDebug("BIT 6,B");  /* DEBUG */
  TestBit6(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x71(){ /* BIT 6,C (Test bit 6 of register C) */
  PrintDebug("BIT 6,C");  /* DEBUG */
  TestBit6(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x72(){ /* BIT 6,D (Test bit 6 of register D) */
  PrintDebug("BIT 6,D");  /* DEBUG */
  TestBit6(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x73(){ /* BIT 6,E (Test bit 6 of register E) */
  PrintDebug("BIT 6,E");  /* DEBUG */
  TestBit6(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x74(){ /* BIT 6,H (Test bit 6 of register H) */
  PrintDebug("BIT 6,H");  /* DEBUG */
  TestBit6(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x75(){ /* BIT 6,L (Test bit 6 of register L) */
  PrintDebug("BIT 6,L");  /* DEBUG */
  TestBit6(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x76(){ /* BIT 6,(HL) (Test bit 6 of value at [HL]) */
  PrintDebug("BIT 6,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit6(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x77(){ /* BIT 6,A (Test bit 6 of register A) */
  PrintDebug("BIT 6,A");  /* DEBUG */
  TestBit6(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x78(){ /* BIT 7,B (Test bit 7 of register B) */
  PrintDebug("BIT 7,B");  /* DEBUG */
  TestBit7(Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x79(){ /* BIT 7,C (Test bit 7 of register C) */
  PrintDebug("BIT 7,C");  /* DEBUG */
  TestBit7(Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7A(){ /* BIT 7,D (Test bit 7 of register D) */
  PrintDebug("BIT 7,D");  /* DEBUG */
  TestBit7(Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7B(){ /* BIT 7,E (Test bit 7 of register E) */
  PrintDebug("BIT 7,E");  /* DEBUG */
  TestBit7(Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7C(){ /* BIT 7,H (Test bit 7 of register H) */
  PrintDebug("BIT 7,H");  /* DEBUG */
  TestBit7(Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7D(){ /* BIT 7,L (Test bit 7 of register L) */
  PrintDebug("BIT 7,L");  /* DEBUG */
  TestBit7(Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7E(){ /* BIT 7,(HL) (Test bit 7 of value at address HL) */
  PrintDebug("BIT 7,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  TestBit7(UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x7F(){ /* BIT 7,A (Test bit 7 of register A) */
  PrintDebug("BIT 7,A");  /* DEBUG */
  TestBit7(Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x80(){ /* RES 0,B (Reset bit 0 of register B) */
  PrintDebug("RES 0,B");  /* DEBUG */
  ResetBit0(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x81(){ /* RES 0,C (Reset bit 0 of register C) */
  PrintDebug("RES 0,C");  /* DEBUG */
  ResetBit0(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x82(){ /* RES 0,D (Reset bit 0 of register D) */
  PrintDebug("RES 0,D");  /* DEBUG */
  ResetBit0(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x83(){ /* RES 0,E (Reset bit 0 of register E) */
  PrintDebug("RES 0,E");  /* DEBUG */
  ResetBit0(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x84(){ /* RES 0,H (Reset bit 0 of register H) */
  PrintDebug("RES 0,H");  /* DEBUG */
  ResetBit0(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x85(){ /* RES 0,L (Reset bit 0 of register L) */
  PrintDebug("RES 0,L");  /* DEBUG */
  ResetBit0(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x86(){ /* RES 0,(HL) (Reset bit 0 of value at [HL]) */
  PrintDebug("RES 0,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit0(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x87(){ /* RES 0,A (Reset bit 0 of register A) */
  PrintDebug("RES 0,A");  /* DEBUG */
  ResetBit0(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x88(){ /* RES 1,B (Reset bit 1 of register B) */
  PrintDebug("RES 1,B");  /* DEBUG */
  ResetBit1(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x89(){ /* RES 1,C (Reset bit 1 of register C) */
  PrintDebug("RES 1,C");  /* DEBUG */
  ResetBit1(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8A(){ /* RES 1,D (Reset bit 1 of register D) */
  PrintDebug("RES 1,D");  /* DEBUG */
  ResetBit1(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8B(){ /* RES 1,E (Reset bit 1 of register E) */
  PrintDebug("RES 1,E");  /* DEBUG */
  ResetBit1(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8C(){ /* RES 1,H (Reset bit 1 of register H) */
  PrintDebug("RES 1,H");  /* DEBUG */
  ResetBit1(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8D(){ /* RES 1,L (Reset bit 1 of register L) */
  PrintDebug("RES 1,L");  /* DEBUG */
  ResetBit1(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8E(){ /* RES 1,(HL) (Reset bit 1 of value at [HL]) */
  PrintDebug("RES 1,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit1(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x8F(){ /* RES 1,A (Reset bit 1 of register A) */
  PrintDebug("RES 1,A");  /* DEBUG */
  ResetBit1(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x90(){ /* RES 2,B (Reset bit 2 of register B) */
  PrintDebug("RES 2,B");  /* DEBUG */
  ResetBit2(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x91(){ /* RES 2,C (Reset bit 2 of register C) */
  PrintDebug("RES 2,C");  /* DEBUG */
  ResetBit2(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x92(){ /* RES 2,D (Reset bit 2 of register D) */
  PrintDebug("RES 2,D");  /* DEBUG */
  ResetBit2(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x93(){ /* RES 2,E (Reset bit 2 of register E) */
  PrintDebug("RES 2,E");  /* DEBUG */
  ResetBit2(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x94(){ /* RES 2,H (Reset bit 2 of register H) */
  PrintDebug("RES 2,H");  /* DEBUG */
  ResetBit2(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x95(){ /* RES 2,L (Reset bit 2 of register L) */
  PrintDebug("RES 2,L");  /* DEBUG */
  ResetBit2(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x96(){ /* RES 2,(HL) (Reset bit 2 of value at [HL]) */
  PrintDebug("RES 2,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit2(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x97(){ /* RES 2,A (Reset bit 2 of register A) */
  PrintDebug("RES 2,A");  /* DEBUG */
  ResetBit2(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x98(){ /* RES 3,B (Reset bit 3 of register B) */
  PrintDebug("RES 3,B");  /* DEBUG */
  ResetBit3(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x99(){ /* RES 3,C (Reset bit 3 of register C) */
  PrintDebug("RES 3,C");  /* DEBUG */
  ResetBit3(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9A(){ /* RES 3,D (Reset bit 3 of register D) */
  PrintDebug("RES 3,D");  /* DEBUG */
  ResetBit3(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9B(){ /* RES 3,E (Reset bit 3 of register E) */
  PrintDebug("RES 3,E");  /* DEBUG */
  ResetBit3(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9C(){ /* RES 3,H (Reset bit 3 of register H) */
  PrintDebug("RES 3,H");  /* DEBUG */
  ResetBit3(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9D(){ /* RES 3,L (Reset bit 3 of register L) */
  PrintDebug("RES 3,L");  /* DEBUG */
  ResetBit3(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9E(){ /* RES 3,(HL) (Reset bit 3 of value at [HL]) */
  PrintDebug("RES 3,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit3(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0x9F(){ /* RES 3,A (Reset bit 3 of register A) */
  PrintDebug("RES 3,A");  /* DEBUG */
  ResetBit3(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA0(){ /* RES 4,B (Reset bit 4 of register B) */
  PrintDebug("RES 4,B");  /* DEBUG */
  ResetBit4(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA1(){ /* RES 4,C (Reset bit 4 of register C) */
  PrintDebug("RES 4,C");  /* DEBUG */
  ResetBit4(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA2(){ /* RES 4,D (Reset bit 4 of register D) */
  PrintDebug("RES 4,D");  /* DEBUG */
  ResetBit4(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA3(){ /* RES 4,E (Reset bit 4 of register E) */
  PrintDebug("RES 4,E");  /* DEBUG */
  ResetBit4(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA4(){ /* RES 4,H (Reset bit 4 of register H) */
  PrintDebug("RES 4,H");  /* DEBUG */
  ResetBit4(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA5(){ /* RES 4,L (Reset bit 4 of register L) */
  PrintDebug("RES 4,L");  /* DEBUG */
  ResetBit4(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA6(){ /* RES 4,(HL) (Reset bit 4 of value at [HL]) */
  PrintDebug("RES 4,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit4(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA7(){ /* RES 4,A (Reset bit 4 of register A) */
  PrintDebug("RES 4,A");  /* DEBUG */
  ResetBit4(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA8(){ /* RES 5,B (Reset bit 5 of register B) */
  PrintDebug("RES 5,B");  /* DEBUG */
  ResetBit5(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xA9(){ /* RES 5,C (Reset bit 5 of register C) */
  PrintDebug("RES 5,C");  /* DEBUG */
  ResetBit5(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAA(){ /* RES 5,D (Reset bit 5 of register D) */
  PrintDebug("RES 5,D");  /* DEBUG */
  ResetBit5(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAB(){ /* RES 5,E (Reset bit 5 of register E) */
  PrintDebug("RES 5,E");  /* DEBUG */
  ResetBit5(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAC(){ /* RES 5,H (Reset bit 5 of register H) */
  PrintDebug("RES 5,H");  /* DEBUG */
  ResetBit5(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAD(){ /* RES 5,L (Reset bit 5 of register L) */
  PrintDebug("RES 5,L");  /* DEBUG */
  ResetBit5(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAE(){ /* RES 5,(HL) (Reset bit 5 of value at [HL]) */
  PrintDebug("RES 5,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit5(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xAF(){ /* RES 5,A (Reset bit 5 of register A) */
  PrintDebug("RES 5,A");  /* DEBUG */
  ResetBit5(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB0(){ /* RES 6,B (Reset bit 6 of register B) */
  PrintDebug("RES 6,B");  /* DEBUG */
  ResetBit6(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB1(){ /* RES 6,C (Reset bit 6 of register C) */
  PrintDebug("RES 6,C");  /* DEBUG */
  ResetBit6(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB2(){ /* RES 6,D (Reset bit 6 of register D) */
  PrintDebug("RES 6,D");  /* DEBUG */
  ResetBit6(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB3(){ /* RES 6,E (Reset bit 6 of register E) */
  PrintDebug("RES 6,E");  /* DEBUG */
  ResetBit6(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB4(){ /* RES 6,H (Reset bit 6 of register H) */
  PrintDebug("RES 6,H");  /* DEBUG */
  ResetBit6(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB5(){ /* RES 6,L (Reset bit 6 of register L) */
  PrintDebug("RES 6,L");  /* DEBUG */
  ResetBit6(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB6(){ /* RES 6,(HL) (Reset bit 6 of value at [HL]) */
  PrintDebug("RES 6,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit6(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB7(){ /* RES 6,A (Reset bit 6 of register A) */
  PrintDebug("RES 6,A");  /* DEBUG */
  ResetBit6(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB8(){ /* RES 7,B (Reset bit 7 of register B) */
  PrintDebug("RES 7,B");  /* DEBUG */
  ResetBit7(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xB9(){ /* RES 7,C (Reset bit 7 of register C) */
  PrintDebug("RES 7,C");  /* DEBUG */
  ResetBit7(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBA(){ /* RES 7,D (Reset bit 7 of register D) */
  PrintDebug("RES 7,D");  /* DEBUG */
  ResetBit7(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBB(){ /* RES 7,E (Reset bit 7 of register E) */
  PrintDebug("RES 7,E");  /* DEBUG */
  ResetBit7(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBC(){ /* RES 7,H (Reset bit 7 of register H) */
  PrintDebug("RES 7,H");  /* DEBUG */
  ResetBit7(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBD(){ /* RES 7,L (Reset bit 7 of register L) */
  PrintDebug("RES 7,L");  /* DEBUG */
  ResetBit7(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBE(){ /* RES 7,(HL) (Reset bit 7 of value at [HL]) */
  PrintDebug("RES 7,(HL)");  /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  ResetBit7(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xBF(){ /* RES 7,A (Reset bit 7 of register A) */
  PrintDebug("RES 7,A");  /* DEBUG */
  ResetBit7(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC0(){  /* SET 0,B  (Set bit 0 of B) */
  PrintDebug("SET 0,B");   /* DEBUG */
  SetBit0(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC1(){  /* SET 0,C  (Set bit 0 of C) */
  PrintDebug("SET 0,C");   /* DEBUG */
  SetBit0(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC2(){  /* SET 0,D  (Set bit 0 of D) */
  PrintDebug("SET 0,D");   /* DEBUG */
  SetBit0(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC3(){  /* SET 0,E  (Set bit 0 of E) */
  PrintDebug("SET 0,E");   /* DEBUG */
  SetBit0(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC4(){  /* SET 0,H  (Set bit 0 of H) */
  PrintDebug("SET 0,H");   /* DEBUG */
  SetBit0(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC5(){  /* SET 0,L  (Set bit 0 of L) */
  PrintDebug("SET 0,L");   /* DEBUG */
  SetBit0(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC6(){  /* SET 0,(HL)  (Set bit 0 of value at HL) */
  PrintDebug("SET 0,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit0(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC7(){  /* SET 0,A  (Set bit 0 of A) */
  PrintDebug("SET 0,A");   /* DEBUG */
  SetBit0(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC8(){  /* SET 1,B  (Set bit 1 of B) */
  PrintDebug("SET 1,B");   /* DEBUG */
  SetBit1(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xC9(){  /* SET 1,C  (Set bit 1 of C) */
  PrintDebug("SET 1,C");   /* DEBUG */
  SetBit1(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCA(){  /* SET 1,D  (Set bit 1 of D) */
  PrintDebug("SET 1,D");   /* DEBUG */
  SetBit1(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCB(){  /* SET 1,E  (Set bit 1 of E) */
  PrintDebug("SET 1,E");   /* DEBUG */
  SetBit1(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCC(){  /* SET 1,H  (Set bit 1 of H) */
  PrintDebug("SET 1,H");   /* DEBUG */
  SetBit1(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCD(){  /* SET 1,L  (Set bit 1 of L) */
  PrintDebug("SET 1,L");   /* DEBUG */
  SetBit1(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCE(){  /* SET 1,(HL)  (Set bit 1 of value at HL) */
  PrintDebug("SET 1,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit1(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xCF(){  /* SET 1,A  (Set bit 1 of A) */
  PrintDebug("SET 1,A");   /* DEBUG */
  SetBit1(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD0(){  /* SET 2,B  (Set bit 2 of B) */
  PrintDebug("SET 2,B");   /* DEBUG */
  SetBit2(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD1(){  /* SET 2,C  (Set bit 2 of C) */
  PrintDebug("SET 2,C");   /* DEBUG */
  SetBit2(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD2(){  /* SET 2,D  (Set bit 2 of D) */
  PrintDebug("SET 2,D");   /* DEBUG */
  SetBit2(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD3(){  /* SET 2,E  (Set bit 2 of E) */
  PrintDebug("SET 2,E");   /* DEBUG */
  SetBit2(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD4(){  /* SET 2,H  (Set bit 2 of H) */
  PrintDebug("SET 2,H");   /* DEBUG */
  SetBit2(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD5(){  /* SET 2,L  (Set bit 2 of L) */
  PrintDebug("SET 2,L");   /* DEBUG */
  SetBit2(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD6(){  /* SET 2,(HL)  (Set bit 2 of value at HL) */
  PrintDebug("SET 2,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit2(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD7(){  /* SET 2,A  (Set bit 2 of A) */
  PrintDebug("SET 2,A");   /* DEBUG */
  SetBit2(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD8(){  /* SET 3,B  (Set bit 3 of B) */
  PrintDebug("SET 3,B");   /* DEBUG */
  SetBit3(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xD9(){  /* SET 3,C  (Set bit 3 of C) */
  PrintDebug("SET 3,C");   /* DEBUG */
  SetBit3(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDA(){  /* SET 3,D  (Set bit 3 of D) */
  PrintDebug("SET 3,D");   /* DEBUG */
  SetBit3(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDB(){  /* SET 3,E  (Set bit 3 of E) */
  PrintDebug("SET 3,E");   /* DEBUG */
  SetBit3(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDC(){  /* SET 3,H  (Set bit 3 of H) */
  PrintDebug("SET 3,H");   /* DEBUG */
  SetBit3(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDD(){  /* SET 3,L  (Set bit 3 of L) */
  PrintDebug("SET 3,L");   /* DEBUG */
  SetBit3(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDE(){  /* SET 3,(HL)  (Set bit 3 of value at HL) */
  PrintDebug("SET 3,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit3(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xDF(){  /* SET 3,A  (Set bit 3 of A) */
  PrintDebug("SET 3,A");   /* DEBUG */
  SetBit3(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE0(){  /* SET 4,B  (Set bit 4 of B) */
  PrintDebug("SET 4,B");   /* DEBUG */
  SetBit4(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE1(){  /* SET 4,C  (Set bit 4 of C) */
  PrintDebug("SET 4,C");   /* DEBUG */
  SetBit4(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE2(){  /* SET 4,D  (Set bit 4 of D) */
  PrintDebug("SET 4,D");   /* DEBUG */
  SetBit4(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE3(){  /* SET 4,E  (Set bit 4 of E) */
  PrintDebug("SET 4,E");   /* DEBUG */
  SetBit4(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE4(){  /* SET 4,H  (Set bit 4 of H) */
  PrintDebug("SET 4,H");   /* DEBUG */
  SetBit4(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE5(){  /* SET 4,L  (Set bit 4 of L) */
  PrintDebug("SET 4,L");   /* DEBUG */
  SetBit4(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE6(){  /* SET 4,(HL)  (Set bit 4 of value at HL) */
  PrintDebug("SET 4,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit4(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE7(){  /* SET 4,A  (Set bit 4 of A) */
  PrintDebug("SET 4,A");   /* DEBUG */
  SetBit4(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE8(){  /* SET 5,B  (Set bit 5 of B) */
  PrintDebug("SET 5,B");   /* DEBUG */
  SetBit5(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xE9(){  /* SET 5,C  (Set bit 5 of C) */
  PrintDebug("SET 5,C");   /* DEBUG */
  SetBit5(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xEA(){  /* SET 5,D  (Set bit 5 of D) */
  PrintDebug("SET 5,D");   /* DEBUG */
  SetBit5(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xEB(){  /* SET 5,E  (Set bit 5 of E) */
  PrintDebug("SET 5,E");   /* DEBUG */
  SetBit5(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xEC(){  /* SET 5,H  (Set bit 5 of H) */
  PrintDebug("SET 5,H");   /* DEBUG */
  SetBit5(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xED(){  /* SET 5,L  (Set bit 5 of L) */
  PrintDebug("SET 5,L");   /* DEBUG */
  SetBit5(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xEE(){  /* SET 5,(HL)  (Set bit 5 of value at HL) */
  PrintDebug("SET 5,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit5(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xEF(){  /* SET 5,A  (Set bit 5 of A) */
  PrintDebug("SET 5,A");   /* DEBUG */
  SetBit5(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF0(){  /* SET 6,B  (Set bit 6 of B) */
  PrintDebug("SET 6,B");   /* DEBUG */
  SetBit6(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF1(){  /* SET 6,C  (Set bit 6 of C) */
  PrintDebug("SET 6,C");   /* DEBUG */
  SetBit6(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF2(){  /* SET 6,D  (Set bit 6 of D) */
  PrintDebug("SET 6,D");   /* DEBUG */
  SetBit6(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF3(){  /* SET 6,E  (Set bit 6 of E) */
  PrintDebug("SET 6,E");   /* DEBUG */
  SetBit6(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF4(){  /* SET 6,H  (Set bit 6 of H) */
  PrintDebug("SET 6,H");   /* DEBUG */
  SetBit6(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF5(){  /* SET 6,L  (Set bit 6 of L) */
  PrintDebug("SET 6,L");   /* DEBUG */
  SetBit6(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF6(){  /* SET 6,(HL)  (Set bit 6 of value at HL) */
  PrintDebug("SET 6,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit6(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF7(){  /* SET 6,A  (Set bit 6 of A) */
  PrintDebug("SET 6,A");   /* DEBUG */
  SetBit6(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF8(){  /* SET 7,B  (Set bit 7 of B) */
  PrintDebug("SET 7,B");   /* DEBUG */
  SetBit7(&Register.B);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xF9(){  /* SET 7,C  (Set bit 7 of C) */
  PrintDebug("SET 7,C");   /* DEBUG */
  SetBit7(&Register.C);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFA(){  /* SET 7,D  (Set bit 7 of D) */
  PrintDebug("SET 7,D");   /* DEBUG */
  SetBit7(&Register.D);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFB(){  /* SET 7,E  (Set bit 7 of E) */
  PrintDebug("SET 7,E");   /* DEBUG */
  SetBit7(&Register.E);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFC(){  /* SET 7,H  (Set bit 7 of H) */
  PrintDebug("SET 7,H");   /* DEBUG */
  SetBit7(&Register.H);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFD(){  /* SET 7,L  (Set bit 7 of L) */
  PrintDebug("SET 7,L");   /* DEBUG */
  SetBit7(&Register.L);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFE(){  /* SET 7,(HL)  (Set bit 7 of value at HL) */
  PrintDebug("SET 7,(HL)");   /* DEBUG */
  uint8_t UbyteBuff1 = MemoryRead(ReadRegHL());
  SetBit7(&UbyteBuff1);
  MemoryWrite(ReadRegHL(), UbyteBuff1);
  Register.PC += 2;
  return 16;  /* that many CPU cycles should be spent on this instruction */
}
int OP2_0xFF(){  /* SET 7,A  (Set bit 7 of A) */
  PrintDebug("SET 7,A");   /* DEBUG */
  SetBit7(&Register.A);
  Register.PC += 2;
  return 8;  /* that many CPU cycles should be spent on this instruction */
}

typedef int (*OP_T)();

const OP_T OP[] = {
  OP_0x00, OP_0x01, OP_0x02, OP_0x03, OP_0x04, OP_0x05, OP_0x06, OP_0x07, OP_0x08, OP_0x09, OP_0x0A, OP_0x0B, OP_0x0C, OP_0x0D, OP_0x0E, OP_0x0F, OP_0x10, OP_0x11, OP_0x12, OP_0x13, OP_0x14, OP_0x15, OP_0x16, OP_0x17, OP_0x18, OP_0x19, OP_0x1A, OP_0x1B, OP_0x1C, OP_0x1D, OP_0x1E, OP_0x1F, OP_0x20, OP_0x21, OP_0x22, OP_0x23, OP_0x24, OP_0x25, OP_0x26, OP_0x27, OP_0x28, OP_0x29, OP_0x2A, OP_0x2B, OP_0x2C, OP_0x2D, OP_0x2E, OP_0x2F, OP_0x30, OP_0x31, OP_0x32, OP_0x33, OP_0x34, OP_0x35, OP_0x36, OP_0x37, OP_0x38, OP_0x39, OP_0x3A, OP_0x3B, OP_0x3C, OP_0x3D, OP_0x3E, OP_0x3F, OP_0x40, OP_0x41, OP_0x42, OP_0x43, OP_0x44, OP_0x45, OP_0x46, OP_0x47, OP_0x48, OP_0x49, OP_0x4A, OP_0x4B, OP_0x4C, OP_0x4D, OP_0x4E, OP_0x4F, OP_0x50, OP_0x51, OP_0x52, OP_0x53, OP_0x54, OP_0x55, OP_0x56, OP_0x57, OP_0x58, OP_0x59, OP_0x5A, OP_0x5B, OP_0x5C, OP_0x5D, OP_0x5E, OP_0x5F, OP_0x60, OP_0x61, OP_0x62, OP_0x63, OP_0x64, OP_0x65, OP_0x66, OP_0x67, OP_0x68, OP_0x69, OP_0x6A, OP_0x6B, OP_0x6C, OP_0x6D, OP_0x6E, OP_0x6F, OP_0x70, OP_0x71, OP_0x72, OP_0x73, OP_0x74, OP_0x75, OP_0x76, OP_0x77, OP_0x78, OP_0x79, OP_0x7A, OP_0x7B, OP_0x7C, OP_0x7D, OP_0x7E, OP_0x7F, OP_0x80, OP_0x81, OP_0x82, OP_0x83, OP_0x84, OP_0x85, OP_0x86, OP_0x87, OP_0x88, OP_0x89, OP_0x8A, OP_0x8B, OP_0x8C, OP_0x8D, OP_0x8E, OP_0x8F, OP_0x90, OP_0x91, OP_0x92, OP_0x93, OP_0x94, OP_0x95, OP_0x96, OP_0x97, OP_0x98, OP_0x99, OP_0x9A, OP_0x9B, OP_0x9C, OP_0x9D, OP_0x9E, OP_0x9F, OP_0xA0, OP_0xA1, OP_0xA2, OP_0xA3, OP_0xA4, OP_0xA5, OP_0xA6, OP_0xA7, OP_0xA8, OP_0xA9, OP_0xAA, OP_0xAB, OP_0xAC, OP_0xAD, OP_0xAE, OP_0xAF, OP_0xB0, OP_0xB1, OP_0xB2, OP_0xB3, OP_0xB4, OP_0xB5, OP_0xB6, OP_0xB7, OP_0xB8, OP_0xB9, OP_0xBA, OP_0xBB, OP_0xBC, OP_0xBD, OP_0xBE, OP_0xBF, OP_0xC0, OP_0xC1, OP_0xC2, OP_0xC3, OP_0xC4, OP_0xC5, OP_0xC6, OP_0xC7, OP_0xC8, OP_0xC9, OP_0xCA, OP_0xCB, OP_0xCC, OP_0xCD, OP_0xCE, OP_0xCF, OP_0xD0, OP_0xD1, OP_0xD2, OP_0x00, OP_0xD4, OP_0xD5, OP_0xD6, OP_0xD7, OP_0xD8, OP_0xD9, OP_0xDA, OP_0x00, OP_0xDC, OP_0x00, OP_0xDE, OP_0xDF, OP_0xE0, OP_0xE1, OP_0xE2, OP_0x00, OP_0x00, OP_0xE5, OP_0xE6, OP_0xE7, OP_0xE8, OP_0xE9, OP_0xEA, OP_0x00, OP_0x00, OP_0x00, OP_0xEE, OP_0xEF, OP_0xF0, OP_0xF1, OP_0xF2, OP_0xF3, OP_0x00, OP_0xF5, OP_0xF6, OP_0xF7, OP_0xF8, OP_0xF9, OP_0xFA, OP_0xFB, OP_0x00, OP_0x00, OP_0xFE, OP_0xFF
};

const OP_T OP2[] = {
  OP2_0x00, OP2_0x01, OP2_0x02, OP2_0x03, OP2_0x04, OP2_0x05, OP2_0x06, OP2_0x07, OP2_0x08, OP2_0x09, OP2_0x0A, OP2_0x0B, OP2_0x0C, OP2_0x0D, OP2_0x0E, OP2_0x0F, OP2_0x10, OP2_0x11, OP2_0x12, OP2_0x13, OP2_0x14, OP2_0x15, OP2_0x16, OP2_0x17, OP2_0x18, OP2_0x19, OP2_0x1A, OP2_0x1B, OP2_0x1C, OP2_0x1D, OP2_0x1E, OP2_0x1F, OP2_0x20, OP2_0x21, OP2_0x22, OP2_0x23, OP2_0x24, OP2_0x25, OP2_0x26, OP2_0x27, OP2_0x28, OP2_0x29, OP2_0x2A, OP2_0x2B, OP2_0x2C, OP2_0x2D, OP2_0x2E, OP2_0x2F, OP2_0x30, OP2_0x31, OP2_0x32, OP2_0x33, OP2_0x34, OP2_0x35, OP2_0x36, OP2_0x37, OP2_0x38, OP2_0x39, OP2_0x3A, OP2_0x3B, OP2_0x3C, OP2_0x3D, OP2_0x3E, OP2_0x3F, OP2_0x40, OP2_0x41, OP2_0x42, OP2_0x43, OP2_0x44, OP2_0x45, OP2_0x46, OP2_0x47, OP2_0x48, OP2_0x49, OP2_0x4A, OP2_0x4B, OP2_0x4C, OP2_0x4D, OP2_0x4E, OP2_0x4F, OP2_0x50, OP2_0x51, OP2_0x52, OP2_0x53, OP2_0x54, OP2_0x55, OP2_0x56, OP2_0x57, OP2_0x58, OP2_0x59, OP2_0x5A, OP2_0x5B, OP2_0x5C, OP2_0x5D, OP2_0x5E, OP2_0x5F, OP2_0x60, OP2_0x61, OP2_0x62, OP2_0x63, OP2_0x64, OP2_0x65, OP2_0x66, OP2_0x67, OP2_0x68, OP2_0x69, OP2_0x6A, OP2_0x6B, OP2_0x6C, OP2_0x6D, OP2_0x6E, OP2_0x6F, OP2_0x70, OP2_0x71, OP2_0x72, OP2_0x73, OP2_0x74, OP2_0x75, OP2_0x76, OP2_0x77, OP2_0x78, OP2_0x79, OP2_0x7A, OP2_0x7B, OP2_0x7C, OP2_0x7D, OP2_0x7E, OP2_0x7F, OP2_0x80, OP2_0x81, OP2_0x82, OP2_0x83, OP2_0x84, OP2_0x85, OP2_0x86, OP2_0x87, OP2_0x88, OP2_0x89, OP2_0x8A, OP2_0x8B, OP2_0x8C, OP2_0x8D, OP2_0x8E, OP2_0x8F, OP2_0x90, OP2_0x91, OP2_0x92, OP2_0x93, OP2_0x94, OP2_0x95, OP2_0x96, OP2_0x97, OP2_0x98, OP2_0x99, OP2_0x9A, OP2_0x9B, OP2_0x9C, OP2_0x9D, OP2_0x9E, OP2_0x9F, OP2_0xA0, OP2_0xA1, OP2_0xA2, OP2_0xA3, OP2_0xA4, OP2_0xA5, OP2_0xA6, OP2_0xA7, OP2_0xA8, OP2_0xA9, OP2_0xAA, OP2_0xAB, OP2_0xAC, OP2_0xAD, OP2_0xAE, OP2_0xAF, OP2_0xB0, OP2_0xB1, OP2_0xB2, OP2_0xB3, OP2_0xB4, OP2_0xB5, OP2_0xB6, OP2_0xB7, OP2_0xB8, OP2_0xB9, OP2_0xBA, OP2_0xBB, OP2_0xBC, OP2_0xBD, OP2_0xBE, OP2_0xBF, OP2_0xC0, OP2_0xC1, OP2_0xC2, OP2_0xC3, OP2_0xC4, OP2_0xC5, OP2_0xC6, OP2_0xC7, OP2_0xC8, OP2_0xC9, OP2_0xCA, OP2_0xCB, OP2_0xCC, OP2_0xCD, OP2_0xCE, OP2_0xCF, OP2_0xD0, OP2_0xD1, OP2_0xD2, OP2_0xD3, OP2_0xD4, OP2_0xD5, OP2_0xD6, OP2_0xD7, OP2_0xD8, OP2_0xD9, OP2_0xDA, OP2_0xDB, OP2_0xDC, OP2_0xDD, OP2_0xDE, OP2_0xDF, OP2_0xE0, OP2_0xE1, OP2_0xE2, OP2_0xE3, OP2_0xE4, OP2_0xE5, OP2_0xE6, OP2_0xE7, OP2_0xE8, OP2_0xE9, OP2_0xEA, OP2_0xEB, OP2_0xEC, OP2_0xED, OP2_0xEE, OP2_0xEF, OP2_0xF0, OP2_0xF1, OP2_0xF2, OP2_0xF3, OP2_0xF4, OP2_0xF5, OP2_0xF6, OP2_0xF7, OP2_0xF8, OP2_0xF9, OP2_0xFA, OP2_0xFB, OP2_0xFC, OP2_0xFD, OP2_0xFE, OP2_0xFF
};

int OP_0xCB(){  /* Here we have a CBxx opcode... */
  return OP2[ MemoryRead(Register.PC + 1) ]();
}

inline int CpuExec(void) {
  return OP[MemoryRead(Register.PC)]();
}
