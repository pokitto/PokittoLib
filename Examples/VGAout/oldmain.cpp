#include "Pokitto.h"

//mbed::DigitalOut HSync(EXT1);

mbed::PwmOut HSync(EXT0);
mbed::InterruptIn HSCount(EXT1);
mbed::DigitalOut VSync(EXT2);
mbed::DigitalOut R(EXT4);
mbed::Timeout vstimer;
mbed::PwmOut joo;
LPC_SCT0_Type* PWM;
int counts[0xFF]; uint8_t ccount=0;

#define CLR_VSYNC LPC_GPIO_PORT->CLR[0] = 1 << 12
#define SET_VSYNC LPC_GPIO_PORT->SET[0] = 1 << 12

#define CLR_R LPC_GPIO_PORT->CLR[0] = 1 << 14
#define SET_R LPC_GPIO_PORT->SET[0] = 1 << 14

int lines, vstate, pixcount;

#define VSTATE_DRAW     0
#define VSTATE_FPORCH   1
#define VSTATE_VSYNC    2
#define VSTATE_BPORCH   3

#define LINES_DRAWEND      480
#define LINES_FPORCHEND    LINES_DRAWEND + 10 // 490
#define LINES_VSYNCEND     LINES_FPORCHEND + 2 //492
#define LINES_BPORCHEND    LINES_VSYNCEND + 29 //521

void vsync_lo();void vsync_hi();

void vsync_hi() {
    SET_VSYNC;
    vstimer.detach();
    //vstimer.attach_us(&vsync_lo,16683);

}

void vsync_lo() {
    CLR_VSYNC;
    vstimer.attach_us(&vsync_hi,63);

}

void startpx () {
    pixcount=0;
}

void hsincrement() {
    lines++; pixcount=0;
    //counts[ccount++]=timmy.read_us();
    //timmy.reset();
    /*PWM->COUNT = 1595;
    LPC_PIN_INT_T* t;
    t=(LPC_PIN_INT_T*)0xA0004000;
    t->IST=0;*/

    CLR_R;//blanking period

    switch (vstate) {
    case VSTATE_DRAW:
        if (lines==LINES_DRAWEND) {vstate++; }//SET_VSYNC;}
        break;
    case VSTATE_FPORCH:
        if (lines==LINES_FPORCHEND) {vstate++; }//CLR_VSYNC;}
        break;
    case VSTATE_VSYNC:
        if (lines==LINES_VSYNCEND) {
                vstate++;
                vstimer.attach_us(&vsync_lo,16683-63);
                }//SET_VSYNC;}
        break;
    case VSTATE_BPORCH:
        if (lines==LINES_BPORCHEND) {vstate=0;lines=0;}
        break;
    }
}

int main ()
{
vstate=lines=pixcount=0;

VSync=1;
//HSync.period(0.0000318f); //30.80us
//HSync.pulsewidth(0.000028f); // 3.08us
HSync.period(0.000032f); //30.80us
HSync.pulsewidth(0.000028f); // 3.08us
HSCount.fall(&hsincrement);  // attach interrupt to start of hsync
//HSCount.rise(&hsreset);  // attach interrupt to end of hsync
//pwmout_t* obj = (pwmout_t*)&HSync;
//PWM = obj->pwm;

	while (1)
	{
        int toggle=0;
        while (HSCount) {
                SET_R;
        }
        //counts[ccount++] = PWM->COUNT;
        CLR_R;
	}
}
