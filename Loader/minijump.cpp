/**************************************************************************/
/*!
    @file     minibuttons.cpp
    @author   Jonne Valola

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Jonne Valola
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "miniload.h"


void m_delayms(unsigned long ms) {
    ms = ms * 4800; //for 48 MHZ clock rate
    while (ms--) {
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    }
}




void boot_jump(unsigned int * vtbp){
//__asm__("LDR SP, [R0]"); // Load new stack pointer address
//__asm__("LDR PC, [R0, #4]"); // Load new program counter address
__disable_irq();
SCB->VTOR = APPL_ADDRESS; /* Change vector table address */
__set_MSP(vtbp[0]); // load SP
((void (*)(void)) vtbp[1])(); // go...
}

/** Set main system clock source
* http://docs.lpcware.com/lpcopen/v1.03/clock__11xx_8c_source.html
* void Chip_Clock_SetMainClockSource(CHIP_SYSCTL_MAINCLKSRC_T src)
*/

void Chip_Clock_SetMainClockSource_To_IRC()
{
  //LPC_SYSCTL->MAINCLKSEL  = 0; // 0 is IRC (uint32_t) src;
  //LPC_SYSCTL->MAINCLKUEN  = 0;
  //LPC_SYSCTL->MAINCLKUEN  = 1;
  //   <o.0..1> System PLL Clock Source Select (SYSPLLCLKSEL)
//        <0=> IRC Oscillator
//        <1=> Crystal Oscillator (SYSOSC)
//        <3=> RTC Oscillator (32 kHz)
  LPC_SYSCON->SYSPLLCLKSEL  = 0x00;             /* Select PLL Input           */
  LPC_SYSCON->SYSPLLCLKUEN  = 0x01;             /* Update Clock Source        */
  LPC_SYSCON->SYSPLLCLKUEN  = 0x00;             /* Toggle Update Register     */
  LPC_SYSCON->SYSPLLCLKUEN  = 0x01;
}

/** start the user application
* https://community.nxp.com/thread/417695
*
*/
void start_application(unsigned long app_link_location){
    //asm(" ldr sp, [r0,#0]");
    //asm(" ldr pc, [r0,#4]");
    //This code is not valid for the Cortex-m0+ instruction set.
    // The equivalent for this (as used by the KL26) is
__disable_irq();// Start by disabling interrupts, before changing interrupt vectors

// delete buttons
pokDeleteButtons();

// completely kill button interrupts in preparation for reset
LPC_PINT->IENR = 0;
LPC_PINT->IENF = 0;

SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk & ~(SysTick_CTRL_ENABLE_Msk); //disable systick
LPC_SYSCON->PDRUNCFG     |=  (1 << 10);       /* Power-down USB PHY         */
LPC_SYSCON->PDRUNCFG     |=  (1 <<  8);       /* Power-down USB PLL         */

// reset clock source to IRC
LPC_SYSCON->MAINCLKUEN    = 0x01;             /* Update MCLK Clock Source   */
LPC_SYSCON->MAINCLKUEN    = 0x00;             /* Toggle Update Register     */
while (LPC_SYSCON->MAINCLKUEN & 0x01);     /* Wait Until Updated         */
// switch clock selection to IRC
LPC_SYSCON->MAINCLKSEL    = 0;             /* Select Clock Source        */
LPC_SYSCON->MAINCLKUEN    = 0x01;             /* Update MCLK Clock Source   */
LPC_SYSCON->MAINCLKUEN    = 0x00;             /* Toggle Update Register     */
while (LPC_SYSCON->MAINCLKUEN & 0x01);     /* Wait Until Updated         */
//disable PLL clock output
LPC_SYSCON->SYSPLLCLKUEN = 0;
while (LPC_SYSCON->SYSPLLCLKUEN & 0x00);
LPC_SYSCON->SYSPLLCTRL = 0;

//kill peripherals
LPC_SYSCON->MAINCLKSEL = 0;
LPC_SYSCON->PRESETCTRL = 0; //disable all peripherals

//power down PLL
volatile uint32_t tmp;
tmp = (LPC_SYSCON->PDRUNCFG & 0x000025FFL);
tmp |= ((1<<7) & 0x000025FFL);
LPC_SYSCON->PDRUNCFG = (tmp | 0x0000C800L); /* Power-down SYSPLL          */

//Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC); //switch to IRC

// clear all gpio states
LPC_GPIO_PORT->PIN[0] = 0;
LPC_GPIO_PORT->PIN[1] = 0;
LPC_GPIO_PORT->PIN[2] = 0;

SCB->VTOR = app_link_location;//APPL_ADDRESS; /* Change vector table address */
asm(" mov r0, %[address]"::[address] "r" (app_link_location));
asm(" ldr r1, [r0,#0]"); // get the stack pointer value from the program's reset vector
asm(" mov sp, r1");      // copy the value to the stack pointer
asm(" ldr r0, [r0,#4]"); // get the program counter value from the program's reset vector
asm(" blx r0");          // jump to the' start address
}

void execute_code(unsigned addr)
{
void (*user_code_entry)(void);
uint32_t *p;
uint32_t *q;

__disable_irq();// Start by disabling interrupts, before changing interrupt vectors
SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk & ~(SysTick_CTRL_ENABLE_Msk); //disable systick
//Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC); //switch to IRC
Chip_Clock_SetMainClockSource_To_IRC(); //switch to IRC


/* Set vector table offset*/
SCB->VTOR = addr;

p = (uint32_t *)addr;
q = p;

/*Set stack pointer to given address*/
__set_MSP(*q);

/*Set address for RESET*/
p++;
user_code_entry = (void (*)(void))(*p);

/*Set stack pointer to given address*/
//I switched these around
//  __set_MSP(*q);

/*Jump to application*/
user_code_entry();

}

