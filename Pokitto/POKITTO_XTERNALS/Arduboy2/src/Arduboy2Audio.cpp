/**
 * @file Arduboy2Audio.cpp
 * \brief
 * The Arduboy2Audio class for speaker and sound control.
 */

#include "Arduboy2.h"
#include "Arduboy2Audio.h"


bool Arduboy2Audio::audio_enabled = false;

void Arduboy2Audio::on()
{
  _c.sound.ampEnable(true);
  audio_enabled = true;
}

void Arduboy2Audio::off()
{
  _c.sound.ampEnable(false);
  audio_enabled = false;
}

void Arduboy2Audio::saveOnOff()
{
  // ToDo EEPROM.update(EEPROM_AUDIO_ON_OFF, audio_enabled);
}

void Arduboy2Audio::begin()
{
  // jonnetodo if (EEPROM.read(EEPROM_AUDIO_ON_OFF))
    on();
}

bool Arduboy2Audio::enabled()
{
  return audio_enabled;
}
