#include "Pokitto.h"

typedef struct uFMT_STRUCT {
  short comp_code;
  short num_channels;
  unsigned sample_rate;
  unsigned avg_Bps;
  short block_align;
  short sig_bps;
} FMT_STRUCT;


/** wave file player class.
 *
 * Example:
 * @code
 * #include <mbed.h>
 * #include <wave_player.h>
 *
 * AnalogOut DACout(p18);
 * wave_player waver(&DACout);
 *
 * int main() {
 *  FILE *wave_file;
 *
 *  printf("\n\n\nHello, wave world!\n");
 *  wave_file=fopen("/sd/44_8_st.wav","r");
 *  waver.play(wave_file);
 *  fclose(wave_file);
 * }
 * @endcode
 */
class wave_player {

public:
/** Create a wave player using a pointer to the given AnalogOut object.
 *
 * @param _dac pointer to an AnalogOut object to which the samples are sent.
 */
wave_player();

/** the player function.
 *
 * @param wavefile  A pointer to an opened wave file
 */
void play(FILE *wavefile);

void play2(FILE *wavefile);

void update(FILE *wavefile);

/** Set the printf verbosity of the wave player.  A nonzero verbosity level
 * will put wave_player in a mode where the complete contents of the wave
 * file are echoed to the screen, including header values, and including
 * all of the sample values placed into the DAC FIFO, and the sample values
 * removed from the DAC FIFO by the ISR.  The sample output frequency is
 * fixed at 2 Hz in this mode, so it's all very slow and the DAC output isn't
 * very useful, but it lets you see what's going on and may help for debugging
 * wave files that don't play correctly.
 *
 * @param v the verbosity level
 */
void set_verbosity(int v);

private:
void dac_out(void);
int verbosity;
//AnalogOut *wave_DAC;
Ticker tick;
unsigned short DAC_fifo[256];
unsigned char current_FIFO;
short DAC_wptr;
volatile short DAC_rptr;
short DAC_on;
};


