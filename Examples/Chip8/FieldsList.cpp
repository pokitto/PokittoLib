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

#include "FieldsList.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList item constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
FieldsListItem::FieldsListItem() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList item destructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
FieldsListItem::~FieldsListItem() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
FieldsList::FieldsList() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// FieldsList destructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
FieldsList::~FieldsList() {

  delete[] items;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Init the fields list
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t FieldsList::init(Pokitto::Core *pokittoCore, uint8_t maxItems) {

  pokitto = pokittoCore;

  if (maxItems > 254) return 0;

  // Init colors
  itemColor = FL_ITEM_COLOR;
  selectedItemColor = FL_SELECTED_ITEM_COLOR;

  itemsCount = 0;
  selectedItem = 0;
  firstDigit = 0;
  selectedDigit = 0;
  state = FL_STATE_VIEWMODE;
  changeCanceled = false;

  // Allocate memory for all the items
  items = new FieldsListItem[maxItems];
  if (items == NULL) {

    this->maxItems = 0;
    return 0;
  }
  else {

    this->maxItems = maxItems;
    return maxItems;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Add an item to the fields list
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t FieldsList::addItem(uint8_t x, uint8_t y, const char* text) {

  uint8_t textLength = strlen(text);

  if (textLength < FL_ITEM_LENGTH && itemsCount < maxItems) {

    items[itemsCount].x = x;
    items[itemsCount].y = y;
    items[itemsCount].value = 0;
    items[itemsCount].minValue = 0;
    items[itemsCount].maxValue = 0;
    items[itemsCount].isAnEditableValue = false;

    strcpy(items[itemsCount].text, text);
    items[itemsCount].text[textLength] = '\0';

    itemsCount++;

    return itemsCount;
  }
  else return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Add an editable value item to the fields list
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t FieldsList::addItem(uint8_t x, uint8_t y, uint16_t value, uint16_t minValue, uint16_t maxValue) {

  if (minValue <= maxValue && value >= minValue && value <= maxValue) {

    items[itemsCount].x = x;
    items[itemsCount].y = y;
    items[itemsCount].value = value;
    items[itemsCount].minValue = minValue;
    items[itemsCount].maxValue = maxValue;
    items[itemsCount].isAnEditableValue = true;

    sprintf(items[itemsCount].text, "%u", maxValue);

    switch (strlen(items[itemsCount].text)) {

      case 1:
        sprintf(items[itemsCount].text, "%01u", value);
        break;

      case 2:
        sprintf(items[itemsCount].text, "%02u", value);
        break;

      case 3:
        sprintf(items[itemsCount].text, "%03u", value);
        break;

      case 4:
        sprintf(items[itemsCount].text, "%04u", value);
        break;

      default:
        sprintf(items[itemsCount].text, "%05u", value);
    }

    itemsCount++;

    return itemsCount;
  }
  else return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Set an item  value
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void FieldsList::setValue(uint8_t itemId, uint16_t value) {

  // Update the value of the item
  if (value < items[itemId].minValue) {

    value = items[itemId].minValue;
  }
  else if (value > items[itemId].maxValue) {

    value = items[itemId].maxValue;
  }
  items[itemId].value = value;

  // Update item text with the new value
  switch (strlen(items[itemId].text)) {

    case 1:
      sprintf(items[itemId].text, "%01u", value);
      break;

    case 2:
      sprintf(items[itemId].text, "%02u", value);
      break;

    case 3:
      sprintf(items[itemId].text, "%03u", value);
      break;

    case 4:
      sprintf(items[itemId].text, "%04u", value);
      break;

    default:
      sprintf(items[itemId].text, "%05u", value);
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Update the fields list
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void FieldsList::update() {

  uint8_t i;
  uint8_t itemsDeltaX;
  uint8_t itemsDeltaY;
  uint8_t lastItemFound;
  uint8_t lastItemFoundDeltaX;
  uint8_t lastItemFoundDeltaY;
  //uint8_t valueLength;
  uint16_t newValue;


  changeCanceled = false;

  // View mode ------------------------------------------------------------------------------------------------------
  if (state == FL_STATE_VIEWMODE) {

    // Up Button --------------------------------------------------------------------------------
    if (pokitto->buttons.repeat(BTN_UP, 1)) {

      itemsDeltaX = 255;
      itemsDeltaY = 255;
      lastItemFound = 255;
      lastItemFoundDeltaX = 255;
      lastItemFoundDeltaY = 255;

      for(i=0; i<itemsCount; i++) {

        // Select items located above the selected item only
        if (items[i].y < items[selectedItem].y) {

          itemsDeltaX = abs(items[i].x - items[selectedItem].x);
          itemsDeltaY = items[selectedItem].y - items[i].y;

          if (itemsDeltaY == lastItemFoundDeltaY) {

            if (itemsDeltaX < lastItemFoundDeltaX) {

              lastItemFound = i;
              lastItemFoundDeltaX = itemsDeltaX;
              lastItemFoundDeltaY = itemsDeltaY;
            }
          }
          else if (itemsDeltaY < lastItemFoundDeltaY) {

            lastItemFound = i;
            lastItemFoundDeltaX = itemsDeltaX;
            lastItemFoundDeltaY = itemsDeltaY;
          }
        }
      }

      // If an item was found
      if (lastItemFound < 255) {

        selectedItem = lastItemFound;
      }
    }

    // Down Button ------------------------------------------------------------------------------
    else if (pokitto->buttons.repeat(BTN_DOWN, 1)) {

      itemsDeltaX = 255;
      itemsDeltaY = 255;
      lastItemFound = 255;
      lastItemFoundDeltaX = 255;
      lastItemFoundDeltaY = 255;

      for(i=0; i<itemsCount; i++) {

        // Select items located below the selected item only
        if (items[i].y > items[selectedItem].y) {

          itemsDeltaX = abs(items[i].x - items[selectedItem].x);
          itemsDeltaY = items[i].y - items[selectedItem].y;

          if (itemsDeltaY == lastItemFoundDeltaY) {

            if (itemsDeltaX < lastItemFoundDeltaX) {

              lastItemFound = i;
              lastItemFoundDeltaX = itemsDeltaX;
              lastItemFoundDeltaY = itemsDeltaY;
            }
          }
          else if (itemsDeltaY < lastItemFoundDeltaY) {

            lastItemFound = i;
            lastItemFoundDeltaX = itemsDeltaX;
            lastItemFoundDeltaY = itemsDeltaY;
          }
        }
      }

      // If an item was found
      if (lastItemFound < 255) {

        selectedItem = lastItemFound;
      }
    }

    // Left Button ------------------------------------------------------------------------------
    else if (pokitto->buttons.repeat(BTN_LEFT, 1)) {

      lastItemFound = 255;
      lastItemFoundDeltaX = 255;

      for(i=0; i<itemsCount; i++) {

        // Select items located on the left and on the same line of the selected item only
        if ((items[i].x < items[selectedItem].x) && (items[i].y == items[selectedItem].y)) {

          itemsDeltaX = items[selectedItem].x - items[i].x;

          if (itemsDeltaX < lastItemFoundDeltaX) {

            lastItemFound = i;
            lastItemFoundDeltaX = itemsDeltaX;
          }
        }
      }

      // If an item was found
      if (lastItemFound < 255) {

        selectedItem = lastItemFound;
      }
    }

    // Right Button -----------------------------------------------------------------------------
    else if (pokitto->buttons.repeat(BTN_RIGHT, 1)) {

      lastItemFound = 255;
      lastItemFoundDeltaX = 255;

      for(i=0; i<itemsCount; i++) {

        // Select items located on the right and on the same line of the selected item only
        if ((items[i].x > items[selectedItem].x) && (items[i].y == items[selectedItem].y)) {

          itemsDeltaX = items[i].x - items[selectedItem].x;

          if (itemsDeltaX < lastItemFoundDeltaX) {

            lastItemFound = i;
            lastItemFoundDeltaX = itemsDeltaX;
          }
        }
      }

      // If an item was found
      if (lastItemFound < 255) {

        selectedItem = lastItemFound;
      }
    }
    // A Button ---------------------------------------------------------------------------------
    else if (pokitto->buttons.pressed(BTN_A) && items[selectedItem].isAnEditableValue) {

      // Init the digits of the value to edit
      firstDigit = FL_MAX_ITEM_DIGITS - strlen(items[selectedItem].text);
      selectedDigit = firstDigit;

      itemDigits[0] = (uint8_t) (items[selectedItem].value / 10000);
      itemDigits[1] = (uint8_t) ((items[selectedItem].value % 10000) / 1000);
      itemDigits[2] = (uint8_t) ((items[selectedItem].value % 1000) / 100);
      itemDigits[3] = (uint8_t) ((items[selectedItem].value % 100) / 10);
      itemDigits[4] = (uint8_t) ((items[selectedItem].value % 100) % 10);

      // Switch to edit mode in order to edit the value
      state = FL_STATE_EDITMODE;
    }
  }

  // Edit mode ------------------------------------------------------------------------------------------------------
  else {

    // Up Button --------------------------------------------------------------------------------
    if (pokitto->buttons.repeat(BTN_UP, 2)) {

      itemDigits[selectedDigit]++;
      itemDigits[selectedDigit] %= 10;
    }

    // Down Button ------------------------------------------------------------------------------
    else if (pokitto->buttons.repeat(BTN_DOWN, 2)) {

      itemDigits[selectedDigit]--;
      if (itemDigits[selectedDigit] == 255) {

        itemDigits[selectedDigit] = 9;
      }
    }

    // Left Button ------------------------------------------------------------------------------
    else if (pokitto->buttons.pressed(BTN_LEFT)) {

      selectedDigit --;
      if (selectedDigit < firstDigit || selectedDigit == 255) {

        selectedDigit = FL_MAX_ITEM_DIGITS - 1;
      }
    }

    // Right Button -----------------------------------------------------------------------------
    else if (pokitto->buttons.pressed(BTN_RIGHT)) {

      selectedDigit++;
      if (selectedDigit == FL_MAX_ITEM_DIGITS) {

        selectedDigit = firstDigit;
      }
    }

    // A Button ---------------------------------------------------------------------------------
    else if (pokitto->buttons.pressed(BTN_A)) {

      // Save the new value of the item
      newValue = itemDigits[0] * 10000 + itemDigits[1] * 1000 + itemDigits[2] * 100 + itemDigits[3] * 10 + itemDigits[4];
      setValue(selectedItem, newValue);

      // Switch to view mode
      state = FL_STATE_VIEWMODE;
    }

    // B Button ---------------------------------------------------------------------------------
    else if (pokitto->buttons.pressed(BTN_B)) {

      changeCanceled = true;

      // Cancels the modification by going back to view mode
      state = FL_STATE_VIEWMODE;
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Show the fields list
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void FieldsList::show() {

  uint8_t item;
  uint8_t digit;
  uint8_t digitX;
  char digitText[2];
  uint8_t oldColor;

  oldColor = pokitto->display.color;

  // Draw each item on the screen
  for(item=0; item<itemsCount; item++) {

    if (item == selectedItem){

      if (state == FL_STATE_EDITMODE) {

        digitX = 0;
        for (digit=firstDigit; digit<FL_MAX_ITEM_DIGITS; digit++) {

          if (digit == selectedDigit) {

            pokitto->display.color = selectedItemColor;
          }
          else {

            pokitto->display.color = itemColor;
          }

          sprintf(digitText, "%d", itemDigits[digit]);
          pokitto->display.print(items[item].x + digitX, items[item].y, digitText);

          digitX += FL_ITEM_CHAR_WIDTH;
        }
      }

      else {

        pokitto->display.color = selectedItemColor;
        pokitto->display.print(items[item].x, items[item].y, items[item].text);
      }
    }

    else {

      pokitto->display.color = itemColor;
      pokitto->display.print(items[item].x, items[item].y, items[item].text);
    }
  }

  pokitto->display.color = oldColor;
}
