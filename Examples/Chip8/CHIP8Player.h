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

#ifndef CHIP8_PLAYER_H
#define CHIP8_PLAYER_H

#include "pokitto.h"
#include "ListBox.h"
#include "FieldsList.h"

#define DIVIDE_BY_2 1
#define DIVIDE_BY_8 3
#define MULTIPLY_BY_2 1
#define MULTIPLY_BY_4 2

#define MEM_SIZE 4096
#define ROM_START 512
#define MAX_ROM_SIZE 3584 // MEM_SIZE - START_ADDRESS
#define OPCODES_COUNT 35
#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32
#define POKITTO_SCREEN_WIDTH 110
#define POKITTO_SCREEN_HEIGHT 88
#define SCREEN_LENGTH 4840 // POKITTO_SCREEN_WIDTH * POKITTO_SCREEN_HEIGHT

#define STATE_MAIN_MENU 0
#define STATE_SETTINGS 1
#define STATE_EDITOR 2
#define STATE_GAME 3
#define STATE_CONTEXT_MENU 4
#define STATE_ADD_MENU 5

#define STATE_VX_EQUALS_MENU 6
#define STATE_VX_EQUALS_N_MENU 7
#define STATE_VX_EQUALS_VY_MENU 8
#define STATE_VX_PLUS_N_MENU 9
#define STATE_VX_PLUS_VY_MENU 10
#define STATE_VX_MINUS_VY_MENU 11
#define STATE_VY_MINUS_VX_MENU 12
#define STATE_VX_AND_VY_MENU 13
#define STATE_VX_OR_VY_MENU 14
#define STATE_VX_XOR_VY_MENU 15
#define STATE_VX_LEFTSHIFT_MENU 16
#define STATE_VX_RIGHTSHIFT_MENU 17
#define STATE_VX_RANDOM_MENU 18
#define STATE_VX_TIMER_MENU 19
#define STATE_VX_READKEY_MENU 20

#define STATE_I_EQUALS_MENU 21
#define STATE_I_EQUALS_N_MENU 22
#define STATE_I_EQUALS_CHAR_VX_MENU 23
#define STATE_I_PLUS_VX_MENU 24

#define STATE_SPRITE_MENU 25

#define STATE_IF_VX_MENU 26
#define STATE_IF_VX_EQUALS_N_MENU 27
#define STATE_IF_VX_EQUALS_VY_MENU 28
#define STATE_IF_VX_NOT_EQUALS_N_MENU 29
#define STATE_IF_VX_NOT_EQUALS_VY_MENU 30

#define STATE_GOTO_N_MENU 31
#define STATE_CALL_N_MENU 32
#define STATE_IF_KEY_VX_MENU 33
#define STATE_IF_NOT_KEY_VX_MENU 34
#define STATE_TIMER_MENU 35
#define STATE_DATA_MENU 36
#define STATE_SAVEDEC_VX_MENU 37
#define STATE_SOUNDTIMER_MENU 38
#define STATE_SAVE_V0_TO_VX_MENU 39
#define STATE_LOAD_V0_TO_VX_MENU 40
#define STATE_GOTO_N_PLUS_V0_MENU 41

#define EDITOR_X 0
#define EDITOR_Y 0
#define EDITOR_WIDTH 19
#define EDITOR_HEIGHT 9
#define FIRST_EDITOR_LINE 0
#define LAST_EDITOR_LINE 8

#define MAIN_MENU_X 30
#define MAIN_MENU_Y 45
#define MAIN_MENU_WIDTH 12
#define MAIN_MENU_HEIGHT 4
#define MAIN_MENU_TITLE_X 24
#define MAIN_MENU_TITLE_Y 17
#define MAIN_MENU_FRAME_X 5
#define MAIN_MENU_FRAME_Y 5
#define MAIN_MENU_FRAME_WIDTH 100
#define MAIN_MENU_FRAME_HEIGHT 30
#define CONTEXT_MENU_X 6
#define CONTEXT_MENU_Y 5
#define CONTEXT_MENU_WIDTH 16
#define CONTEXT_MENU_HEIGHT 8
#define MENU_X 3
#define MENU_Y 10
#define MENU_WIDTH 17
#define MENU_HEIGHT 8
#define TITLE_Y 2
#define FRAME_X 3
#define FRAME_Y 12
#define FRAME_WIDTH 103
#define FRAME_HEIGHT 63
#define PARAM_X 12
#define VALUE_X 30
#define PARAM_Y_1_1 40
#define PARAM_Y_1_2 35
#define PARAM_Y_2_2 45
#define PARAM_Y_1_3 30
#define PARAM_Y_2_3 40
#define PARAM_Y_3_3 50
#define OK_BUTTON_X 97
#define OK_BUTTON_Y 79
#define CANCEL_BUTTON_X 1
#define CANCEL_BUTTON_Y 79


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Editor class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class CHIP8Player {

  public:

    uint8_t mem[MEM_SIZE];
    uint16_t PC;
    uint16_t I;
    uint8_t V[16];
    uint16_t stack[16];
    uint8_t stackLength;
    uint16_t delayTimer;
    uint8_t soundTimer;
    uint16_t opcodesIds[OPCODES_COUNT];
    uint16_t opcodesMasks[OPCODES_COUNT];
    uint8_t opcodeIndex;
    uint8_t keys[16];
    bool keyPressed;
    uint16_t romSize;
    uint8_t speed;
    uint8_t screen[SCREEN_LENGTH];
    uint8_t screenWidth;
    uint8_t screenHeight;

    ListBox mainMenu;
    ListBox settingsMenu;
    ListBox editorView;
    ListBox contextMenu;
    ListBox addMenu;

    ListBox vxEqualsMenu;
    FieldsList vxEqualsNMenu;
    FieldsList vxEqualsVyMenu;
    FieldsList vxPlusNMenu;
    FieldsList vxPlusVyMenu;
    FieldsList vxMinusVyMenu;
    FieldsList vyMinusVxMenu;
    FieldsList vxAndVyMenu;
    FieldsList vxOrVyMenu;
    FieldsList vxXorVyMenu;
    FieldsList vxLeftShiftMenu;
    FieldsList vxRightShiftMenu;
    FieldsList vxRandomMenu;
    FieldsList vxTimerMenu;
    FieldsList vxReadKeyMenu;

    ListBox iEqualsMenu;
    FieldsList iEqualsNMenu;
    FieldsList iEqualsCharVXMenu;
    FieldsList iPlusVxMenu;

    FieldsList spriteMenu;

    ListBox ifVxMenu;
    FieldsList ifVxEqualsNMenu;
    FieldsList ifVxEqualsVyMenu;
    FieldsList ifVxNotEqualsNMenu;
    FieldsList ifVxNotEqualsVyMenu;

    FieldsList gotoNMenu;
    FieldsList callNMenu;
    FieldsList ifKeyVxMenu;
    FieldsList ifNotKeyVxMenu;
    FieldsList timerMenu;
    FieldsList dataMenu;
    FieldsList saveDecVxMenu;
    FieldsList soundTimerMenu;
    FieldsList saveV0ToVx;
    FieldsList loadV0ToVx;
    FieldsList gotoNPlusV0Menu;

    uint8_t state;
    uint16_t firstInstructionToShow;

    CHIP8Player();
    void init(Pokitto::Core *pokittoCore);
    void initEditor();
    void initPlayer();
    void update();
    void readOpcode();

  private:

    Pokitto::Core *pokitto;
    bool screenIsInEditMode;

    void setPixel(uint8_t x, uint8_t y, uint8_t color);
    uint8_t getPixel(uint8_t x, uint8_t y);
    void clearScreen();
    void drawScreen();
    void drawSprite(uint8_t x, uint8_t y, uint8_t height);
    void readKeys();
    uint8_t getOpcodeIndex(uint16_t opcode);
    uint16_t getOpcode(uint16_t memAddress);
    void getOpcodeSource(char *opcodeSource, uint16_t memAddress);
    void addOpcode(uint16_t opcode);
    void deleteOpcode();
    void updateOpcode(uint16_t opcode);
    void moveOpcodeUp();
    void moveOpcodeDown();
    void dumpRomSource();
    uint16_t loadRom(char *romPath);
    uint16_t loadUserRom();
    uint8_t saveUserRom();
    void updateEditorView();
};

#endif
