
bool stopexit=true;

struct Instrument{
    char name[10];
    int8_t wave;
    int8_t arpMode;
    int8_t loop;
    int8_t echo;
    int8_t distort;
    int8_t normalize;
    int8_t ADSR;
    uint8_t attack; //0-255
    uint8_t decay; //0-255
    uint8_t sustain; //0-255
    uint8_t release; //0-255
    int16_t volume; //0-300
    int16_t pitchRate; //-1000, 1000
    int16_t pitchMax; //-6000, 6000
    Instrument() : wave(0), arpMode(0), loop(0), echo(0), distort(0), normalize(0), ADSR(0),
    attack(0), decay(0), sustain(0), release(0), volume(0), pitchRate(0), pitchMax(0) {}
};
struct Instrument instr[15];
int8_t instrPointer = 0;
int8_t settingPointer = 0;

void saveInstrInSong();

void changeValues(int8_t i){
    if (settingPointer == 9) patch[instrPointer].attack += i;
    else if (settingPointer == 10) patch[instrPointer].decay += i;
    else if (settingPointer == 11) patch[instrPointer].sustain += i;
    else if (settingPointer == 12) patch[instrPointer].release += i;
    else if (settingPointer == 13) patch[instrPointer].bendrate = tracker.minMax(patch[instrPointer].bendrate+i, -100, 100);
    else if (settingPointer == 14) patch[instrPointer].maxbend = tracker.minMax(patch[instrPointer].maxbend+i, -90, 190);
    else if (settingPointer == 15) patch[instrPointer].vol = tracker.minMax(patch[instrPointer].vol+i, 0, 300);
}

bool checkButtons(){
    bool changed=false;
    if (pok.buttons.released(BTN_C)) {
            stopexit=false; // quick hack to stop jumping back immediately
    }
    if(pok.buttons.repeat(BTN_DOWN, buttonRepeatFrame))
        {settingPointer = tracker.minMax(settingPointer+1, 0, 19); changed=true;}
    else if(pok.buttons.repeat(BTN_UP, buttonRepeatFrame))
        {settingPointer = tracker.minMax(settingPointer-1, 0, 19); changed=true;}
    else if(pok.buttons.aBtn() && pok.buttons.repeat(BTN_LEFT, buttonRepeatFrame))
        {changeValues(-10);changed=true;}
    else if(pok.buttons.aBtn() && pok.buttons.repeat(BTN_RIGHT, buttonRepeatFrame))
        {changeValues(10);changed=true;}
    else if(pok.buttons.bBtn() && pok.buttons.repeat(BTN_LEFT, buttonRepeatFrame))
        {changeValues(-100);changed=true;}
    else if(pok.buttons.bBtn() && pok.buttons.repeat(BTN_RIGHT, buttonRepeatFrame))
        {changeValues(100);changed=true;}
    else if(pok.buttons.repeat(BTN_LEFT, buttonRepeatFrame))
    {
        if (settingPointer == 0) instrPointer = tracker.minMax(instrPointer-1, 0, 14);
        else if (settingPointer == 2) patch[instrPointer].wave = tracker.minMax(patch[instrPointer].wave-1, 0, 5);
		else if (settingPointer == 3) patch[instrPointer].arpmode = tracker.minMax(patch[instrPointer].arpmode-1, 0, 15);
        else if (settingPointer == 4) patch[instrPointer].loop = tracker.minMax(patch[instrPointer].loop-1, 0, 1);
        else if (settingPointer == 5) patch[instrPointer].echo = tracker.minMax(patch[instrPointer].echo-1, 0, 1);
        else if (settingPointer == 6) patch[instrPointer].overdrive = tracker.minMax(patch[instrPointer].overdrive-1, 0, 1);
        else if (settingPointer == 7) patch[instrPointer].kick = tracker.minMax(patch[instrPointer].kick-1, 0, 1);
        else if (settingPointer == 8) patch[instrPointer].adsr = tracker.minMax(patch[instrPointer].adsr-1, 0, 1);
        changeValues(-1);
        changed=true;
    }
    else if(pok.buttons.repeat(BTN_RIGHT, buttonRepeatFrame))
    {
        if (settingPointer == 0) instrPointer = tracker.minMax(instrPointer+1, 0, 14);
        else if (settingPointer == 2) patch[instrPointer].wave = tracker.minMax(patch[instrPointer].wave+1, 0, 5);
		else if (settingPointer == 3) patch[instrPointer].arpmode = tracker.minMax(patch[instrPointer].arpmode+1, 0, 15);
        else if (settingPointer == 4) patch[instrPointer].loop = tracker.minMax(patch[instrPointer].loop+1, 0, 1);
        else if (settingPointer == 5) patch[instrPointer].echo = tracker.minMax(patch[instrPointer].echo+1, 0, 1);
        else if (settingPointer == 6) patch[instrPointer].overdrive = tracker.minMax(patch[instrPointer].overdrive+1, 0, 1);
        else if (settingPointer == 7) patch[instrPointer].kick = tracker.minMax(patch[instrPointer].kick+1, 0, 1);
        else if (settingPointer == 8) patch[instrPointer].adsr = tracker.minMax(patch[instrPointer].adsr+1, 0, 1);
        changeValues(1);
        changed=true;
    }
    else if(pok.buttons.pressed(BTN_A))
    {
        tracker._currpatch=instrPointer;
        if(settingPointer == 1)
            {pok.keyboard(tracker.patchnames[instrPointer], 10);changed=true;}
        //else if(settingPointer == 16)
            //save instrument
        //else if(settingPointer == 17)
            //load instrument
        else if(settingPointer == 18){
            tracker.saveSong();
            saveInstrInSong();
            changed=true;
            }
        //else if(settingPointer == 19)
            //load song
        else tracker.playNote(); // try the note
    }
    else if(pok.buttons.pressed(BTN_C) && stopexit==false)
    {
        screen = 0;
        settingPointer = 0;
        changed=true;
        stopexit=true; // a low an mean, dirty, unprofessional hack
    }
    return changed;
}

void printSettings(){
    pok.display.setColor(1);
    pok.display.setCursor(2, 0);
    pok.display.print("Patch: ");
    pok.display.print(instrPointer, 10);
    pok.display.setCursor(2, fontH + 1);
    pok.display.print("Name: ");
    pok.display.print(tracker.patchnames[instrPointer]);
    pok.display.setCursor(2, (2 * fontH) + 2);
    pok.display.print("Wave: ");
    pok.display.print(waveNames[patch[instrPointer].wave]);
    pok.display.setCursor(2, (3 * fontH) + 3);
    pok.display.print("Arp Mode: ");
    pok.display.print(arpModes[patch[instrPointer].arpmode]);
    pok.display.setCursor(2, (4 * fontH) + 4);
    pok.display.print("Loop: ");
    pok.display.print((int)patch[instrPointer].loop);
    pok.display.setCursor(2, (5 * fontH) + 5);
    pok.display.print("Echo: ");
    pok.display.print((int)patch[instrPointer].echo);
    pok.display.setCursor(2, (6 * fontH) + 6);
    pok.display.print("Dist: ");
    pok.display.print((int)patch[instrPointer].overdrive);
    pok.display.setCursor(2, (7 * fontH) + 7);
    pok.display.print("Norm: ");
    pok.display.print((int)patch[instrPointer].kick);
    pok.display.setCursor(2, (8 * fontH) + 8);
    pok.display.print("ADSR: ");
    pok.display.print((int)patch[instrPointer].adsr);
    pok.display.setCursor(2, (9 * fontH) + 9);
    pok.display.print("Attack: ");
    pok.display.print(patch[instrPointer].attack, 10);
    pok.display.setCursor(2, (10 * fontH) + 10);
    pok.display.print("Decay: ");
    pok.display.print(patch[instrPointer].decay, 10);
    pok.display.setCursor(2, (11 * fontH) + 11);
    pok.display.print("Sustain: ");
    pok.display.print(patch[instrPointer].sustain, 10);
    pok.display.setCursor(2, (12 * fontH) + 12);
    pok.display.print("Release: ");
    pok.display.print(patch[instrPointer].release, 10);
    pok.display.setCursor(2, (13 * fontH) + 13);
    pok.display.print("Pitch Bend Rate: ");
    pok.display.print(patch[instrPointer].bendrate, 10);
    pok.display.setCursor(2, (14 * fontH) + 14);
    pok.display.print("Pitch Bend Max: ");
    pok.display.print(patch[instrPointer].maxbend, 10);
    pok.display.setCursor(2, (15 * fontH) + 15);
    pok.display.print("Volume: ");
    pok.display.print(patch[instrPointer].vol, 10);
    pok.display.setCursor(2, (16 * fontH) + 16);
    pok.display.print("Save Instrument");
    pok.display.setCursor(2, (17 * fontH) + 17);
    pok.display.print("Load Instrument");
    pok.display.setCursor(2, (18 * fontH) + 18);
    pok.display.print("Save Song");
    pok.display.setCursor(2, (19 * fontH) + 19);
    pok.display.print("Load Song");
}

void drawPointer(){
    pok.display.drawBitmap(130, (settingPointer * fontH) + settingPointer, pointBitmap);
}

void saveInstrInSong(){
    for (uint8_t j = 0; j < tracker.getLP() + 1; j++){
        if (tracker.patchnames[0] == 0) tracker.filePutInt(j);
        else tracker.filePrint(tracker.patchnames[j], sizeof(tracker.patchnames[j]));
        tracker.NL();
//        tracker.filePrint(waveChar, sizeof(waveChar));
        tracker.filePutInt(patch[j].wave);
        tracker.NL();
//        tracker.filePrint(volChar, sizeof(volChar));
        tracker.filePutInt(patch[j].vol);
        tracker.NL();
//        tracker.filePrint(pitchRateChar, sizeof(pitchRateChar));
        tracker.filePutInt(patch[j].bendrate);
        tracker.NL();
//        tracker.filePrint(pitchMaxChar, sizeof(pitchMaxChar));
        tracker.filePutInt(patch[j].maxbend);
        tracker.NL();
 //       tracker.filePrint(vibChar, sizeof(vibChar));
        tracker.filePutInt(0);
        tracker.NL();
   //     tracker.filePrint(arpChar, sizeof(arpChar));
        tracker.filePutInt(patch[j].arpmode);
        tracker.NL();
     //   tracker.filePrint(ADSRChar, sizeof(ADSRChar));
        tracker.filePutInt(patch[j].adsr);
        tracker.NL();
    //    tracker.filePrint(attackChar, sizeof(attackChar));
        tracker.filePutInt(patch[j].attack);
        tracker.NL();
  //      tracker.filePrint(decayChar, sizeof(decayChar));
        tracker.filePutInt(patch[j].decay);
        tracker.NL();
  //      tracker.filePrint(sustainChar, sizeof(sustainChar));
        tracker.filePutInt(patch[j].sustain);
        tracker.NL();
   //     tracker.filePrint(releaseChar, sizeof(releaseChar));
        tracker.filePutInt(patch[j].release);
        tracker.NL();
   //     tracker.filePrint(loopChar, sizeof(loopChar));
        tracker.filePutInt(patch[j].loop);
        tracker.NL();
   //     tracker.filePrint(echoChar, sizeof(echoChar));
        tracker.filePutInt(patch[j].echo);
        tracker.NL();
   //     tracker.filePrint(overdriveChar, sizeof(overdriveChar));
        tracker.filePutInt(patch[j].overdrive);
        tracker.NL();
    //    tracker.filePrint(drumChar, sizeof(drumChar));
        tracker.filePutInt(patch[j].kick);
        tracker.NL();
    }
    fileClose();
}
