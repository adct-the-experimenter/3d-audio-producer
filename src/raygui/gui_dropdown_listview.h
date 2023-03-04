/*******************************************************************************************
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2021 Pablo Camacho
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/



#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui/raygui.h"

#include <stdlib.h>
#include <string.h>

//GuiListView with Dropdown box bound change

#ifndef GUI_DROPDOWN_LISTVIEW_H
#define GUI_DROPDOWN_LISTVIEW_H

typedef struct 
{
	int itemsShowCount;
	bool editMode;
	bool valueChanged;
	int scrollIndex;
} DropDownListViewSettings;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex);

//function to use guilistview when drop down box is selected.
int Gui_Dropdown_ListView(Rectangle bounds, int itemsShowCount, int itemsCount, const char *text, int *scrollIndex, int active, bool* editMode, bool* valueChanged);

//function to use guilistview when drop down box is selected.
int Gui_Dropdown_ListView_Simple(DropDownListViewSettings* settings,Rectangle bounds, const char *text, int itemsCount, int active);

#ifdef __cplusplus
}
#endif


#endif

#if defined(GUI_DROPDOWN_LISTVIEW)


int Gui_Dropdown_ListView(Rectangle bounds, int itemsShowCount, int itemsCount, const char *text, int *scrollIndex, int active, bool* editMode, bool* valueChanged)
{
	
	Rectangle boundsOpen = bounds;
    boundsOpen.height = (itemsShowCount + 1)*(bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));
		
	Rectangle& current_bounds = bounds;
		
	// Update control
    //--------------------------------------------------------------------
    
	Vector2 mousePoint = GetMousePosition();

	if (*editMode)
	{
		// Check if mouse has been pressed or released outside limits
		if (!CheckCollisionPointRec(mousePoint, boundsOpen))
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
			{
				*editMode = false;
				*scrollIndex = active;
			} 
		}
		
	}
	else
	{
		//check if mouse clicked on drop down box
		if (CheckCollisionPointRec(mousePoint, bounds))
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				*editMode = true;
			}
		}
	}

	if (*editMode){current_bounds = boundsOpen;}
	
	int itemSelected = -1;
	
	
	itemSelected = GuiListView( current_bounds, text, scrollIndex, active );
		
	
	if(itemSelected != -1 && itemSelected < itemsCount && itemSelected != active)
	{
		*valueChanged = true;
		*editMode = false;
		*scrollIndex = itemSelected;
	}
	else
	{
		itemSelected = active;
		*valueChanged = false;
	}
	
	return itemSelected;
	
}

DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
{
	DropDownListViewSettings settings = { 0 };
	
	settings.itemsShowCount = itemsShowCount;
	settings.editMode = false;
	settings.valueChanged = false;
	settings.scrollIndex = 0;
	
	return settings;
}

int Gui_Dropdown_ListView_Simple(DropDownListViewSettings* settings,Rectangle bounds, const char *text, int itemsCount, int active)
{
	return Gui_Dropdown_ListView(bounds, settings->itemsShowCount, itemsCount, text, &settings->scrollIndex, active, &settings->editMode, &settings->valueChanged);
}


#endif

