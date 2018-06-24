//FX Synth script
void playsoundfx(int fxno, int channel) {
  gb.sound.command(0, soundfx[fxno][6], 0, channel); // set volume
  gb.sound.command(1, soundfx[fxno][0], 0, channel); // set waveform
  gb.sound.command(2, soundfx[fxno][5], -soundfx[fxno][4], channel); // set volume slide
  gb.sound.command(3, soundfx[fxno][3], soundfx[fxno][2] - 58, channel); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1], soundfx[fxno][7], channel); // play note
}

//MUTE ALL
void soundMute() {
  if (gb.buttons.pressed(BTN_B)) {
    switch (sound_volume) {
      case 0: sound_volume = true; break;
      case 1: sound_volume = false; break;
    }
  }
  gb.sound.setVolume(sound_volume, 0);
}
