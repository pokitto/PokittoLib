/*
    Few functions related to numeral conversions
    This file is part of the zBoy project
    Copyright (C) Mateusz Viste 2010, 2011
*/


/* Here I am replacing my overkill (and painfully slow) UbyteToByte() function by a simple DEFINE casting */
#define UbyteToByte(ub) (signed char)(ub)

/* #define DwordVal(FirstByte, SecondByte) ((SecondByte << 8) | FirstByte) */

static inline int DwordVal(register uint8_t FirstByte, register uint8_t SecondByte) {
  return((SecondByte << 8) | FirstByte);
}
