//
// Tiny Chip-8 : A Chip-8 emulator for the Pokitto (http://www.pokitto.com)
//
// Copyright (C) 2017 Jerome Perrot (Initgraph)
//
// Version : Experimental
//
// Licence :
//
//   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// For any remarks or questions you can contact me on Twitter (@InitgraphDotCom) or by email (contact@initgraph.com).
// You can also visit my website : http://www.initgraph.com
//
// Enjoy !
//

#include "CHIP8Player.h"
#include "PokittoDisk.h" //jonne

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
CHIP8Player::CHIP8Player() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Init
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void CHIP8Player::init(Pokitto::Core *pokittoCore) {

  pokitto = pokittoCore;
  uint16_t i;
  char opcodeSource[32];

  // Init CHIP-8 default font
  mem[0]=0xF0;  mem[1]=0x90;  mem[2]=0x90;  mem[3]=0x90;  mem[4]=0xF0;  // O
  mem[5]=0x20;  mem[6]=0x60;  mem[7]=0x20;  mem[8]=0x20;  mem[9]=0x70;  // 1
  mem[10]=0xF0; mem[11]=0x10; mem[12]=0xF0; mem[13]=0x80; mem[14]=0xF0; // 2
  mem[15]=0xF0; mem[16]=0x10; mem[17]=0xF0; mem[18]=0x10; mem[19]=0xF0; // 3
  mem[20]=0x90; mem[21]=0x90; mem[22]=0xF0; mem[23]=0x10; mem[24]=0x10; // 4
  mem[25]=0xF0; mem[26]=0x80; mem[27]=0xF0; mem[28]=0x10; mem[29]=0xF0; // 5
  mem[30]=0xF0; mem[31]=0x80; mem[32]=0xF0; mem[33]=0x90; mem[34]=0xF0; // 6
  mem[35]=0xF0; mem[36]=0x10; mem[37]=0x20; mem[38]=0x40; mem[39]=0x40; // 7
  mem[40]=0xF0; mem[41]=0x90; mem[42]=0xF0; mem[43]=0x90; mem[44]=0xF0; // 8
  mem[45]=0xF0; mem[46]=0x90; mem[47]=0xF0; mem[48]=0x10; mem[49]=0xF0; // 9
  mem[50]=0xF0; mem[51]=0x90; mem[52]=0xF0; mem[53]=0x90; mem[54]=0x90; // A
  mem[55]=0xE0; mem[56]=0x90; mem[57]=0xE0; mem[58]=0x90; mem[59]=0xE0; // B
  mem[60]=0xF0; mem[61]=0x80; mem[62]=0x80; mem[63]=0x80; mem[64]=0xF0; // C
  mem[65]=0xE0; mem[66]=0x90; mem[67]=0x90; mem[68]=0x90; mem[69]=0xE0; // D
  mem[70]=0xF0; mem[71]=0x80; mem[72]=0xF0; mem[73]=0x80; mem[74]=0xF0; // E
  mem[75]=0xF0; mem[76]=0x80; mem[77]=0xF0; mem[78]=0x80; mem[79]=0x80; // F

  // Init opcodes list
  opcodesIds[0]=0x0FFF;  opcodesMasks[0]= 0x0000;  // 0NNN
  opcodesIds[1]=0x00E0;  opcodesMasks[1]= 0xFFFF;  // 00E0
  opcodesIds[2]=0x00EE;  opcodesMasks[2]= 0xFFFF;  // 00EE
  opcodesIds[3]=0x1000;  opcodesMasks[3]= 0xF000;  // 1NNN
  opcodesIds[4]=0x2000;  opcodesMasks[4]= 0xF000;  // 2NNN
  opcodesIds[5]=0x3000;  opcodesMasks[5]= 0xF000;  // 3XNN
  opcodesIds[6]=0x4000;  opcodesMasks[6]= 0xF000;  // 4XNN
  opcodesIds[7]=0x5000;  opcodesMasks[7]= 0xF00F;  // 5XY0
  opcodesIds[8]=0x6000;  opcodesMasks[8]= 0xF000;  // 6XNN
  opcodesIds[9]=0x7000;  opcodesMasks[9]= 0xF000;  // 7XNN
  opcodesIds[10]=0x8000; opcodesMasks[10]= 0xF00F; // 8XY0
  opcodesIds[11]=0x8001; opcodesMasks[11]= 0xF00F; // 8XY1
  opcodesIds[12]=0x8002; opcodesMasks[12]= 0xF00F; // 8XY2
  opcodesIds[13]=0x8003; opcodesMasks[13]= 0xF00F; // BXY3
  opcodesIds[14]=0x8004; opcodesMasks[14]= 0xF00F; // 8XY4
  opcodesIds[15]=0x8005; opcodesMasks[15]= 0xF00F; // 8XY5
  opcodesIds[16]=0x8006; opcodesMasks[16]= 0xF00F; // 8XY6
  opcodesIds[17]=0x8007; opcodesMasks[17]= 0xF00F; // 8XY7
  opcodesIds[18]=0x800E; opcodesMasks[18]= 0xF00F; // 8XYE
  opcodesIds[19]=0x9000; opcodesMasks[19]= 0xF00F; // 9XY0
  opcodesIds[20]=0xA000; opcodesMasks[20]= 0xF000; // ANNN
  opcodesIds[21]=0xB000; opcodesMasks[21]= 0xF000; // BNNN
  opcodesIds[22]=0xC000; opcodesMasks[22]= 0xF000; // CXNN
  opcodesIds[23]=0xD000; opcodesMasks[23]= 0xF000; // DXYN
  opcodesIds[24]=0xE09E; opcodesMasks[24]= 0xF0FF; // EX9E
  opcodesIds[25]=0xE0A1; opcodesMasks[25]= 0xF0FF; // EXA1
  opcodesIds[26]=0xF007; opcodesMasks[26]= 0xF0FF; // FX07
  opcodesIds[27]=0xF00A; opcodesMasks[27]= 0xF0FF; // FX0A
  opcodesIds[28]=0xF015; opcodesMasks[28]= 0xF0FF; // FX15
  opcodesIds[29]=0xF018; opcodesMasks[29]= 0xF0FF; // FX18
  opcodesIds[30]=0xF01E; opcodesMasks[30]= 0xF0FF; // FX1E
  opcodesIds[31]=0xF029; opcodesMasks[31]= 0xF0FF; // FX29
  opcodesIds[32]=0xF033; opcodesMasks[32]= 0xF0FF; // FX33
  opcodesIds[33]=0xF055; opcodesMasks[33]= 0xF0FF; // FX55
  opcodesIds[34]=0xF065; opcodesMasks[34]= 0xF0FF; // FX65

  // Init Editor
  initEditor();

  // Init player
  initPlayer();

  // Init screen
  screenWidth = CHIP8_SCREEN_WIDTH;
  screenHeight = CHIP8_SCREEN_HEIGHT;

  opcodeIndex = 0;
  speed = 16;
  screenIsInEditMode = false;
  state = STATE_MAIN_MENU;

  // Init the main menu
  mainMenu.init(pokitto, MAIN_MENU_X, MAIN_MENU_Y, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT, 8, 1);
  mainMenu.addItem("PLAY !");
  mainMenu.addItem("EDITOR");
  mainMenu.addItem("SETTINGS");
  mainMenu.addItem("CREDITS");
  mainMenu.addItem("NEW");
  mainMenu.addItem("LOAD");
  mainMenu.addItem("SAVE");
  mainMenu.addItem("LOAD DEMO");
  mainMenu.selectedItem = 1;

  // Init the editor view
  editorView.init(pokitto, EDITOR_X, EDITOR_Y, EDITOR_WIDTH, EDITOR_HEIGHT, EDITOR_HEIGHT, LB_HIDE_BORDER);

  // Init the context menu
  contextMenu.init(pokitto, CONTEXT_MENU_X, CONTEXT_MENU_Y, CONTEXT_MENU_WIDTH, CONTEXT_MENU_HEIGHT, 6, 0);
  contextMenu.addItem("ADD...");
  contextMenu.addItem("DELETE");
  contextMenu.addItem("MOVE UP");
  contextMenu.addItem("MOVE DOWN");
  contextMenu.addItem("EDIT AS SPRITE");
  contextMenu.addItem("EDIT AS DATA");

  // Init the "ADD..." menu
  addMenu.init(pokitto, MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT, 17, 0);
  addMenu.addItem("VX = ...");
  addMenu.addItem("I = ...");
  addMenu.addItem("SPRITE VX VY N");
  addMenu.addItem("IF VX ...");
  addMenu.addItem("GOTO N");
  addMenu.addItem("CALL N");
  addMenu.addItem("IF KEY VX");
  addMenu.addItem("IF NOT KEY VX");
  addMenu.addItem("RETURN");
  addMenu.addItem("TIMER = VX");
  addMenu.addItem("DATA");
  addMenu.addItem("SAVEDEC VX");
  addMenu.addItem("SOUNDTIMER = VX");
  addMenu.addItem("SAVE V0 TO VX");
  addMenu.addItem("LOAD V0 TO VX");
  addMenu.addItem("GOTO N + V0");
  addMenu.addItem("CLS");

  // Init the "VX = ..." menu
  vxEqualsMenu.init(pokitto, MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT, 14, 0);
  vxEqualsMenu.addItem("VX = N");
  vxEqualsMenu.addItem("VX = VY");
  vxEqualsMenu.addItem("VX = VX + N");
  vxEqualsMenu.addItem("VX = VX + VY");
  vxEqualsMenu.addItem("VX = VX - VY");
  vxEqualsMenu.addItem("VX = VY - VX");
  vxEqualsMenu.addItem("VX = VX AND VY");
  vxEqualsMenu.addItem("VX = VX OR VY");
  vxEqualsMenu.addItem("VX = VX XOR VY");
  vxEqualsMenu.addItem("VX = VX << 1");
  vxEqualsMenu.addItem("VX = VX >> 1");
  vxEqualsMenu.addItem("VX = RANDOM 0 TO N");
  vxEqualsMenu.addItem("VX = TIMER");
  vxEqualsMenu.addItem("VX = READ KEY");

  // Init the "VX = N" menu
  vxEqualsNMenu.init(pokitto, 4);
  vxEqualsNMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15);  // VX
  vxEqualsNMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 255); // N
  vxEqualsNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxEqualsNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VY" menu
  vxEqualsVyMenu.init(pokitto, 4);
  vxEqualsVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxEqualsVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxEqualsVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxEqualsVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX + N" menu
  vxPlusNMenu.init(pokitto, 4);
  vxPlusNMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxPlusNMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 255); // N
  vxPlusNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxPlusNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX + VY" menu
  vxPlusVyMenu.init(pokitto, 4);
  vxPlusVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxPlusVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxPlusVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxPlusVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX - VY" menu
  vxMinusVyMenu.init(pokitto, 4);
  vxMinusVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxMinusVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxMinusVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxMinusVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VY - VX" menu
  vyMinusVxMenu.init(pokitto, 4);
  vyMinusVxMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vyMinusVxMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vyMinusVxMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vyMinusVxMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX AND VY" menu
  vxAndVyMenu.init(pokitto, 4);
  vxAndVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxAndVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxAndVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxAndVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX OR VY" menu
  vxOrVyMenu.init(pokitto, 4);
  vxOrVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxOrVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxOrVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxOrVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX XOR VY" menu
  vxXorVyMenu.init(pokitto, 4);
  vxXorVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxXorVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  vxXorVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxXorVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX << 1" menu
  vxLeftShiftMenu.init(pokitto, 3);
  vxLeftShiftMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  vxLeftShiftMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxLeftShiftMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = VX >> 1" menu
  vxRightShiftMenu.init(pokitto, 3);
  vxRightShiftMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  vxRightShiftMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxRightShiftMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = RANDOM 0 TO N" menu
  vxRandomMenu.init(pokitto, 4);
  vxRandomMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  vxRandomMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 255); // N
  vxRandomMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxRandomMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = TIMER" menu
  vxTimerMenu.init(pokitto, 3);
  vxTimerMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  vxTimerMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxTimerMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "VX = READ KEY" menu
  vxReadKeyMenu.init(pokitto, 3);
  vxReadKeyMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  vxReadKeyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  vxReadKeyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "I = ..." menu
  iEqualsMenu.init(pokitto, MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT, 3, 0);
  iEqualsMenu.addItem("I = N");
  iEqualsMenu.addItem("I = CHAR VX");
  iEqualsMenu.addItem("I = I + VX");

  // Init "I = N" Menu
  iEqualsNMenu.init(pokitto, 3);
  iEqualsNMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 4094); // N
  iEqualsNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  iEqualsNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "I = CHAR VX" Menu
  iEqualsCharVXMenu.init(pokitto, 3);
  iEqualsCharVXMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  iEqualsCharVXMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  iEqualsCharVXMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "I = I + VX" Menu
  iPlusVxMenu.init(pokitto, 3);
  iPlusVxMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  iPlusVxMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  iPlusVxMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "SPRITE VX VY N" Menu
  spriteMenu.init(pokitto, 5);
  spriteMenu.addItem(VALUE_X, PARAM_Y_1_3, 0, 0, 15); // VX
  spriteMenu.addItem(VALUE_X, PARAM_Y_2_3, 0, 0, 15); // VY
  spriteMenu.addItem(VALUE_X, PARAM_Y_3_3, 0, 0, 15); // N
  spriteMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  spriteMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "IF VX ..." menu
  ifVxMenu.init(pokitto, MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT, 4, 0);
  ifVxMenu.addItem("IF VX == N");
  ifVxMenu.addItem("IF VX == VY");
  ifVxMenu.addItem("IF VX != N");
  ifVxMenu.addItem("IF VX != VY");

  // Init the "IF VX == N" menu
  ifVxEqualsNMenu.init(pokitto, 4);
  ifVxEqualsNMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15);  // VX
  ifVxEqualsNMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 255); // N
  ifVxEqualsNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifVxEqualsNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "IF VX == VY" menu
  ifVxEqualsVyMenu.init(pokitto, 4);
  ifVxEqualsVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  ifVxEqualsVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  ifVxEqualsVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifVxEqualsVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

    // Init the "IF VX != N" menu
  ifVxNotEqualsNMenu.init(pokitto, 4);
  ifVxNotEqualsNMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15);  // VX
  ifVxNotEqualsNMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 255); // N
  ifVxNotEqualsNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifVxNotEqualsNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init the "IF VX != VY" menu
  ifVxNotEqualsVyMenu.init(pokitto, 4);
  ifVxNotEqualsVyMenu.addItem(VALUE_X, PARAM_Y_1_2, 0, 0, 15); // VX
  ifVxNotEqualsVyMenu.addItem(VALUE_X, PARAM_Y_2_2, 0, 0, 15); // VY
  ifVxNotEqualsVyMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifVxNotEqualsVyMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "GOTO N" Menu
  gotoNMenu.init(pokitto, 3);
  gotoNMenu.addItem(VALUE_X, PARAM_Y_1_1, 512, 512, 4094); // N
  gotoNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  gotoNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "CALL N" Menu
  callNMenu.init(pokitto, 3);
  callNMenu.addItem(VALUE_X, PARAM_Y_1_1, 512, 512, 4094); // N
  callNMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  callNMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "IF KEY VX" Menu
  ifKeyVxMenu.init(pokitto, 3);
  ifKeyVxMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  ifKeyVxMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifKeyVxMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "IF NOT KEY VX" Menu
  ifNotKeyVxMenu.init(pokitto, 3);
  ifNotKeyVxMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  ifNotKeyVxMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  ifNotKeyVxMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "TIMER = VX" Menu
  timerMenu.init(pokitto, 3);
  timerMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  timerMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  timerMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "DATA" Menu
  dataMenu.init(pokitto, 4);
  dataMenu.addItem(50, PARAM_Y_1_2, 0, 0, 255); // DATA 1
  dataMenu.addItem(50, PARAM_Y_2_2, 0, 0, 255); // DATA 2
  dataMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  dataMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "SAVEDEC VX" Menu
  saveDecVxMenu.init(pokitto, 3);
  saveDecVxMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  saveDecVxMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  saveDecVxMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "SOUNDTIMER = VX" Menu
  soundTimerMenu.init(pokitto, 3);
  soundTimerMenu.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  soundTimerMenu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  soundTimerMenu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "SAVE VO TO VX" Menu
  saveV0ToVx.init(pokitto, 3);
  saveV0ToVx.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  saveV0ToVx.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  saveV0ToVx.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "LOAD V0 TO VX" Menu
  loadV0ToVx.init(pokitto, 3);
  loadV0ToVx.addItem(VALUE_X, PARAM_Y_1_1, 0, 0, 15); // VX
  loadV0ToVx.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  loadV0ToVx.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // Init "GOTO N + V0" Menu
  gotoNPlusV0Menu.init(pokitto, 3);
  gotoNPlusV0Menu.addItem(VALUE_X, PARAM_Y_1_1, 512, 512, 4094); // N
  gotoNPlusV0Menu.addItem(CANCEL_BUTTON_X, CANCEL_BUTTON_Y, "CANCEL");
  gotoNPlusV0Menu.addItem(OK_BUTTON_X, OK_BUTTON_Y, "OK");

  // ---------- Démo
  /*mem[512] = 0x00; mem[513] = 0xE0; // 00E0: CLS
  mem[514] = 0x60; mem[515] = 0x00; // 6XNN: V0 = 0
  mem[516] = 0xF0; mem[517] = 0x29; // FX29: I = CHAR V0
  mem[518] = 0x61; mem[519] = 0x01; // 6XNN: V1 = 1
  mem[520] = 0x62; mem[521] = 0x01; // 6XNN: V2 = 1
  mem[522] = 0xD1; mem[523] = 0x25; // DXYN: SPRITE V1 V2 5
  mem[524] = 0x63; mem[525] = 0x1E; // 6XNN: V3 = 30
  mem[526] = 0xF3; mem[527] = 0x15; // FX15: TIMER = V3
  mem[528] = 0xF3; mem[529] = 0x07; // FX07: V3 = TIMER
  mem[530] = 0x43; mem[531] = 0x00; // 4XNN: IF V3 = 0
  mem[532] = 0x12; mem[533] = 0x18; // 1NNN:   GOTO 536
  mem[534] = 0x12; mem[535] = 0x10; // 1NNN: GOTO 528
  mem[536] = 0x70; mem[537] = 0x01; // 7XNN: V0 = V0 + 1
  mem[538] = 0xF0; mem[539] = 0x29; // FX29: I = CHAR V0
  mem[540] = 0x00; mem[541] = 0xE0; // 00E0: CLS
  mem[542] = 0xD1; mem[543] = 0x25; // DXYN: SPRITE V1 V2 5
  mem[544] = 0x12; mem[545] = 0x0C; // 1NNN: GOTO 524
  romSize = 34;*/
}

// ----------------------------------------------------------------------------------------------------
// Init the TINY CHIP-8 editor
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::initEditor() {

  uint16_t i;
  uint8_t editorItemsCount;

  // Clear memory
  for (i=ROM_START; i<MEM_SIZE; i++) {

    mem[i] = 0;
  }
  romSize = 0;

  // Clear the editor view
  editorItemsCount = editorView.itemsCount;
  for (i=0; i<editorItemsCount; i++) {

    editorView.removeItem(0);
  }
  firstInstructionToShow = ROM_START;
}

// ----------------------------------------------------------------------------------------------------
// Init the TINY CHIP-8 player
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::initPlayer() {

  uint8_t i;

  PC = ROM_START;
  I = 0;

  // Init variables, stack and keys
  for (i=0; i<16; i++) {

    V[i] = 0;
    stack[i] = 0;
    keys[i] = 0;
  }
  stackLength = 0;
  keyPressed = false;

  // Init timers
  delayTimer = 0;
  soundTimer = 0;

  clearScreen();
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::update() {

  uint16_t i;
  uint16_t opcode = 0;
  uint8_t opcodeIndex = 0;
  uint8_t opcodePart3 = 0;
  uint8_t opcodePart2 = 0;
  uint8_t opcodePart1 = 0;
  uint16_t memAddress = 0;
  uint16_t temp;

  switch (state) {

    //----- Game -------------------------------------------------------------------------------------------
    case STATE_GAME:

      // If the "C" button is pressed
      if (pokitto->buttons.released(BTN_C)) {

        // Go back to the main menu
        state = STATE_MAIN_MENU;
      }

      readKeys();

      for (i=0; i<speed; i++) {

        readOpcode();
      }

      drawScreen();

      if (delayTimer > 0) delayTimer--;
      if (soundTimer > 0) soundTimer--;

      pokitto->display.print(0, 40, "Opcode: ");
      pokitto->display.print(45, 40, PC);

      break;

    //----- Editor -----------------------------------------------------------------------------------------
    case STATE_EDITOR:

      // If the "A" button is pressed
      if (pokitto->buttons.pressed(BTN_A) && editorView.itemsCount) {

        memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

        opcode = (mem[memAddress] << 8) + mem[memAddress + 1];

        opcodeIndex = getOpcodeIndex(opcode);

        opcodePart3 = (opcode & 0x0F00) >> 8; // Bits 0, 1, 2 et 3
        opcodePart2 = (opcode & 0x00F0) >> 4; // Bits 4, 5, 6 et 7
        opcodePart1 = opcode & 0x000F;        // Bits 8, 9, 10 et 11

        switch (opcodeIndex) {

          case 0: // 0NNN: Calls RCA 1802 (not implemented here, replaced by bytes data).

            // Show the "DATA" screen in edit mode
            dataMenu.setValue(0, opcode >> 8);
            dataMenu.setValue(1, opcode & 255);
            dataMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_DATA_MENU;
            break;

          case 1: // 00E0: Clears the screen.

            // Nothing to edit so nothing to do
            break;

          case 2: // 00EE:

            // Nothing to edit so nothing to do
            break;

          case 3: // 1NNN: effectue un saut à l'adresse NNN.

            // Show the "GOTO N" screen in edit mode
            gotoNMenu.setValue(0, (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1);
            gotoNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_GOTO_N_MENU;
            break;

          case 4: // 2NNN: appelle le sous-programme en NNN, mais on revient ensuite.

            // Show the "CALL N" screen in edit mode
            callNMenu.setValue(0, (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1);
            callNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_CALL_N_MENU;
            break;

          case 5: // 3XNN: saute l'instruction suivante si VX est égal à NN (IF VX != N).

            // Show the "IF VX != N" screen in edit mode
            ifVxNotEqualsNMenu.setValue(0, opcodePart3);
            ifVxNotEqualsNMenu.setValue(1, (opcodePart2 << 4) + opcodePart1);
            ifVxNotEqualsNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_VX_NOT_EQUALS_N_MENU;
            break;

          case 6: // 4XNN: saute l'instruction suivante si VX et NN ne sont pas égaux.

            // Show the "IF VX == N" screen in edit mode
            ifVxEqualsNMenu.setValue(0, opcodePart3);
            ifVxEqualsNMenu.setValue(1, (opcodePart2 << 4) + opcodePart1);
            ifVxEqualsNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_VX_EQUALS_N_MENU;
            break;

          case 7: // 5XY0: saute l'instruction suivante si VX et VY sont égaux.

            // Show the "IF VX != VY" screen in edit mode
            ifVxNotEqualsVyMenu.setValue(0, opcodePart3);
            ifVxNotEqualsVyMenu.setValue(1, opcodePart2);
            ifVxNotEqualsVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_VX_NOT_EQUALS_VY_MENU;
            break;

          case 8: // 6XNN: définit VX à NN.

            // Show the "VX = N" screen in edit mode
            vxEqualsNMenu.setValue(0, opcodePart3);
            vxEqualsNMenu.setValue(1, ((opcodePart2 << 4) + opcodePart1));
            vxEqualsNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_EQUALS_N_MENU;
            break;

          case 9: // 7XNN: ajoute NN à VX.

            // Show the "VX = VX + N" screen in edit mode
            vxPlusNMenu.setValue(0, opcodePart3);
            vxPlusNMenu.setValue(1, ((opcodePart2 << 4) + opcodePart1));
            vxPlusNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_PLUS_N_MENU;
            break;

          case 10: // 8XY0: définit VX à la valeur de VY.

            // Show the "VX = VY" screen in edit mode
            vxEqualsVyMenu.setValue(0, opcodePart3);
            vxEqualsVyMenu.setValue(1, opcodePart2);
            vxEqualsVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_EQUALS_VY_MENU;
            break;


          case 11: // 8XY1: définit VX à VX OR VY.

            // Show the "VX = VX OR VY" screen in edit mode
            vxOrVyMenu.setValue(0, opcodePart3);
            vxOrVyMenu.setValue(1, opcodePart2);
            vxOrVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_OR_VY_MENU;
            break;

          case 12: // 8XY2: définit VX à VX AND VY.

            // Show the "VX = VX AND VY" screen in edit mode
            vxAndVyMenu.setValue(0, opcodePart3);
            vxAndVyMenu.setValue(1, opcodePart2);
            vxAndVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_AND_VY_MENU;
            break;

          case 13: // 8XY3: définit VX à VX XOR VY.

            // Show the "VX = VX XOR VY" screen in edit mode
            vxXorVyMenu.setValue(0, opcodePart3);
            vxXorVyMenu.setValue(1, opcodePart2);
            vxXorVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_XOR_VY_MENU;
            break;

          case 14: // 8XY4: ajoute VY à VX. VF est mis à 1 quand il y a un dépassement de mémoire (carry), et à 0 quand il n'y en pas.

            // Show the "VX = VX + VY" screen in edit mode
            vxPlusVyMenu.setValue(0, opcodePart3);
            vxPlusVyMenu.setValue(1, opcodePart2);
            vxPlusVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_PLUS_VY_MENU;
            break;

          case 15: // 8XY5: VY est soustraite de VX. VF est mis à 0 quand il y a un emprunt, et à 1 quand il n'y a en pas.

            // Show the "VX = VX - VY" screen in edit mode
            vxMinusVyMenu.setValue(0, opcodePart3);
            vxMinusVyMenu.setValue(1, opcodePart2);
            vxMinusVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_MINUS_VY_MENU;
            break;

          case 16: // 8XY6: décale VX de 1 bit vers la droite. VF est fixé à la valeur du bit de poids faible de VX avant le décalage.

            // Show the "VX = VX >> 1" screen in edit mode
            vxRightShiftMenu.setValue(0, opcodePart3);
            vxRightShiftMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_RIGHTSHIFT_MENU;
            break;

          case 17: // 8XY7: VX = VY - VX. VF est mis à 0 quand il y a un emprunt et à 1 quand il n'y en a pas.

            // Show the "VX = VY - VX" screen in edit mode
            vyMinusVxMenu.setValue(0, opcodePart3);
            vyMinusVxMenu.setValue(1, opcodePart2);
            vyMinusVxMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VY_MINUS_VX_MENU;
            break;

          case 18: // 8XYE: décale VX de 1 bit vers la gauche. VF est fixé à la valeur du bit de poids fort de VX avant le décalage.

            // Show the "VX = VX << 1" screen in edit mode
            vxLeftShiftMenu.setValue(0, opcodePart3);
            vxLeftShiftMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_LEFTSHIFT_MENU;
            break;

          case 19: // 9XY0: saute l'instruction suivante si VX et VY ne sont pas égaux.

            // Show the "VX = VX - VY" screen in edit mode
            ifVxEqualsVyMenu.setValue(0, opcodePart3);
            ifVxEqualsVyMenu.setValue(1, opcodePart2);
            ifVxEqualsVyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_VX_EQUALS_VY_MENU;
            break;

          case 20: // ANNN: affecte NNN à I.

            // Show the "I = N" screen in edit mode
            iEqualsNMenu.setValue(0, ((opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1));
            iEqualsNMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_I_EQUALS_N_MENU;
            break;

          case 21: // BNNN: passe à l'adresse NNN + V0.

            // Show the "GOTO N + V0" screen in edit mode
            gotoNPlusV0Menu.setValue(0, ((opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1));
            gotoNPlusV0Menu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_GOTO_N_PLUS_V0_MENU;
            break;

          case 22: // CXNN: Sets VX to the result of a bitwise and operation on a random byte value (0 to 255) and NN

            // Show the "VX = RANDOM 0 TO N" screen in edit mode
            vxRandomMenu.setValue(0, opcodePart3);
            vxRandomMenu.setValue(1, (opcodePart2 << 4) + opcodePart1);
            vxRandomMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_RANDOM_MENU;
            break;

          case 23: // DXYN:  dessine un sprite de hauteur N aux coordonnées (VX, VY).

            // Show the "SPRITE VX VY N" screen in edit mode
            spriteMenu.setValue(0, opcodePart3);
            spriteMenu.setValue(1, opcodePart2);
            spriteMenu.setValue(2, opcodePart1);
            spriteMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_SPRITE_MENU;
            break;

          case 24: // EX9E: saute l'instruction suivante si la clé stockée dans VX est pressée.

            // Show the "IF NOT KEY VX" screen in edit mode
            ifNotKeyVxMenu.setValue(0, opcodePart3);
            ifNotKeyVxMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_NOT_KEY_VX_MENU;
            break;


          case 25: // EXA1: saute l'instruction suivante si la clé stockée dans VX n'est pas pressée.

            //sprintf(opcodeSource, "%04u IF KEY V%d\n", memAddress, opcodePart3);
            // Show the "IF KEY VX" screen in edit mode
            ifKeyVxMenu.setValue(0, opcodePart3);
            ifKeyVxMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_IF_KEY_VX_MENU;
            break;

          case 26: // FX07: affecte la valeur de la temporisation à VX

            // Show the "VX = TIMER" screen in edit mode
            vxTimerMenu.setValue(0, opcodePart3);
            vxTimerMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_TIMER_MENU;
            break;

          case 27: // FX0A: attend l'appui sur une touche et stocke ensuite sa valeur dans VX.

            // Show the "VX = READ KEY" screen in edit mode
            vxReadKeyMenu.setValue(0, opcodePart3);
            vxReadKeyMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_VX_READKEY_MENU;
            break;

          case 28: // FX15: définit la temporisation à VX.

            // Show the "TIMER = VX" screen in edit mode
            timerMenu.setValue(0, opcodePart3);
            timerMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_TIMER_MENU;
            break;

          case 29: // FX18: définit la minuterie sonore à VX.

            // Show the "SOUNDTIMER = VX" screen in edit mode
            soundTimerMenu.setValue(0, opcodePart3);
            soundTimerMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_SOUNDTIMER_MENU;
            break;

          case 30: // FX1E: ajoute VX à I. VF est mis à 1 quand il y a overflow (I+VX>0xFFF), et à 0 si tel n'est pas le cas.

            // Show the "I = I + VX" screen in edit mode
            iPlusVxMenu.setValue(0, opcodePart3);
            iPlusVxMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_I_PLUS_VX_MENU;
            break;

          case 31: // FX29: définit I à l'emplacement du caractère stocké dans VX.

            // Show the "I = CHAR VX" screen in edit mode
            iEqualsCharVXMenu.setValue(0, opcodePart3);
            iEqualsCharVXMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_I_EQUALS_CHAR_VX_MENU;
            break;

          case 32: // FX33: stocke dans la mémoire le code décimal représentant VX (dans I, I+1, I+2).

            // Show the "SAVEDEC VX" screen in edit mode
            saveDecVxMenu.setValue(0, opcodePart3);
            saveDecVxMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_SAVEDEC_VX_MENU;
            break;

          case 33: // FX55: stocke V0 à VX en mémoire à partir de l'adresse I.

            // Show the "SAVE V0 TO VX" screen in edit mode
            saveV0ToVx.setValue(0, opcodePart3);
            saveV0ToVx.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_SAVE_V0_TO_VX_MENU;
            break;

          case 34: // FX65: remplit V0 à VX avec les valeurs de la mémoire à partir de l'adresse I.

            // Show the "LOAD V0 TO VX" screen in edit mode
            loadV0ToVx.setValue(0, opcodePart3);
            loadV0ToVx.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_LOAD_V0_TO_VX_MENU;
            break;

          default:

            // Show the "DATA" screen in edit mode
            dataMenu.setValue(0, opcode >> 8);
            dataMenu.setValue(1, opcode & 255);
            dataMenu.selectedItem = 0;
            screenIsInEditMode = true;
            state = STATE_DATA_MENU;
        }
      }

      // If the "B" button is pressed
      else if (pokitto->buttons.released(BTN_B)) { //jonne

        // If the editor is not empty
        if (editorView.itemsCount) {

          // Show the context menu
          contextMenu.selectedItem = 0;
          state = STATE_CONTEXT_MENU;
        }

        // If the editor is empty
        else {

          // Show the "ADD..." menu in order to add the first instruction
          state = STATE_ADD_MENU;
        }
      }

      // If the "C" button is pressed
      else if (pokitto->buttons.released(BTN_C)) { //jonne

        // Show the main menu
        state = STATE_MAIN_MENU;
      }

      else if (pokitto->buttons.repeat(BTN_UP, 4)) {

        // If the selected instruction is the first of the editor view and not the first of the source code
        if (editorView.selectedItem == FIRST_EDITOR_LINE && firstInstructionToShow > ROM_START) {

          // Update editor view with the previous part of the source code
          firstInstructionToShow -= 2;
          updateEditorView();
        }
      }

      else if (pokitto->buttons.repeat(BTN_DOWN, 4)) {

        // If the selected instruction is the last of the editor view and not the last of the source code
        if (editorView.selectedItem == LAST_EDITOR_LINE && ((firstInstructionToShow + 16) < (ROM_START + romSize - 2))) {

          // Update editor view with the next part of the source code
          firstInstructionToShow += 2;
          updateEditorView();
        }
      }

      editorView.update();
      editorView.show();

      // If the rom is empty
      if (editorView.itemsCount == 0) {

        // Show the editor welcome screen
        pokitto->display.color = 1;
        pokitto->display.print(15, 10, "Welcome to the");
        pokitto->display.print(22, 20, "TINY CHIP-8");
        pokitto->display.print(37, 30, "editor");
        pokitto->display.print(7, 60, "Press B to add an");
        pokitto->display.print(12, 70, "instruction...");
      }

      break;

    //----- Main menu -----------------------------------------------------------------------------------
    case STATE_MAIN_MENU:

      // Update ListBox component
      mainMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "RUN !" menu is selected
        if (mainMenu.selectedItem == 0) {

          // Run the game
          initPlayer();
          state = STATE_GAME;
        }

        // If the "EDITOR" menu is selected
        if (mainMenu.selectedItem == 1) {

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "NEW" menu is selected
        if (mainMenu.selectedItem == 4) {

          // Init the editor
          initEditor();

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "LOAD" menu is selected
        if (mainMenu.selectedItem == 5) {

          // Load user rom into memory
          loadUserRom();

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "SAVE" menu is selected
        if (mainMenu.selectedItem == 6) {

          saveUserRom();
        }

        // If the "LOAD DEMO" menu is selected
        if (mainMenu.selectedItem == 7) {

          // Load BRIX rom into memory
          loadRom("chip8rom/BREAKOUT.ch8");

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // Show the main menu screen
      pokitto->display.drawRect(MAIN_MENU_FRAME_X, MAIN_MENU_FRAME_Y, MAIN_MENU_FRAME_WIDTH, MAIN_MENU_FRAME_HEIGHT);
      pokitto->display.print(MAIN_MENU_TITLE_X, MAIN_MENU_TITLE_Y, "TINY CHIP-8");
      mainMenu.show();

      break;

    //----- Context menu -----------------------------------------------------------------------------------
    case STATE_CONTEXT_MENU:

      // Update ListBox component
      contextMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "ADD..." menu is selected
        if (contextMenu.selectedItem == 0) {

          // Show the "ADD..." menu
          state = STATE_ADD_MENU;
        }

        // If the "DELETE" menu is selected
        if (contextMenu.selectedItem == 1) {

          // Delete the selected opcode
          deleteOpcode();

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "MOVE UP" menu is selected
        if (contextMenu.selectedItem == 2) {

          // Move the selected opcode up
          moveOpcodeUp();

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "MOVE DOWN" menu is selected
        if (contextMenu.selectedItem == 3) {

          // Move the selected opcode down
          moveOpcodeDown();

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "EDIT AS SPRITE" menu is selected
        if (contextMenu.selectedItem == 4) {

        }

        // If the "EDIT AS DATA" menu is selected
        if (contextMenu.selectedItem == 5) {

          memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);
          opcode = (mem[memAddress] << 8) + mem[memAddress + 1];

          // Show the "DATA" screen in edit mode
          dataMenu.setValue(0, opcode >> 8);
          dataMenu.setValue(1, opcode & 255);
          dataMenu.selectedItem = 0;
          screenIsInEditMode = true;
          state = STATE_DATA_MENU;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B)) { //jonne

        // Show the editor
        state = STATE_EDITOR;
      }

      // Show the context menu on top of the editor view
      editorView.show();
      contextMenu.show();

      break;

    //----- "ADD..." menu ----------------------------------------------------------------------------------
    case STATE_ADD_MENU:

      // Update ListBox component
      addMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "VX = ..." menu is selected
        if (addMenu.selectedItem == 0) {

          // Show the "VX = ..." menu
          state = STATE_VX_EQUALS_MENU;
        }

        // If the "I = ..." menu is selected
        else if (addMenu.selectedItem == 1) {

          // Show the "I = ..." menu
          state = STATE_I_EQUALS_MENU;
        }

        // If the "SPRITE VX VY N" menu is selected
        else if (addMenu.selectedItem == 2) {

          // Show the "SPRITE VX VY N" screen
          state = STATE_SPRITE_MENU;
        }

        // If the "IF VX ..." menu is selected
        else if (addMenu.selectedItem == 3) {

          // Show the "IF VX ..." menu
          state = STATE_IF_VX_MENU;
        }

        // If the "GOTO N" menu is selected
        else if (addMenu.selectedItem == 4) {

          // Show the "GOTO N" menu
          state = STATE_GOTO_N_MENU;
        }

        // If the "CALL N" menu is selected
        else if (addMenu.selectedItem == 5) {

          // Show the "CALL N" menu
          state = STATE_CALL_N_MENU;
        }

        // If the "IF KEY VX" menu is selected
        else if (addMenu.selectedItem == 6) {

          // Show the "IF KEY VX" menu
          state = STATE_IF_KEY_VX_MENU;
        }

        // If the "IF NOT KEY VX" menu is selected
        else if (addMenu.selectedItem == 7) {

          // Show the "IF NOT KEY VX" menu
          state = STATE_IF_NOT_KEY_VX_MENU;
        }

        // If the "RETURN" menu is selected
        else if (addMenu.selectedItem == 8) {

          // Add opcode 00EE to memory
          addOpcode(0x00EE);

          // Show the editor
          state = STATE_EDITOR;
        }

        // If the "TIMER = VX" menu is selected
        else if (addMenu.selectedItem == 9) {

          // Show the "TIMER = VX" menu
          state = STATE_TIMER_MENU;
        }

        // If the "DATA" menu is selected
        else if (addMenu.selectedItem == 10) {

          // Show the "DATA" menu
          state = STATE_DATA_MENU;
        }

        // If the "SAVEDEC VX" menu is selected
        else if (addMenu.selectedItem == 11) {

          // Show the "SAVEDEC VX" menu
          state = STATE_SAVEDEC_VX_MENU;
        }

        // If the "SOUNDTIMER = VX" menu is selected
        else if (addMenu.selectedItem == 12) {

          // Show the "SOUNDTIMER = VX" menu
          state = STATE_SOUNDTIMER_MENU;
        }

        // If the "SAVE V0 TO VX" menu is selected
        else if (addMenu.selectedItem == 13) {

          // Show the "SAVE V0 TO VX" menu
          state = STATE_SAVE_V0_TO_VX_MENU;
        }

        // If the "LOAD V0 TO VX" menu is selected
        else if (addMenu.selectedItem == 14) {

          // Show the "LOAD V0 TO VX" menu
          state = STATE_LOAD_V0_TO_VX_MENU;
        }

        // If the "GOTO N + V0" menu is selected
        else if (addMenu.selectedItem == 15) {

          // Show the "GOTO N + V0" menu
          state = STATE_GOTO_N_PLUS_V0_MENU;
        }

        // If the "CLS" menu is selected
        else if (addMenu.selectedItem == 16) {

          // Add opcode 00E0 to memory
          addOpcode(0x00E0);

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B)) { //jonne

        // If the editor is not empty
        if (editorView.itemsCount) {

          // Go back to the context menu
          contextMenu.selectedItem = 0;
          state = STATE_CONTEXT_MENU;
        }

        // If the editor is empty
        else {

          // Go back to the editor
          state = STATE_EDITOR;
        }
      }

      // Show the "ADD..." menu
      pokitto->display.print(40, 2, "ADD...");
      addMenu.show();

      break;

    //----- "VX = ..." menu --------------------------------------------------------------------------------
    case STATE_VX_EQUALS_MENU:

      // Update ListBox component
      vxEqualsMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "VX = N" menu is selected
        if (vxEqualsMenu.selectedItem == 0) {

          // Show the "VX = N" screen
          state = STATE_VX_EQUALS_N_MENU;
        }

        // If the "VX = VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 1) {

          // Show the "VX = VY" screen
          state = STATE_VX_EQUALS_VY_MENU;
        }

        // If the "VX = VX + N" menu is selected
        else if (vxEqualsMenu.selectedItem == 2) {

          // Show the "VX = VX + N" screen
          state = STATE_VX_PLUS_N_MENU;
        }

        // If the "VX = VX + VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 3) {

          // Show the "VX = VX + VY" screen
          state = STATE_VX_PLUS_VY_MENU;
        }

        // If the "VX = VX - VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 4) {

          // Show the "VX = VX - VY" screen
          state = STATE_VX_MINUS_VY_MENU;
        }

        // If the "VX = VY - VX" menu is selected
        else if (vxEqualsMenu.selectedItem == 5) {

          // Show the "VX = VY - VX" screen
          state = STATE_VY_MINUS_VX_MENU;
        }

        // If the "VX = VX AND VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 6) {

          // Show the "VX = VX AND VY" screen
          state = STATE_VX_AND_VY_MENU;
        }

        // If the "VX = VX OR VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 7) {

          // Show the "VX = VX OR VY" screen
          state = STATE_VX_OR_VY_MENU;
        }

        // If the "VX = VX XOR VY" menu is selected
        else if (vxEqualsMenu.selectedItem == 8) {

          // Show the "VX = VX XOR VY" screen
          state = STATE_VX_XOR_VY_MENU;
        }

        // If the "VX = VX << 1" menu is selected
        else if (vxEqualsMenu.selectedItem == 9) {

          // Show the "VX = VX << 1" screen
          state = STATE_VX_LEFTSHIFT_MENU;
        }

        // If the "VX = VX >> 1" menu is selected
        else if (vxEqualsMenu.selectedItem == 10) {

          // Show the "VX = VX >> 1" screen
          state = STATE_VX_RIGHTSHIFT_MENU;
        }

        // If the "VX = RANDOM 0 TO N" menu is selected
        else if (vxEqualsMenu.selectedItem == 11) {

          // Show the "VX = RANDOM 0 TO N" screen
          state = STATE_VX_RANDOM_MENU;
        }

        // If the "VX = TIMER" menu is selected
        else if (vxEqualsMenu.selectedItem == 12) {

          // Show the "VX = TIMER" screen
          state = STATE_VX_TIMER_MENU;
        }

        // If the "VX = READ KEY" menu is selected
        else if (vxEqualsMenu.selectedItem == 13) {

          // Show the "VX = READ KEY" screen
          state = STATE_VX_READKEY_MENU;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B)) { //jonne

        // Go back to the previous menu
        state = STATE_ADD_MENU;
      }

      // Show the "VX = ..." menu
      pokitto->display.print(30, 2, "VX = ...");
      vxEqualsMenu.show();

      break;

     //----- "VX = N" menu (6XNN) ---------------------------------------------------------------------------------
    case STATE_VX_EQUALS_N_MENU:

      // Update FieldsList component
      vxEqualsNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //

        // If the "CANCEL" item is selected
        if (vxEqualsNMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxEqualsNMenu.selectedItem == 3) {

          // Add opcode 6XNN to memory
          opcode = (((6 << 4) + vxEqualsNMenu.items[0].value) << 8) + // 6X
                   vxEqualsNMenu.items[1].value;                      // NN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B) && !vxEqualsNMenu.changeCanceled) { //jonne

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(37, TITLE_Y, "VX = N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "N:");
      vxEqualsNMenu.show();

      break;

    //----- "VX = VY" menu (8XY0) ---------------------------------------------------------------------------------
    case STATE_VX_EQUALS_VY_MENU:

      // Update FieldsList component
      vxEqualsVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "CANCEL" item is selected
        if (vxEqualsVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxEqualsVyMenu.selectedItem == 3) {

          // Add opcode 8XY0 to memory
          opcode = (((8 << 4) + vxEqualsVyMenu.items[0].value) << 8) + // 8X
                   (vxEqualsVyMenu.items[1].value << 4);               // Y0

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B) && !vxEqualsVyMenu.changeCanceled) { //jonne

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(35, TITLE_Y, "VX = VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxEqualsVyMenu.show();

      break;

    //----- "VX = VX + N" menu (7XNN) ----------------------------------------------------------------------
    case STATE_VX_PLUS_N_MENU:

      // Update FieldsList component
      vxPlusNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "CANCEL" item is selected
        if (vxPlusNMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxPlusNMenu.selectedItem == 3) {

          // Add opcode 7XNN to memory
          opcode = (((0x7 << 4) + vxPlusNMenu.items[0].value) << 8) + // 7X
                   vxPlusNMenu.items[1].value;                        // NN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B) && !vxPlusNMenu.changeCanceled) { //jonne

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX + N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(25, TITLE_Y, "VX = VX + N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "N:");
      vxPlusNMenu.show();

      break;

    //----- "VX = VX + VY" menu (8XY4) ---------------------------------------------------------------------
    case STATE_VX_PLUS_VY_MENU:

      // Update FieldsList component
      vxPlusVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.released(BTN_A)) { //jonne

        // If the "CANCEL" item is selected
        if (vxPlusVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxPlusVyMenu.selectedItem == 3) {

          // Add opcode 8XY4 to memory
          opcode = (((0x8 << 4) + vxPlusVyMenu.items[0].value) << 8) + // 8X
                   (vxPlusVyMenu.items[1].value << 4) + 0x4;           // Y4

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.released(BTN_B) && !vxPlusVyMenu.changeCanceled) { //jonne

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX + VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(23, TITLE_Y, "VX = VX + VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxPlusVyMenu.show();

      break;

    //----- "VX = VX - VY" menu (8XY5) ---------------------------------------------------------------------
    case STATE_VX_MINUS_VY_MENU:

      // Update FieldsList component
      vxMinusVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxMinusVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxMinusVyMenu.selectedItem == 3) {

          // Add opcode 8XY5 to memory
          opcode = (((0x8 << 4) + vxMinusVyMenu.items[0].value) << 8) + // 8X
                   (vxMinusVyMenu.items[1].value << 4) + 0x5;           // Y5

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxMinusVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX - VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(23, TITLE_Y, "VX = VX - VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxMinusVyMenu.show();

      break;

    //----- "VX = VY - VX" menu (8XY7) ---------------------------------------------------------------------
    case STATE_VY_MINUS_VX_MENU:

      // Update FieldsList component
      vyMinusVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vyMinusVxMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vyMinusVxMenu.selectedItem == 3) {

          // Add opcode 8XY7 to memory
          opcode = (((0x8 << 4) + vyMinusVxMenu.items[0].value) << 8) + // 8X
                   (vyMinusVxMenu.items[1].value << 4) + 0x7;           // Y7

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vyMinusVxMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VY - VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(23, TITLE_Y, "VX = VY - VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vyMinusVxMenu.show();

      break;

    //----- "VX = VX AND VY" menu (8XY2) ---------------------------------------------------------------------
    case STATE_VX_AND_VY_MENU:

      // Update FieldsList component
      vxAndVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxAndVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxAndVyMenu.selectedItem == 3) {

          // Add opcode 8XY2 to memory
          opcode = (((0x8 << 4) + vxAndVyMenu.items[0].value) << 8) + // 8X
                   (vxAndVyMenu.items[1].value << 4) + 0x2;           // Y2

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxAndVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX AND VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(18, TITLE_Y, "VX = VX AND VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxAndVyMenu.show();

      break;

    //----- "VX = VX OR VY" menu (8XY1) ---------------------------------------------------------------------
    case STATE_VX_OR_VY_MENU:

      // Update FieldsList component
      vxOrVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxOrVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxOrVyMenu.selectedItem == 3) {

          // Add opcode 8XY1 to memory
          opcode = (((0x8 << 4) + vxOrVyMenu.items[0].value) << 8) + // 8X
                   (vxOrVyMenu.items[1].value << 4) + 0x1;           // Y1

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxOrVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX OR VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(20, TITLE_Y, "VX = VX OR VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxOrVyMenu.show();

      break;

    //----- "VX = VX XOR VY" menu (8XY3) ---------------------------------------------------------------------
    case STATE_VX_XOR_VY_MENU:

      // Update FieldsList component
      vxXorVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxXorVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxXorVyMenu.selectedItem == 3) {

          // Add opcode 8XY3 to memory
          opcode = (((0x8 << 4) + vxXorVyMenu.items[0].value) << 8) + // 8X
                   (vxXorVyMenu.items[1].value << 4) + 0x3;           // Y3

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxXorVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX XOR VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(18, TITLE_Y, "VX = VX XOR VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY:");
      vxXorVyMenu.show();

      break;

    //----- "VX = VX << 1" menu (8XYE) ---------------------------------------------------------------------
    case STATE_VX_LEFTSHIFT_MENU:

      // Update FieldsList component
      vxLeftShiftMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxLeftShiftMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxLeftShiftMenu.selectedItem == 2) {

          // Add opcode 8XYE to memory
          opcode = (((0x8 << 4) + vxLeftShiftMenu.items[0].value) << 8) + // 8X
                   0xE;                                                   // YE (Y is ignored)

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxLeftShiftMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX << 1" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(23, TITLE_Y, "VX = VX << 1");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      vxLeftShiftMenu.show();

      break;

    //----- "VX = VX >> 1" menu (8XY6) ---------------------------------------------------------------------
    case STATE_VX_RIGHTSHIFT_MENU:

      // Update FieldsList component
      vxRightShiftMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxRightShiftMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxRightShiftMenu.selectedItem == 2) {

          // Add opcode 8XY6 to memory
          opcode = (((0x8 << 4) + vxRightShiftMenu.items[0].value) << 8) + // 8X
                   0x6;                                                    // Y6 (Y is ignored)

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxRightShiftMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = VX >> 1" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(23, TITLE_Y, "VX = VX >> 1");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      vxRightShiftMenu.show();

      break;

    //----- "VX = RANDOM 0 TO N" menu (CXNN) ---------------------------------------------------------------------
    case STATE_VX_RANDOM_MENU:

      // Update FieldsList component
      vxRandomMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxRandomMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxRandomMenu.selectedItem == 3) {

          // Add opcode CXNN to memory
          opcode = (((0xC << 4) + vxRandomMenu.items[0].value) << 8) + // CX
                   vxRandomMenu.items[1].value;                        // NN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxRandomMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = RANDOM 0 TO N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(6, TITLE_Y, "VX = RANDOM 0 TO N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX:");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "N:");
      vxRandomMenu.show();

      break;

    //----- "VX = TIMER" menu (FX07) ---------------------------------------------------------------------
    case STATE_VX_TIMER_MENU:

      // Update FieldsList component
      vxTimerMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxTimerMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxTimerMenu.selectedItem == 2) {

          // Add opcode FX07 to memory
          opcode = (((0xF << 4) + vxTimerMenu.items[0].value) << 8) + // FX
                   0x7;                                               // 07

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxTimerMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = TIMER" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(26, TITLE_Y, "VX = TIMER");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      vxTimerMenu.show();

      break;

    //----- "VX = READ KEY" menu (FX0A) ---------------------------------------------------------------------
    case STATE_VX_READKEY_MENU:

      // Update FieldsList component
      vxReadKeyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (vxReadKeyMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_VX_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (vxReadKeyMenu.selectedItem == 2) {

          // Add opcode FX0A to memory
          opcode = (((0xF << 4) + vxReadKeyMenu.items[0].value) << 8) + // FX
                   0xA;                                                 // 0A

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !vxReadKeyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_VX_EQUALS_MENU;
        }
      }

      // Show the "VX = READ KEY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(19, TITLE_Y, "VX = READ KEY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      vxReadKeyMenu.show();

      break;

    //----- "I = ..." menu -------------------------------------------------------------------------------
    case STATE_I_EQUALS_MENU:

      // Update ListBox component
      iEqualsMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "I = N" menu is selected
        if (iEqualsMenu.selectedItem == 0) {

           // Show the "I = N" screen
           state = STATE_I_EQUALS_N_MENU;
        }

        // If the "I = CHAR VX" menu is selected
        else if (iEqualsMenu.selectedItem == 1) {

           // Show the "I = CHAR VX" screen
           state = STATE_I_EQUALS_CHAR_VX_MENU;
        }

        // If the "I = I + VX" menu is selected
        else if (iEqualsMenu.selectedItem == 2) {

           // Show the "I = I + VX" screen
           state = STATE_I_PLUS_VX_MENU;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B)) {

        // Go back to the previous menu
        state = STATE_ADD_MENU;
      }

      // Show the "I = ..." menu
      pokitto->display.print(35, TITLE_Y, "I = ...");
      iEqualsMenu.show();

      break;

    //----- "I = N" menu (ANNN)---------------------------------------------------------------------------------
    case STATE_I_EQUALS_N_MENU:

      // Update FieldsList component
      iEqualsNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (iEqualsNMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_I_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (iEqualsNMenu.selectedItem == 2) {

          // Add opcode ANNN to memory
          opcode = (0xA << 12) + iEqualsNMenu.items[0].value; // ANNN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !iEqualsNMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_I_EQUALS_MENU;
        }
      }

      // Show the "I = N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(40, TITLE_Y, "I = N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "N: ");
      iEqualsNMenu.show();

      break;

    //----- "I = CHAR VX" menu (FX29) ---------------------------------------------------------------------
    case STATE_I_EQUALS_CHAR_VX_MENU:

      // Update FieldsList component
      iEqualsCharVXMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (iEqualsCharVXMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_I_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (iEqualsCharVXMenu.selectedItem == 2) {

          // Add opcode FX29 to memory
          opcode = (((0xF << 4) + iEqualsCharVXMenu.items[0].value) << 8) + // FX
                   0x29;                                                    // 29

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !iEqualsCharVXMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_I_EQUALS_MENU;
        }
      }

      // Show the "I = CHAR VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(24, TITLE_Y, "I = CHAR VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      iEqualsCharVXMenu.show();

      break;

    //----- "I = I + VX" menu (FX1E) ---------------------------------------------------------------------
    case STATE_I_PLUS_VX_MENU:

      // Update FieldsList component
      iPlusVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (iPlusVxMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_I_EQUALS_MENU;
          }
        }

        // If the "OK" item is selected
        else if (iPlusVxMenu.selectedItem == 2) {

          // Add opcode FX1E to memory
          opcode = (((0xF << 4) + iPlusVxMenu.items[0].value) << 8) + // FX
                   0x1E;                                              // 1E

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !iPlusVxMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_I_EQUALS_MENU;
        }
      }

      // Show the "I = I + VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(20, TITLE_Y, "I = I + VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      iPlusVxMenu.show();

      break;

    //----- "SPRITE VX VY N" menu (DXYN) ---------------------------------------------------------------------------------
    case STATE_SPRITE_MENU:

      // Update FieldsList component
      spriteMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (spriteMenu.selectedItem == 3) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (spriteMenu.selectedItem == 4) {

          // Add opcode DXYN to memory
          opcode = (((0xD << 4) + spriteMenu.items[0].value) << 8) +               // DX
                   ((spriteMenu.items[1].value << 4) + spriteMenu.items[2].value); // YN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !spriteMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "SPRITE VX VY N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(16, TITLE_Y, "SPRITE VX VY N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_3, "VX: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_3, "VY: ");
      pokitto->display.print(PARAM_X, PARAM_Y_3_3, "N: ");
      spriteMenu.show();

      break;

    //----- "IF VX  ..." menu -------------------------------------------------------------------------------
    case STATE_IF_VX_MENU:

      // Update ListBox component
      ifVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "IF VX == N" menu is selected
        if (ifVxMenu.selectedItem == 0) {

           // Show the "IF VX == N" screen
           state = STATE_IF_VX_EQUALS_N_MENU;
        }

        // If the "IF VX == VY" menu is selected
        else if (ifVxMenu.selectedItem == 1) {

           // Show the "IF VX == VY" screen
           state = STATE_IF_VX_EQUALS_VY_MENU;
        }

        // If the "IF VX != N" menu is selected
        else if (ifVxMenu.selectedItem == 2) {

           // Show the "IF VX != N" screen
           state = STATE_IF_VX_NOT_EQUALS_N_MENU;
        }

        // If the "IF VX != VY" menu is selected
        else if (ifVxMenu.selectedItem == 3) {

           // Show the "IF VY != N" screen
           state = STATE_IF_VX_NOT_EQUALS_VY_MENU;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B)) {

        // Go back to the previous menu
        state = STATE_ADD_MENU;
      }

      // Show the "I = ..." menu
      pokitto->display.print(30, TITLE_Y, "IF VX ...");
      ifVxMenu.show();

      break;

    //----- "IF VX == N" menu (4XNN)---------------------------------------------------------------------------------
    case STATE_IF_VX_EQUALS_N_MENU:

      // Update FieldsList component
      ifVxEqualsNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifVxEqualsNMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_IF_VX_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifVxEqualsNMenu.selectedItem == 3) {

          // Add opcode 4XNN to memory
          opcode = (((0x4 << 4) + ifVxEqualsNMenu.items[0].value) << 8) + // 4X
                   ifVxEqualsNMenu.items[1].value;                        // NN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifVxEqualsNMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_IF_VX_MENU;
        }
      }

      // Show the "IF VX == N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(27, TITLE_Y, "IF VX == N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "N: ");
      ifVxEqualsNMenu.show();

      break;

    //----- "IF VX == VY" menu (9XY0)---------------------------------------------------------------------------------
    case STATE_IF_VX_EQUALS_VY_MENU:

      // Update FieldsList component
      ifVxEqualsVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifVxEqualsVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_IF_VX_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifVxEqualsVyMenu.selectedItem == 3) {

          // Add opcode 9XY0 to memory
          opcode = (((0x9 << 4) + ifVxEqualsVyMenu.items[0].value) << 8) + // 9X
                   (ifVxEqualsVyMenu.items[1].value << 4);                 // Y0

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifVxEqualsVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_IF_VX_MENU;
        }
      }

      // Show the "IF VX == VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(24, TITLE_Y, "IF VX == VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY: ");
      ifVxEqualsVyMenu.show();

      break;

    //----- "IF VX != N" menu (3XNN)---------------------------------------------------------------------------------
    case STATE_IF_VX_NOT_EQUALS_N_MENU:

      // Update FieldsList component
      ifVxNotEqualsNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifVxNotEqualsNMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_IF_VX_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifVxNotEqualsNMenu.selectedItem == 3) {

          // Add opcode 3XNN to memory
          opcode = (((0x3 << 4) + ifVxNotEqualsNMenu.items[0].value) << 8) + // 3X
                   ifVxNotEqualsNMenu.items[1].value;                        // NN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifVxNotEqualsNMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_IF_VX_MENU;
        }
      }

      // Show the "IF VX != N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(27, TITLE_Y, "IF VX != N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "N: ");
      ifVxNotEqualsNMenu.show();

      break;

    //----- "IF VX != VY" menu (5XY0)---------------------------------------------------------------------------------
    case STATE_IF_VX_NOT_EQUALS_VY_MENU:

      // Update FieldsList component
      ifVxNotEqualsVyMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifVxNotEqualsVyMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_IF_VX_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifVxNotEqualsVyMenu.selectedItem == 3) {

          // Add opcode 5XY0 to memory
          opcode = (((0x5 << 4) + ifVxNotEqualsVyMenu.items[0].value) << 8) + // 5X
                   (ifVxNotEqualsVyMenu.items[1].value << 4);                 // Y0

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifVxNotEqualsVyMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_IF_VX_MENU;
        }
      }

      // Show the "IF VX != VY" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(24, TITLE_Y, "IF VX != VY");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "VX: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "VY: ");
      ifVxNotEqualsVyMenu.show();

      break;

    //----- "GOTO N" menu (1NNN)---------------------------------------------------------------------------------
    case STATE_GOTO_N_MENU:

      // Update FieldsList component
      gotoNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (gotoNMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (gotoNMenu.selectedItem == 2) {

          // Add opcode 1NNN to memory
          opcode = (0x1 << 12) + gotoNMenu.items[0].value; // 1NNN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !gotoNMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "GOTO N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(37, TITLE_Y, "GOTO N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "N: ");
      gotoNMenu.show();

      break;

    //----- "CALL N" menu (2NNN)---------------------------------------------------------------------------------
    case STATE_CALL_N_MENU:

      // Update FieldsList component
      callNMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (callNMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (callNMenu.selectedItem == 2) {

          // Add opcode 2NNN to memory
          opcode = (0x2 << 12) + callNMenu.items[0].value; // 2NNN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !callNMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "CALL N" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(37, TITLE_Y, "CALL N");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "N: ");
      callNMenu.show();

      break;

    //----- "IF KEY VX" menu (EXA1) ---------------------------------------------------------------------
    case STATE_IF_KEY_VX_MENU:

      // Update FieldsList component
      ifKeyVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifKeyVxMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifKeyVxMenu.selectedItem == 2) {

          // Add opcode EXA1 to memory
          opcode = (((0xE << 4) + ifKeyVxMenu.items[0].value) << 8) + // EX
                   0xA1;                                              // A1

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifKeyVxMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "IF KEY VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(28, TITLE_Y, "IF KEY VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      ifKeyVxMenu.show();

      break;

    //----- "IF NOT KEY VX" menu (EX9E) ---------------------------------------------------------------------
    case STATE_IF_NOT_KEY_VX_MENU:

      // Update FieldsList component
      ifNotKeyVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (ifNotKeyVxMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (ifNotKeyVxMenu.selectedItem == 2) {

          // Add opcode EX9E to memory
          opcode = (((0xE << 4) + ifNotKeyVxMenu.items[0].value) << 8) + // EX
                   0x9E;                                                 // 9E

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !ifNotKeyVxMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "IF NOT KEY VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(18, TITLE_Y, "IF NOT KEY VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      ifNotKeyVxMenu.show();

      break;

    //----- "TIMER = VX" menu (FX15) ---------------------------------------------------------------------
    case STATE_TIMER_MENU:

      // Update FieldsList component
      timerMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (timerMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (timerMenu.selectedItem == 2) {

          // Add opcode FX15 to memory
          opcode = (((0xF << 4) + timerMenu.items[0].value) << 8) + // FX
                   0x15;                                            // 15

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !timerMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "TIMER = VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(27, TITLE_Y, "TIMER = VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      timerMenu.show();

      break;

    //----- "DATA" menu ---------------------------------------------------------------------------------
    case STATE_DATA_MENU:

      // Update FieldsList component
      dataMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (dataMenu.selectedItem == 2) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (dataMenu.selectedItem == 3) {

          // Add opcode to memory
          opcode = (dataMenu.items[0].value << 8) + // DATA 1
                   dataMenu.items[1].value;         // DATA 2

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !dataMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "DATA" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(42, TITLE_Y, "DATA");
      pokitto->display.print(PARAM_X, PARAM_Y_1_2, "DATA 1: ");
      pokitto->display.print(PARAM_X, PARAM_Y_2_2, "DATA 2: ");
      dataMenu.show();

      break;

    //----- "SAVEDEC VX" menu (FX33) ---------------------------------------------------------------------
    case STATE_SAVEDEC_VX_MENU:

      // Update FieldsList component
      saveDecVxMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (saveDecVxMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (saveDecVxMenu.selectedItem == 2) {

          // Add opcode FX33 to memory
          opcode = (((0xF << 4) + saveDecVxMenu.items[0].value) << 8) + // FX
                   0x33;                                                // 33

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !saveDecVxMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "SAVEDEC VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(25, TITLE_Y, "SAVEDEC VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      saveDecVxMenu.show();

      break;

    //----- "SOUNDTIMER = VX" menu (FX18) ---------------------------------------------------------------------
    case STATE_SOUNDTIMER_MENU:

      // Update FieldsList component
      soundTimerMenu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (soundTimerMenu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (soundTimerMenu.selectedItem == 2) {

          // Add opcode FX18 to memory
          opcode = (((0xF << 4) + soundTimerMenu.items[0].value) << 8) + // FX
                   0x18;                                                 // 18

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !soundTimerMenu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "SOUNDTIMER = VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(12, TITLE_Y, "SOUNDTIMER = VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      soundTimerMenu.show();

      break;

    //----- "SAVE V0 TO VX" menu (FX55) ---------------------------------------------------------------------
    case STATE_SAVE_V0_TO_VX_MENU:

      // Update FieldsList component
      saveV0ToVx.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (saveV0ToVx.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (saveV0ToVx.selectedItem == 2) {

          // Add opcode FX55 to memory
          opcode = (((0xF << 4) + saveV0ToVx.items[0].value) << 8) + // FX
                   0x55;                                             // 55

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !saveV0ToVx.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "SAVE V0 TO VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(19, TITLE_Y, "SAVE V0 TO VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      saveV0ToVx.show();

      break;

    //----- "LOAD V0 TO VX" menu (FX65) ---------------------------------------------------------------------
    case STATE_LOAD_V0_TO_VX_MENU:

      // Update FieldsList component
      loadV0ToVx.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (loadV0ToVx.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (loadV0ToVx.selectedItem == 2) {

          // Add opcode FX55 to memory
          opcode = (((0xF << 4) + loadV0ToVx.items[0].value) << 8) + // FX
                   0x65;                                             // 65

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !loadV0ToVx.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "LOAD V0 TO VX" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(19, TITLE_Y, "LOAD V0 TO VX");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "VX:");
      loadV0ToVx.show();

      break;

    //----- "GOTO N + V0" menu (BNNN) ---------------------------------------------------------------------
    case STATE_GOTO_N_PLUS_V0_MENU:

      // Update FieldsList component
      gotoNPlusV0Menu.update();

      // If the "A" button (select) is pressed
      if (pokitto->buttons.pressed(BTN_A)) {

        // If the "CANCEL" item is selected
        if (gotoNPlusV0Menu.selectedItem == 1) {

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Go back to the editor
            state = STATE_EDITOR;
          }

          // If the screen is in add mode
          else {

            // Go back to the previous menu
            state = STATE_ADD_MENU;
          }
        }

        // If the "OK" item is selected
        else if (gotoNPlusV0Menu.selectedItem == 2) {

          // Add opcode BNNN to memory
          opcode = (0xB << 12) + gotoNPlusV0Menu.items[0].value; // BNNN

          // If the screen is in edit mode
          if (screenIsInEditMode) {

            // Cancel the edit mode
            screenIsInEditMode = false;

            // Update opcode in memory
            updateOpcode(opcode);
          }

          // If the screen is in add mode
          else {

            // Add opcode to memory
            addOpcode(opcode);
          }

          // Show the editor
          state = STATE_EDITOR;
        }
      }

      // If the "B" button (cancel) is pressed
      else if (pokitto->buttons.pressed(BTN_B) && !gotoNPlusV0Menu.changeCanceled) {

        // If the screen is in edit mode
        if (screenIsInEditMode) {

          // Cancel the edit mode
          screenIsInEditMode = false;

          // Go back to the editor
          state = STATE_EDITOR;
        }

        // If the screen is in add mode
        else {

          // Go back to the previous menu
          state = STATE_ADD_MENU;
        }
      }

      // Show the "GOTO N + V0" screen
      pokitto->display.drawRect(FRAME_X, FRAME_Y, FRAME_WIDTH, FRAME_HEIGHT);
      pokitto->display.print(25, TITLE_Y, "GOTO N + V0");
      pokitto->display.print(PARAM_X, PARAM_Y_1_1, "N: ");
      gotoNPlusV0Menu.show();

      break;

     //---------------------------------------------------------------------------------------------------
    default:

      break;
  }
}

// ----------------------------------------------------------------------------------------------------
// Get an opcode index from memory
// ----------------------------------------------------------------------------------------------------
uint8_t CHIP8Player::getOpcodeIndex(uint16_t opcode) {

  uint8_t opcodeIndex = 0;
  uint8_t opcodeIndexFound = 0;

  for (opcodeIndex=0; opcodeIndex<OPCODES_COUNT; opcodeIndex++) {

    if ((opcode&opcodesMasks[opcodeIndex]) == opcodesIds[opcodeIndex]) {

      opcodeIndexFound = opcodeIndex;
      break;
    }
  }

  return opcodeIndexFound;
}

// ----------------------------------------------------------------------------------------------------
// Get an opcode from memory
// ----------------------------------------------------------------------------------------------------
uint16_t CHIP8Player::getOpcode(uint16_t memAddress) {

  return (mem[memAddress] <<8) + mem[memAddress + 1];
}

// ----------------------------------------------------------------------------------------------------
// Add an opcode in memory
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::addOpcode(uint16_t opcode) {

  uint16_t i;
  uint16_t memAddress;

  memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

  if (romSize == 0) {

    mem[ROM_START] = opcode >> 8;
    mem[ROM_START + 1] = opcode & 255;

    romSize += 2;

    editorView.addItem("");

    updateEditorView();
  }

  else if (ROM_START + romSize < MEM_SIZE) {

    for (i=ROM_START+romSize-2; i>memAddress; i-=2) {

      mem[i+2] = mem[i];
      mem[i+3] = mem[i+1];
    }

    mem[memAddress + 2] = opcode >> 8;
    mem[memAddress + 3] = opcode & 255;

    romSize += 2;

    if (editorView.itemsCount < 9) {

      editorView.addItem("");
    }

    editorView.selectedItem++;
    if (editorView.selectedItem > LAST_EDITOR_LINE) {

      firstInstructionToShow += 2;
      editorView.selectedItem = LAST_EDITOR_LINE;
    }

    updateEditorView();
  }
}

// ----------------------------------------------------------------------------------------------------
// Delete an opcode from memory
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::deleteOpcode() {

  uint16_t i;
  uint16_t memAddress;

  memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

  if (romSize > 0) {

    for (i=memAddress; i<ROM_START+romSize-3; i+=2) {

      mem[i] = mem[i+2];
      mem[i+1] = mem[i+3];
    }

    romSize -= 2;
  }

  if (romSize < (EDITOR_HEIGHT << MULTIPLY_BY_2)) {

    editorView.removeItem(editorView.selectedItem);
  }

  // Scroll editor view up
  if ((ROM_START + romSize) - firstInstructionToShow < (EDITOR_HEIGHT << MULTIPLY_BY_2) && firstInstructionToShow > ROM_START) {

    firstInstructionToShow -= 2;

    if (editorView.selectedItem < LAST_EDITOR_LINE) {

      editorView.selectedItem++;
    }
  }

  updateEditorView();
}

// ----------------------------------------------------------------------------------------------------
// Update an opcode in memory
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::updateOpcode(uint16_t opcode) {

  uint16_t memAddress;

  memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

  mem[memAddress] = opcode >> 8;
  mem[memAddress + 1] = opcode & 255;

  updateEditorView();
}

// ----------------------------------------------------------------------------------------------------
// Shift an opcode left in memory
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::moveOpcodeUp() {

  uint8_t tempByte1;
  uint8_t tempByte2;
  uint16_t memAddress;

  memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

  if (memAddress > ROM_START + 1) {

    tempByte1 = mem[memAddress - 2];
    tempByte2 = mem[memAddress - 1];

    mem[memAddress - 2] = mem[memAddress];
    mem[memAddress - 1] = mem[memAddress + 1];

    mem[memAddress] = tempByte1;
    mem[memAddress + 1] = tempByte2;

    editorView.selectedItem--;
    if (editorView.selectedItem == 65535) {

      firstInstructionToShow -= 2;
      editorView.selectedItem = FIRST_EDITOR_LINE;
    }

    updateEditorView();
  }
}

// ----------------------------------------------------------------------------------------------------
// Shift an opcode right in memory
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::moveOpcodeDown() {

  uint8_t tempByte1;
  uint8_t tempByte2;
  uint16_t memAddress;

  memAddress = firstInstructionToShow + (editorView.selectedItem << MULTIPLY_BY_2);

  if (memAddress < ROM_START + romSize - 3) {

    tempByte1 = mem[memAddress + 2];
    tempByte2 = mem[memAddress + 3];

    mem[memAddress + 2] = mem[memAddress];
    mem[memAddress + 3] = mem[memAddress + 1];

    mem[memAddress] = tempByte1;
    mem[memAddress + 1] = tempByte2;

    editorView.selectedItem++;
    if (editorView.selectedItem > LAST_EDITOR_LINE) {

      firstInstructionToShow += 2;
      editorView.selectedItem = LAST_EDITOR_LINE;
    }

    updateEditorView();
  }
}


// ----------------------------------------------------------------------------------------------------
// Set a pixel in a screen array
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::setPixel(uint8_t x, uint8_t y, uint8_t color) {

  uint16_t pixelPosInScreen = 0;    // Position of the pixel bit in the screen array.
  uint16_t pixelBytePosInScreen = 0; // Position of the pixel byte in the screen array.
  uint8_t pixelByte = 0;            // Pixel byte read from the screen array.
  uint8_t pixelPosInPixelByte = 0;  // Pixel position in the pixel byte (0: left position, 1: right position)
  uint8_t pixelByteShift = 0;       // Shift pixel byte to the right if the pixel is on the left position of the byte
  uint8_t pixelByteMask = 0;        // Mask used to read the pixel value
  uint8_t newPixelByte = 0;         // New pixel byte with the new pixel updated.

  pixelPosInScreen = y * screenWidth + x;
  pixelBytePosInScreen = pixelPosInScreen >> DIVIDE_BY_2;
  pixelByte = screen[pixelBytePosInScreen];
  pixelPosInPixelByte = pixelPosInScreen & 1; // Equals to pixelPosInScreen % 2. Pixel in the left position: 0, pixel in the right position: 1

  // Clear old pixel value in the pixel byte
  pixelByteShift = pixelPosInPixelByte << MULTIPLY_BY_4; // Pixel in the left position: 0, pixel in the right position: 4
  pixelByteMask = 15 << pixelByteShift; // Pixel in the left position: 0b00001111, Pixel in the right position: 0b11110000
  newPixelByte = pixelByte & pixelByteMask;

  // Set new pixel value in the pixel byte
  pixelByteShift = (1 - pixelPosInPixelByte) << MULTIPLY_BY_4; // Pixel in the left position: 4, pixel in the right position: 0
  newPixelByte = newPixelByte | (color << pixelByteShift); // Set new pixel value

  screen[pixelBytePosInScreen] = newPixelByte;
}

// ----------------------------------------------------------------------------------------------------
// Read a pixel from a given screen array
// ----------------------------------------------------------------------------------------------------
uint8_t CHIP8Player::getPixel(uint8_t x, uint8_t y) {

  uint16_t pixelPosInScreen = 0;    // Position of the pixel in the screen array.
  uint16_t pixelBytePosInScreen = 0; // Position of the pixel byte in the screen array.
  uint8_t pixelByte = 0;            // Pixel byte read from the screen array.
  uint8_t pixelPosInPixelByte = 0;  // Pixel position in the pixel byte (0: left position, 1: right position)
  uint8_t pixelByteShift = 0;       // Shift pixel byte to the right if the pixel is on the left position of the byte
  uint8_t pixel = 0;                // Pixel color read from the screen array : 0 to 15.

  pixelPosInScreen = y * screenWidth + x;
  pixelBytePosInScreen = pixelPosInScreen >> DIVIDE_BY_2;
  pixelByte = screen[pixelBytePosInScreen];
  pixelPosInPixelByte = pixelPosInScreen & 1; // Equals to pixelPosInScreen % 2. Pixel in left position: 0, pixel in right position: 1
  pixelByteShift = (1 - pixelPosInPixelByte) << MULTIPLY_BY_4; // Pixel in left position: 4, pixel in right position: 0
  pixel = (pixelByte >> pixelByteShift) & 15; // 15 = 0b00001111

  return pixel;
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::clearScreen() {

  uint16_t i;

  for (i=0; i<SCREEN_LENGTH; i++) {

    screen[i] = 0;
  }
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::drawScreen() {

  uint8_t x;
  uint8_t y;

  pokitto->display.color = 0;
  pokitto->display.fillRect(0, 0, screenWidth, screenHeight);
  pokitto->display.color = 1;

  for (y=0; y<screenHeight; y++) {

    for (x=0; x<screenWidth; x++) {

      pokitto->display.drawPixel(x, y, getPixel(x, y));
    }
  }
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::drawSprite(uint8_t x, uint8_t y, uint8_t height) {

  uint16_t memIndex = 0;
  uint8_t byte = 0;
  uint8_t mask = 0;
  uint8_t bit = 0;
  uint8_t bitShift = 0;
  uint8_t pixelY = y;
  uint8_t i;

  V[0xF] = 0;

  // For each bytes of the sprite
  for (memIndex=I; memIndex<I+height; memIndex++) {

    // Get the byte and initialize its mask
    byte = mem[memIndex];
    mask = 128; // 0b10000000

    // For each bit of the byte
    for (i=0; i<8; i++) {

      // Get the bit from the byte and its mask
      bitShift = 7 - i;
      bit = (byte & mask) >> bitShift;

      // If the bit equals to 1 a pixel could be displayed
      if (bit) {

        // If a pixel already exists at this place
        if (getPixel(x + i, pixelY)) {

          // VF register must set to 1 (collision detection)
          V[0xF] = 1;

          // The pixel is cleared
          setPixel(x + i, pixelY, 0);
        }

        // If there is no pixel at this place
        else {

          // A pixel is displayed
          setPixel(x + i, pixelY, 1);
          //pokitto->display.drawPixel(x + i, pixelY);
        }
      }

      // Update the mask in order to read the next bit
      mask = mask >> 1;
    }

    // Go to the next line of the sprite
    pixelY++;
  }
}

// ----------------------------------------------------------------------------------------------------
//
//
//
// [1]:A     [2]:UP    [3]:E     [C]:R   1 2 3 C
// [4]:LEFT  [5]:S     [6]:RIGHT [D]:F   4 5 6 D
// [7]:W     [8]:DOWN  [9]:C     [E]:V   7 8 9 E
// [A]:Z     [0]:Q     [B]:D     [F]:X   A 0 B F
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::readKeys() {

  uint8_t i;

  for (i=0; i<16; i++) {

    keys[i] = 0;
  }

  //if (pokitto->buttons.repeat(BTN_B,1))
  if (pokitto->bBtn()) keys[0] = 1; //jonne
  if (pokitto->upBtn()) keys[2] = 1;
  if (pokitto->leftBtn()) keys[4] = 1;
  if (pokitto->aBtn()) keys[5] = 1;
  if (pokitto->rightBtn()) keys[6] = 1;
  if (pokitto->downBtn()) keys[8] = 1;
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::readOpcode() {

    uint16_t opcode = 0;
    uint8_t opcodeIndex = 0;
    uint8_t opcodePart3 = 0;
    uint8_t opcodePart2 = 0;
    uint8_t opcodePart1 = 0;
    uint16_t temp = 0;
    uint8_t i;

    opcode = (mem[PC]<<8) + mem[PC+1];
    //printf("%x\n", opcode);

    opcodeIndex = getOpcodeIndex(opcode);
    //printf("%d\n", opcodeIndex);

    opcodePart3 = (opcode & 0x0F00) >> 8; // Bits 0, 1, 2 et 3
    opcodePart2 = (opcode & 0x00F0) >> 4; // Bits 4, 5, 6 et 7
    opcodePart1 = opcode & 0x000F;        // Bits 8, 9, 10 et 11


    switch (opcodeIndex) {

      case 0:

        // 0NNN: Calls RCA 1802 (not implemented here).
        break;

      case 1:

        // 00E0: Clear the screen
        clearScreen();
        break;

      case 2:

        //00EE: Returns from a subroutine
        if (stackLength > 0) {

          PC = stack[stackLength - 1];
          stackLength--;
        }
        else {

          printf("Invalid return from subroutine, no returning address from stack !\n");
          PC -= 2;
        }
        break;

      case 3:

        // 1NNN: Jumps to address NNN
        PC = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1;
        PC -= 2;
        break;

      case 4:

        // 2NNN: Calls subroutine at NNN
        stack[stackLength] = PC;
        stackLength++;
        PC = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1;
        PC -= 2;
        break;

      case 5:

        // 3XNN: Skips the next instruction if VX equals NN
        if (V[opcodePart3] == ((opcodePart2 << 4) + opcodePart1)) {

          PC += 2;
        }
        break;

      case 6:

        // 4XNN: Skips the next instruction if VX doesn't equal NN
        if (V[opcodePart3] != ((opcodePart2 << 4) + opcodePart1)) {

          PC += 2;
        }
        break;

      case 7:

        // 5XY0: Skips the next instruction if VX equals VY.
        if (V[opcodePart3] == V[opcodePart2]) {

          PC += 2;
        }
        break;

      case 8:

        // 6XNN: Sets VX to NN
        V[opcodePart3] = (opcodePart2 << 4) + opcodePart1;
        break;

      case 9:

        // 7XNN: Adds NN to VX
        V[opcodePart3] += ((opcodePart2 << 4) + opcodePart1);
        break;

      case 10:

        // 8XY0: Sets VX to the value of VY
        V[opcodePart3] = V[opcodePart2];
        break;

      case 11:

        // 8XY1: Sets VX to VX or VY
        V[opcodePart3] = V[opcodePart3] | V[opcodePart2];
        break;

      case 12:

        // 8XY2: Sets VX to VX and VY
        V[opcodePart3] = V[opcodePart3] & V[opcodePart2];
        break;

      case 13:

        // 8XY3: Sets VX to VX xor VY
        V[opcodePart3] = V[opcodePart3] ^ V[opcodePart2];
        break;

      case 14:

        // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
        V[0xF] = 0;
        temp = V[opcodePart3] + V[opcodePart2];
        if (temp > 255) {

          V[0xF] = 1;
        }
        V[opcodePart3] = temp & 0xFF;
        break;

      case 15:

        // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
        if (V[opcodePart3] < V[opcodePart2]) {

          V[0xF] = 0;
        }
        else {

          V[0xF] = 1;
        }
        V[opcodePart3] -= V[opcodePart2];
        //V[opcodePart3] &= 0xFF;
        break;

      case 16:

        // 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
        V[0xF] = V[opcodePart3] & 0x01;
        V[opcodePart3] = V[opcodePart3] >> 1;
        break;

      case 17:

        // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
        if (V[opcodePart2] < V[opcodePart3]) {

          V[0xF] = 0;
        }
        else {

          V[0xF] = 1;
        }
        V[opcodePart3] = V[opcodePart2] - V[opcodePart3];
        //V[opcodePart3] &= 0xFF;
        break;

      case 18:

        // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
        V[0xF] = V[opcodePart3] >> 7;
        V[opcodePart3] = V[opcodePart3] << 1;
        break;

      case 19:

        // 9XY0: Skips the next instruction if VX doesn't equal VY.
        if (V[opcodePart3] != V[opcodePart2]) {

          PC += 2;
        }
        break;

      case 20:

        // ANNN: Sets I to the address NNN
        I = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1;
        break;

      case 21:

        // BNNN: Jumps to the address NNN plus V0
        PC = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1 + V[0];
        PC -= 2;
        break;

      case 22:

        // CXNN: Sets VX to the result of a bitwise and operation on a random byte value (0 to 255) and NN
        V[opcodePart3] = random(255) & ((opcodePart2 << 4) + opcodePart1);
        break;


      case 23:

        // DXYN:  Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
        drawSprite(V[opcodePart3], V[opcodePart2], opcodePart1);
        break;


      case 24:

        // EX9E: Skips the next instruction if the key stored in VX is pressed.
        if(keys[V[opcodePart3]] == 1) {

          PC += 2;
        }
        break;


      case 25:

        // EXA1: Skips the next instruction if the key stored in VX isn't pressed.
        if(keys[V[opcodePart3]] == 0) {

          PC += 2;
        }
        break;

      case 26:

        // FX07: Sets VX to the value of the delay timer.
        V[opcodePart3] = delayTimer;
        break;

      case 27:

        // FX0A: A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)

        // Check if a key has been pressed
        keyPressed = false;
        for (i=0; i<16; i++) {

          // If a key has been pressed
          if (keys[i] == 1) {

            // Store its value in VX
            V[opcodePart3] = i;

            keyPressed = true;
            break;
          }
        }

        if (!keyPressed) {

          PC -= 2;
        }
        break;

      case 28:

        // FX15: Sets the delay timer to VX.
        delayTimer = V[opcodePart3];
        break;

      case 29:

        // FX18: Sets the sound timer to VX
        soundTimer = V[opcodePart3];
        break;

      case 30:

        // FX1E: Adds VX to I
        V[0xF] = 0;
        temp = I + V[opcodePart3];
        if (temp > 0xFFF) {

          V[0xF] = 1;
        }
        I = temp & 0xFFF;
        break;

      case 31:

        // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
        I = V[opcodePart3] * 5;
        break;

      case 32:

        // FX33: Stores the binary-coded decimal representation of VX
        mem[I] = (uint8_t) (V[opcodePart3] / 100);
        mem[I + 1] = (uint8_t) ((V[opcodePart3] % 100) / 10);
        mem[I + 2] = (uint8_t) ((V[opcodePart3] % 100) % 10);
        break;

      case 33:

        // FX55: Stores V0 to VX (including VX) in memory starting at address I
        for (i=0; i<=opcodePart3; i++) {

          mem[I + i] = V[i];
        }
        break;

      case 34:

        // FX65: Fills V0 to VX (including VX) with values from memory starting at address I
        for (i=0; i<=opcodePart3; i++) {

          V[i] = mem[I + i];
        }
        break;

      default:

        printf("Unknown opcode %x found at address %d !\n", opcode, PC);
    }

    PC += 2;

    if (PC >= MEM_SIZE) {

      PC = MEM_SIZE - 2;
    }
  }

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::getOpcodeSource(char *opcodeSource, uint16_t memAddress) {

  uint16_t opcode = 0;
  uint8_t opcodeIndex = 0;
  uint8_t opcodePart3 = 0;
  uint8_t opcodePart2 = 0;
  uint8_t opcodePart1 = 0;
  uint16_t temp = 0;

  opcode = (mem[memAddress]<<8) + mem[memAddress+1];
  //if (opcode == 0) return "0\0";

  //opcodeSource = (char*) malloc(sizeof(*opcodeSource) * 32);

  opcodePart3 = (opcode & 0x0F00) >> 8; // Bits 0, 1, 2 and 3
  opcodePart2 = (opcode & 0x00F0) >> 4; // Bits 4, 5, 6 and 7
  opcodePart1 = opcode & 0x000F;        // Bits 8, 9, 10 and 11

  opcodeIndex = getOpcodeIndex(opcode);

  switch (opcodeIndex) {

    case 0:

      // 0NNN: Calls RCA 1802 (not implemented here and replaced by bytes data).
      sprintf(opcodeSource, "%04u DATA %d %d\n", memAddress, (opcode >> 8), (opcode & 255));
      break;

    case 1:

      // 00E0: Clears the screen.
      sprintf(opcodeSource, "%04u CLS\n", memAddress);
      break;

    case 2:

      //00EE: retour d'une sous-routine.
      sprintf(opcodeSource, "%04u RETURN\n", memAddress);
      break;

    case 3:

      // 1NNN: effectue un saut à l'adresse NNN.
      temp = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1;
      sprintf(opcodeSource, "%04u GOTO %d\n", memAddress, temp);
      break;

    case 4:

      // 2NNN: appelle le sous-programme en NNN, mais on revient ensuite.
      temp = (opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1;
      sprintf(opcodeSource, "%04u CALL %d\n", memAddress, temp);
      break;

    case 5:

      // 3XNN: saute l'instruction suivante si VX est égal à NN.
      sprintf(opcodeSource, "%04u IF V%d != %d\n", memAddress, opcodePart3, ((opcodePart2 << 4) + opcodePart1));
      break;

    case 6:

      // 4XNN: saute l'instruction suivante si VX et NN ne sont pas égaux.
      sprintf(opcodeSource, "%04u IF V%d == %d\n", memAddress, opcodePart3, ((opcodePart2 << 4) + opcodePart1));
      break;

    case 7:

      // 5XY0: saute l'instruction suivante si VX et VY sont égaux.
      sprintf(opcodeSource, "%04u IF V%d != V%d\n", memAddress, opcodePart3, opcodePart2);
      break;

    case 8:

      // 6XNN: définit VX à NN.
      sprintf(opcodeSource, "%04u V%d = %d\n", memAddress, opcodePart3, ((opcodePart2 << 4) + opcodePart1));
      break;

    case 9:

      // 7XNN: ajoute NN à VX.
      sprintf(opcodeSource, "%04u V%d = V%d + %d\n", memAddress, opcodePart3, opcodePart3, ((opcodePart2 << 4) + opcodePart1));
      break;

    case 10:

      // 8XY0: définit VX à la valeur de VY.
      sprintf(opcodeSource, "%04u V%d = V%d\n", memAddress, opcodePart3, opcodePart2);
      break;


    case 11:

      // 8XY1: définit VX à VX OR VY.
      sprintf(opcodeSource, "%04u V%d = V%d OR V%d\n", memAddress, opcodePart3, opcodePart3, opcodePart2);
      break;

    case 12:

      // 8XY2: définit VX à VX AND VY.
      sprintf(opcodeSource, "%04u V%d = V%d AND V%d\n", memAddress, opcodePart3, opcodePart3, opcodePart2);
      break;

    case 13:

      // 8XY3: définit VX à VX XOR VY.
      sprintf(opcodeSource, "%04u V%d = V%d XOR V%d\n", memAddress, opcodePart3, opcodePart3, opcodePart2);
      break;

    case 14:

      // 8XY4: ajoute VY à VX. VF est mis à 1 quand il y a un dépassement de mémoire (carry), et à 0 quand il n'y en pas.
      sprintf(opcodeSource, "%04u V%d = V%d + V%d\n", memAddress, opcodePart3, opcodePart3, opcodePart2);
      break;

    case 15:

      // 8XY5: VY est soustraite de VX. VF est mis à 0 quand il y a un emprunt, et à 1 quand il n'y a en pas.
      sprintf(opcodeSource, "%04u V%d = V%d - V%d\n", memAddress, opcodePart3, opcodePart3, opcodePart2);
      break;

    case 16:

      // 8XY6: décale VX de 1 bit vers la droite. VF est fixé à la valeur du bit de poids faible de VX avant le décalage.
      sprintf(opcodeSource, "%04u V%d = V%d >> 1\n", memAddress, opcodePart3, opcodePart3);
      break;

    case 17:

      // 8XY7: VX = VY - VX. VF est mis à 0 quand il y a un emprunt et à 1 quand il n'y en a pas.
      sprintf(opcodeSource, "%04u V%d = V%d - V%d\n", memAddress, opcodePart3, opcodePart2, opcodePart3);
      break;

    case 18:

      // 8XYE: décale VX de 1 bit vers la gauche. VF est fixé à la valeur du bit de poids fort de VX avant le décalage.
      sprintf(opcodeSource, "%04u V%d = V%d << 1\n", memAddress, opcodePart3, opcodePart3);
      break;

    case 19:

      // 9XY0: saute l'instruction suivante si VX et VY ne sont pas égaux.
      sprintf(opcodeSource, "%04u IF V%d == V%d\n", memAddress, opcodePart3, opcodePart2);
      break;

    case 20:

      // ANNN: affecte NNN à I.
      sprintf(opcodeSource, "%04u I = %d\n", memAddress, ((opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1));
      break;

    case 21:

      // BNNN: passe à l'adresse NNN + V0.
      sprintf(opcodeSource, "%04u GOTO %d + V0\n", memAddress, ((opcodePart3 << 8) + (opcodePart2 << 4) + opcodePart1));
      break;

    case 22:

      // CXNN: Sets VX to the result of a bitwise and operation on a random byte value (0 to 255) and NN
      sprintf(opcodeSource, "%04u V%d = RANDOM 0 TO %d\n", memAddress, opcodePart3, ((opcodePart2 << 4) + opcodePart1));
      break;

    case 23:

      // DXYN:  dessine un sprite de hauteur N aux coordonnées (VX, VY).
      sprintf(opcodeSource, "%04u SPRITE V%d V%d %d\n", memAddress, opcodePart3, opcodePart2, opcodePart1);
      break;

    case 24:

      // EX9E: saute l'instruction suivante si la clé stockée dans VX est pressée.
      sprintf(opcodeSource, "%04u IF NOT KEY V%d\n", memAddress, opcodePart3);
      break;


    case 25:

      // EXA1: saute l'instruction suivante si la clé stockée dans VX n'est pas pressée.
      sprintf(opcodeSource, "%04u IF KEY V%d\n", memAddress, opcodePart3);
      break;

    case 26:

      // FX07: affecte la valeur de la temporisation à VX
      sprintf(opcodeSource, "%04u V%d = TIMER\n", memAddress, opcodePart3);
      break;

    case 27:

      // FX0A: attend l'appui sur une touche et stocke ensuite sa valeur dans VX.
      sprintf(opcodeSource, "%04u V%d = READ KEY\n", memAddress, opcodePart3);
      break;

    case 28:

      // FX15: définit la temporisation à VX.
      sprintf(opcodeSource, "%04u TIMER = V%d\n", memAddress, opcodePart3);
      break;

    case 29:

      // FX18: définit la minuterie sonore à VX.
      sprintf(opcodeSource, "%04u SOUNDTIMER = V%d\n", memAddress, opcodePart3);
      break;

    case 30:

      // FX1E: ajoute VX à I. VF est mis à 1 quand il y a overflow (I+VX>0xFFF), et à 0 si tel n'est pas le cas.
      sprintf(opcodeSource, "%04u I = I + V%d\n", memAddress, opcodePart3);
      break;

    case 31:

      // FX29: définit I à l'emplacement du caractère stocké dans VX.
      sprintf(opcodeSource, "%04u I = CHAR V%d\n", memAddress, opcodePart3);
      break;

    case 32:

      // FX33: stocke dans la mémoire le code décimal représentant VX (dans I, I+1, I+2).
      sprintf(opcodeSource, "%04u SAVEDEC V%d\n", memAddress, opcodePart3);
      break;

    case 33:

      // FX55: stocke V0 à VX en mémoire à partir de l'adresse I.
      sprintf(opcodeSource, "%04u SAVE V0 TO V%d\n", memAddress, opcodePart3);
      break;

    case 34:

      // FX65: remplit V0 à VX avec les valeurs de la mémoire à partir de l'adresse I.
      sprintf(opcodeSource, "%04u LOAD V0 TO V%d\n", memAddress, opcodePart3);
      break;

      // Display the bytes data
    default:

      sprintf(opcodeSource, "%04u DATA %d %d\n", memAddress, (opcode >> 8), (opcode & 255));
  }
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::updateEditorView() {

  uint8_t i;
  char opcodeSource[32];

  for (i=0; i<editorView.itemsCount; i++) {

    getOpcodeSource(opcodeSource, firstInstructionToShow+i*2);
    strcpy(editorView.items[i].text, opcodeSource);
  }
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
void CHIP8Player::dumpRomSource() {

  uint16_t memAddress;
  char opcodeSource[32];

  for (memAddress=ROM_START; memAddress<ROM_START+romSize; memAddress+=2) {

    getOpcodeSource(opcodeSource, memAddress);
    printf(opcodeSource);
  }
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
uint16_t CHIP8Player::loadRom(char *romPath) {

  //FILE *romFile; //jonne
  uint16_t romFileLength;
  uint16_t opcodesCount;
  uint8_t maxItems;
  uint16_t i;

  //Open rom file
  //romFile = fopen(romPath, "rb");


  /*
  if (!romFile) {

    fprintf(stderr, "Unable to open rom %s.\n", romPath);
    return 0;
  }*/

  //jonne >
  if (fileOpen(romPath,FILE_MODE_BINARY)) {
    return 0;
  }
  // < jonne

  //Get rom file length
  // fseek(romFile, 0, SEEK_END); //jonne
  fileEnd(); //jonne
  //romFileLength = ftell(romFile); // jonne
  romFileLength = fileGetPosition(); // jonne
  // fseek(romFile, 0, SEEK_SET); // jonne
  fileRewind();// jonne
  if (!romFileLength) {

    //fclose(romFile); //jonne
    fileClose(); //jonne
    return 0;
  }

  initEditor();

  //Read rom content into memory
  for (i=0; i<romFileLength; i++) {

    mem[i+ROM_START] = fileGetChar();//jonne fgetc(romFile);
  }
  romSize = romFileLength;

  //fclose(romFile); //jonne
  fileClose(); //jonne

  // Calculate the number of items to add in the editor view
  opcodesCount = romFileLength >> DIVIDE_BY_2;
  if (opcodesCount > EDITOR_HEIGHT) {

    maxItems = 9;
  }
  else {

    maxItems = opcodesCount;
  }

  // Add the items to the editor view
  for(i=0; i<maxItems; i++) {

    editorView.addItem("");
  }

  updateEditorView();

  return romSize;
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
uint8_t CHIP8Player::saveUserRom() {

  FILE *romFile;
  uint16_t i;

  // Open rom file
  romFile = fopen("chip8rom/UserRom.ch8", "wb");
  if (!romFile) {

    fprintf(stderr, "Unable to open user rom.");
    return 0;
  }

  // Write rom content into file
  for (i=ROM_START; i<ROM_START+romSize; i++) {

    fputc(mem[i], romFile);
  }

  // Close rom file
  fclose(romFile);

  return 1;
}

// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
uint16_t CHIP8Player::loadUserRom() {

  //FILE *romFile; //jonne
  uint16_t romFileLength;
  uint16_t opcodesCount;
  uint8_t maxItems;
  uint16_t i;

  //Open rom file
  //jonne romFile = fopen("E:\\Install\\Dev\\Chip8\\Roms\\UserRom.ch8", "rb");

  //jonne if (!romFile) {
  if(fileOpen("chip8rom/UserRom.ch8",FILE_MODE_BINARY)) { //jonne
    // jonne fprintf(stderr, "Unable to open user rom.");
    return 0;
  }

  //Get rom file length
  //jonne fseek(romFile, 0, SEEK_END);
  fileEnd();
  romFileLength = fileGetPosition(); //jonne ftell(romFile);
  //jonne fseek(romFile, 0, SEEK_SET);
  fileRewind(); //jonne
  if (!romFileLength) {

    //jonne fclose(romFile);
    fileClose();//jonne
    return 0;
  }

  initEditor();

  //Read rom content into memory
  for (i=0; i<romFileLength; i++) {

    mem[i+ROM_START] = fileGetChar(); //jonne fgetc(romFile);
  }
  romSize = romFileLength;

  fileClose(); //jonne fclose(romFile);

  // Calculate the number of items to add in the editor view
  opcodesCount = romFileLength >> DIVIDE_BY_2;
  if (opcodesCount > EDITOR_HEIGHT) {

    maxItems = 9;
  }
  else {

    maxItems = opcodesCount;
  }

  // Add the items to the editor view
  for(i=0; i<maxItems; i++) {

    editorView.addItem("");
  }

  updateEditorView();

  return romSize;
}
