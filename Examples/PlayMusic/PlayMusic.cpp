#include <Pokitto.h>
#include <Synth.h>

using Pokitto::Core;
using Pokitto::Display;
using Pokitto::Sound;

struct SoundData
{
    uint8_t noteNumber;
    uint8_t instrumentNumber;
};

// Fanfare 1
// The array item consists of the note number and the instrument number, e.g. {29, 1}.
// The instrument number 0 means no instrument, so the previous note is still playing.
const SoundData Fanfare[] =
{
    // A short note
    {39, 1},{255, 0},
    {41, 1},{255, 0},
    {42, 1},{255, 0},
    // a long note
    {43, 1},{255, 0},{255, 0},{255, 0},
    {45, 1},{255, 0},
    {43, 1},{255, 0},{255, 0},{255, 0},
    // Pause until the next note.
    {255, 2}
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
    track1on = true;
    track2on = false;
    track3on = false;

    // Make some instruments to test with.
    makeSampleInstruments();

    // Enable the internal audio amplifier.
    Sound::ampEnable(1);

    // Change the tempo.
    uint32_t tempo = 45;

    // Samples per tick, calculated as a float and then truncated
    samplespertick = static_cast<uint16_t>(((60.0f / tempo) * POK_AUD_FREQ) / 16.0f);

    // Set the instrument 2 to be silent. That can be used as pause.
    patch[2].wave = WOFF;

    // Start playing!
    playing = true;

    // Store the fanfare to the block 0.
    size_t blockIndex = 0;
    size_t songLen = sizeof(Fanfare) / sizeof(Fanfare[0]);
    for(size_t i = 0; ((i < songLen) && (i < PATTERNLENGTH)); ++i)
    {
        block[blockIndex].notenumber[i] = Fanfare[i].noteNumber;
        block[blockIndex].instrument[i] = Fanfare[i].instrumentNumber;
    }

    while (Core::isRunning())
    {
        if (Core::update())
            Display::print("Playing music...");
    }
    return 0;
}