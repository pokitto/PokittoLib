//
// SpriteEditor : A sprite editor component for the Pokitto (http://www.pokitto.com)
//
// Copyright (C) 2017 Jerome Perrot (Initgraph)
//
// Version : 0.1
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

#ifndef SPRITE_EDITOR_H
#define SPRITE_EDITOR_H

#include "pokitto.h"

#define FL_STATE_VIEWMODE 0
#define FL_STATE_EDITMODE 1

#define FL_ITEM_COLOR 1          // White
#define FL_SELECTED_ITEM_COLOR 4 // Yellow

#define FL_ITEM_CHAR_WIDTH 6

#define SE_TITLE_X 18
#define SE_TITLE_Y 2
#define SE_OK_BUTTON_X 97
#define SE_OK_BUTTON_Y 79
#define SE_CANCEL_BUTTON_X 1
#define SE_CANCEL_BUTTON_Y 79

#define SE_GRID_X 6
#define SE_GRID_Y 14
#define SE_GRID_CELL_SIZE 4
#define SE_GRID_WIDTH 8
#define SE_GRID_HEIGHT 15
#define SE_GRID_PIXEL_WIDTH 32  // SE_GRID_CELL_SIZE * SE_GRID_WIDTH
#define SE_GRID_PIXEL_HEIGHT 60 // SE_GRID_CELL_SIZE * SE_GRID_HEIGHT
#define SE_GRID_COLOR 9


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// SpriteEditor Item class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class SpriteEditorItem {

  public:

    uint8_t x;
    uint8_t y;
    uint8_t type;

    SpriteEditorItem();
    ~SpriteEditorItem();

  private:
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// SpriteEditor class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class SpriteEditor {

  public:

    SpriteEditorItem items[150];
    uint8_t itemsCount;
    uint8_t selectedItem;
    uint8_t itemColor;
    uint8_t selectedItemColor;
    uint8_t state;
    bool changeCanceled;

    SpriteEditor();
    ~SpriteEditor();
    uint8_t addItem(uint8_t x, uint8_t y, uint8_t type);
    void update();
    void show();
    uint8_t init(Pokitto::Core *pokittoCore);

  private:

    Pokitto::Core *pokitto;
    uint8_t maxItems;
};

#endif

