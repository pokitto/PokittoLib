#include "Pokitto.h"
#include "Synth.h"

using Pokitto::Core;
using Pokitto::Display;
using Pokitto::Sound;

// Fanfare 1
// The array item consists of the note number and the instrument number, e.g. {29,1}. The instrument number 0
// means no instrument, so the previous note is still playing.
const uint8_t Fanfare[][2] =
{
    {39, 1},{255, 0},  // A short note
    {41, 1},{255, 0},
    {42, 1},{255, 0},
    {43, 1},{255, 0},{255, 0},{255, 0}, // a long note
    {45, 1},{255, 0},
    {43, 1},{255, 0},{255, 0},{255, 0},
    {255, 2}  // Pause until the next note.
};

int main()
{
    Core::begin();

    // *** Initialize the music player

    // Reset the song data. This contains most of the important song data like
    // - the block sequence,
    // - the current note and instrument streams (arrays)
    // - the current position in the block sequence, and
    // - the song end and loop block indices.
    emptySong();

    // Initialize all the blocks. Each block can contain 64 notes and instrument indices.
    emptyBlocks();

    // Only one simultaneous sound
    track1on = true; track2on = false; track3on = false;

    // Make some instruments to test with.
    makeSampleInstruments();

    // Enable the internal audio amplifier.
    Sound::ampEnable(1);

    // Change the tempo.
    uint32_t tempo = 45;
    samplespertick = (float)((60.0f/(float)tempo)*POK_AUD_FREQ)/16;

    // SEt the instrument 2 to be silent. That can be used as pause.
    patch[2].wave = WOFF;

    // Start playing!
    playing = true;

    // Store the fanfare to the block 0.
    int32_t blockNum = 0;
    int32_t songLen = sizeof(Fanfare) / sizeof(Fanfare[0]);
    for(int32_t i=0; i<songLen && i<PATTERNLENGTH;i++)
    {
        block[blockNum].notenumber[i] = Fanfare[i][0];
        block[blockNum].instrument[i] = Fanfare[i][1];
    }

    while (Core::isRunning())
    {
        if (Core::update())
            Display::print("Playing music...");

    }
    return 0;
}