//
// FieldsList : A FieldsList component for the Pokitto (http://www.pokitto.com)
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

#ifndef FIELDSSET_H
#define FIELDSSET_H

#include "Pokitto.h"

#define FL_STATE_VIEWMODE 0
#define FL_STATE_EDITMODE 1

#define FL_ITEM_COLOR 1          // White
#define FL_SELECTED_ITEM_COLOR 4 // Yellow
#define FL_ITEM_LENGTH 20        // Max characters in a line (19 characters + '\0')
#define FL_MAX_ITEM_DIGITS 5
#define FL_ITEM_CHAR_WIDTH 6

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList Item class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class FieldsListItem {

  public:

    uint8_t x;
    uint8_t y;
    uint16_t minValue;
    uint16_t maxValue;
    uint16_t value;
    char text[FL_ITEM_LENGTH];
    bool isAnEditableValue;

    FieldsListItem();
    ~FieldsListItem();

  private:
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList class
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class FieldsList {

  public:

    FieldsListItem *items;
    uint8_t itemsCount;
    uint8_t selectedItem;
    uint8_t itemColor;
    uint8_t selectedItemColor;
    uint8_t itemDigits[FL_MAX_ITEM_DIGITS];
    uint8_t firstDigit;
    uint8_t selectedDigit;
    uint8_t state;
    bool changeCanceled;

    FieldsList();
    ~FieldsList();
    uint8_t addItem(uint8_t x, uint8_t y, const char* text);
    uint8_t addItem(uint8_t x, uint8_t y, uint16_t value, uint16_t minValue, uint16_t maxValue);
    void setValue(uint8_t itemId, uint16_t value);
    void update();
    void show();
    uint8_t init(Pokitto::Core *pokittoCore, uint8_t maxItems);
    //char* getSubString(const char *string, uint8_t start, uint8_t end);

  private:

    Pokitto::Core *pokitto;
    uint8_t maxItems;
};

#endif

