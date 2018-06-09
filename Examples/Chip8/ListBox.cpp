//
// ListBox : A ListBox component for the Pokitto (http://www.pokitto.com)
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

#include "ListBox.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ListBox item constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
ListBoxItem::ListBoxItem() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ListBox item destructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
ListBoxItem::~ListBoxItem() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ListBox constructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
ListBox::ListBox() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ListBox destructor
// ------------------------------------------------------------------------------------------------------------------------------------------------------
ListBox::~ListBox() {

  delete[] items;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Init the list box
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ListBox::init(Pokitto::Core *pokittoCore, uint8_t x, uint8_t y , uint8_t width, uint8_t height, uint16_t maxItems, uint8_t options) {

  pokitto = pokittoCore;
  uint8_t i;

  this->x = x;
  this->y = y;

  // Min width of the ListBox must be equal to 4
  if (width < 4) width = 4;
  this->width = width;

  // Min height of the ListBox must be equal to 3
  if (height < 3) height = 3;
  this->height = height;

  // Init colors
  backgroundColor = LB_BACKGROUND_COLOR;
  borderColor = LB_BORDER_COLOR;
  itemColor = LB_ITEM_COLOR;
  selectedItemColor = LB_SELECTED_ITEM_COLOR;

  // Init hideBorder option
  if (options & LB_HIDE_BORDER) {

    hideBorder = true;
    viewX = LB_ITEM_MARGIN_LEFT;
    viewY = 0;
    viewWidth = width;
    viewHeight = height;
  }
  else {

    hideBorder = false;
    viewX = LB_ITEM_MARGIN_LEFT + LB_ITEM_CHAR_WIDTH;
    viewY = LB_ITEM_CHAR_HEIGHT;
    viewWidth = width - 2;
    viewHeight = height - 2;
  }

  firstCharToShow = 0;
  firstItemToShow = 0;

  itemsCount = 0;
  selectedItem = 0;

  // Allocate memory for all the items
  items = new ListBoxItem[maxItems];
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
// Add an item to the list box
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t ListBox::addItem(const char* text) {

  uint8_t textLength = strlen(text);

  if (textLength < LB_ITEM_LENGTH && itemsCount < maxItems) {

    strcpy(items[itemsCount].text, text);
    items[itemsCount].text[textLength] = '\0';

    itemsCount++;

    return itemsCount;
  }
  else return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Remove an item to the list box
// ------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t ListBox::removeItem(uint8_t itemId) {

  uint16_t i;

  if (itemsCount > 0 && itemId < itemsCount) {

    for (i=selectedItem; i<itemsCount-1; i++) {

      items[i] = items[i+1];
    }

    itemsCount--;

    if (selectedItem >= itemsCount && selectedItem > 0) {

      selectedItem = itemsCount - 1;
    }

    if (firstItemToShow > selectedItem) {

      firstItemToShow = selectedItem;
    }
  }

  return itemsCount;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Update the list box
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::update() {

  if (pokitto->buttons.repeat(BTN_UP, 2)) {

    if (selectedItem > 0) {

      selectedItem--;
      if (selectedItem < firstItemToShow) {

        scrollUp();
      }
    }
  }
  else if (pokitto->buttons.repeat(BTN_DOWN, 2)) {

    if (selectedItem < itemsCount-1) {

      selectedItem++;
      if (selectedItem > firstItemToShow+viewHeight-1) {

        scrollDown();
      }
    }
  }
  else if (pokitto->buttons.repeat(BTN_LEFT, 2)) {

    scrollLeft();
  }
  else if (pokitto->buttons.repeat(BTN_RIGHT, 2)) {

    scrollRight();
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Scroll up (move view up)
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::scrollUp() {

  if (firstItemToShow > 0) {

    firstItemToShow--;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Scroll down (move view down)
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::scrollDown() {

  if ((firstItemToShow+viewHeight) < itemsCount) {

    firstItemToShow++;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Scroll left (move view left)
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::scrollLeft() {

  if (firstCharToShow > 0) {

    firstCharToShow--;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Scroll right (move view right)
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::scrollRight() {

  if ((firstCharToShow+viewWidth) < (LB_ITEM_LENGTH-1)) {

    firstCharToShow++;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Show the list box
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::show() {

  uint16_t i;
  char itemText[LB_ITEM_LENGTH];
  uint8_t itemY = y + viewY;
  uint8_t borderX;
  uint8_t borderY;
  uint8_t borderWidth;
  uint8_t borderHeight;
  int16_t lastItemToShow; // Must be signed in order to works with an empty list
  uint8_t oldColor;

  oldColor = pokitto->display.color;

  if (hideBorder) {

    // Draw backround
    pokitto->display.color = backgroundColor;
    pokitto->display.fillRect(x, y, width * LB_ITEM_CHAR_WIDTH + 1, height * LB_ITEM_CHAR_HEIGHT - 3);
  }

  // Draw border if it is enabled
  else {

    borderX = x;
    borderY = LB_BORDER_MARGIN_UP + y;
    borderWidth = width * LB_ITEM_CHAR_WIDTH + 1;
    borderHeight = height * LB_ITEM_CHAR_HEIGHT - 3 - LB_BORDER_MARGIN_DOWN - LB_BORDER_MARGIN_UP;

    // Draw backround
    pokitto->display.color = backgroundColor;
    pokitto->display.fillRect(borderX, borderY, borderWidth, borderHeight);

    pokitto->display.color = borderColor;
    pokitto->display.drawRect(borderX, borderY, borderWidth, borderHeight);
  }

  // Set the last item of the list to show in the view
  lastItemToShow = firstItemToShow + viewHeight - 1;
  if (lastItemToShow >= itemsCount) {

    lastItemToShow = itemsCount - 1;
  }

  // Draw each item in the view
  for (i=firstItemToShow; i<=lastItemToShow; i++) {

    subString(itemText, items[i].text, firstCharToShow, firstCharToShow+viewWidth-1);

    if (i == selectedItem){

      pokitto->display.color = selectedItemColor;
    }
    else {

      pokitto->display.color = itemColor;
    }
    pokitto->display.print(x+viewX, itemY, itemText);

    itemY += LB_ITEM_CHAR_HEIGHT;
  }

  pokitto->display.color = oldColor;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Get a substring from a string
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void ListBox::subString(char *dstString, const char *srcString, uint8_t firstChar, uint8_t lastChar) {

  uint8_t i;
  uint8_t srcStringLength;

  srcStringLength = strlen(srcString);

  if (firstChar > lastChar || firstChar > (srcStringLength - 1)) {

    dstString[0] = '\0';
  }
  else {

    if (lastChar > (srcStringLength - 1)) {

      lastChar = srcStringLength - 1;
    }

    for (i=firstChar; i<=lastChar; i++) {

      dstString[i-firstChar] = srcString[i];
    }
    dstString[i-firstChar] = '\0';
  }
}
