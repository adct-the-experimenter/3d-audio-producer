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


#ifndef GUI_VALIDFLOAT_H
#define GUI_VALIDFLOAT_H

typedef struct 
{
	bool editMode;
	float current_value;
	float default_value;
	float min;
	float max;
	char textBuffer[9];
	bool valueChanged;
} ValidFloatParamSettings;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

bool GuiTextBox_ValidValueFloat(Rectangle rect, int textSize, bool editMode, 
								float* current_value, float default_value, float min, float max,
								char* textBuffer,bool* valueChanged, const char* label, int label_width);

ValidFloatParamSettings InitValidFloatParamSettings(float current_value, float default_value, float min, float max, const char* initialText);

bool GuiTextBox_ValidValueFloatSimple(Rectangle rect, int textSize, ValidFloatParamSettings* param, const char* label, int label_width);

#ifdef __cplusplus
}
#endif




#endif

#if defined(GUI_VALIDFLOATBOX)


//text box that only accepts number

bool GuiTextBox_ValidValueFloat(Rectangle rect, int textSize, bool editMode, 
								float* current_value, float default_value, float min, float max,
								char* textBuffer,bool* valueChanged, const char* label, int label_width)
{
	
	
	//draw Gui Text boxRectangle bounds, char *text, int textSize, bool editMode
	bool active = GuiTextBox(rect, textBuffer, textSize, editMode);
	DrawText(label, rect.x - label_width, rect.y + rect.height/2, 10, BLACK);
	
	if(editMode)
	{
		//do writing here
		*valueChanged = true;
	}
	else
	{	
		//if string value in text box changed
		if(*valueChanged)
		{
			//check the result of atof
			double atof_result = atof(textBuffer);
			
			//if result is zero, then set it to default value
			if(atof_result == 0.0){ *current_value = default_value;}
			//else check if value is within bounds, set to default value if not
			else
			{
				if((atof_result >= min) && (atof_result <= max)){ *current_value = atof_result;}
				else{*current_value = default_value;}
			}
			
			//set text to current value
			sprintf(textBuffer, "%f", *current_value);
			textBuffer[8] = '\0';
		}
		
		*valueChanged = false;
	}
	
	
	
	return active;	
}

ValidFloatParamSettings InitValidFloatParamSettings(float current_value, float default_value, float min, float max, const char* initialText)
{
	ValidFloatParamSettings settings = { 0 };
	
	settings.editMode = false;
	settings.current_value = current_value;
	settings.default_value = default_value;
	settings.min = min;
	settings.max = max;
	
	strncpy(settings.textBuffer, initialText, 9);
	settings.textBuffer[8] = '\0';
	settings.valueChanged = false;
	
	return settings;
}

bool GuiTextBox_ValidValueFloatSimple(Rectangle rect, int textSize, ValidFloatParamSettings* param, const char* label, int label_width)
{
	//draw Gui Text boxRectangle bounds, char *text, int textSize, bool editMode
	bool active = GuiTextBox(rect, param->textBuffer, textSize, param->editMode);
	DrawText(label, rect.x - label_width, rect.y + rect.height/2, 10, BLACK);
	
	if(param->editMode)
	{
		//do writing here
		param->valueChanged = true;
	}
	else
	{	
		//if string value in text box changed
		if(param->valueChanged)
		{
			//check the result of atof
			double atof_result = atof(param->textBuffer);
			
			//if result is zero, then set it to default value
			if(atof_result == 0.0){ param->current_value = param->default_value;}
			//else check if value is within bounds, set to default value if not
			else
			{
				if((atof_result >= param->min) && (atof_result <= param->max)){ param->current_value = atof_result;}
				else{param->current_value = param->default_value;}
			}
			
			//set text to current value
			sprintf(param->textBuffer, "%f", param->current_value);
			param->textBuffer[8] = '\0';
		}
		
		param->valueChanged = false;
	}
	
	return active;
}

#endif

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


//3d Object Timeline

#ifndef GUI_3D_OBJECT_TIMELINE_H
#define GUI_3D_OBJECT_TIMELINE_H


typedef struct 
{
	bool editMode;
	float current_timeline_position;
	float max_num_frames;
	bool valueChanged;
	bool addPoint;
	bool showPropertiesBox;
} TimelineSettings;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//Draws timeline 
//returns if timeline was clicked on
bool Gui_3dObject_Timeline(TimelineSettings* settings);

#ifdef __cplusplus
}
#endif

#endif

#if defined(GUI_3D_OBJECT_TIMELINE)

bool Gui_3dObject_Timeline(TimelineSettings* settings)
{
	
//draw timeline

//if show timeline is true

	//draw box with background color
	
	//draw cursor line where mouse pointer is
	
	//if add point is true
		
		//add point at current cursor line
		
	
	//if mouse click on point in timeline and add point is false
		//show properties box to true
		
//draw properties box
	
//if show properties box bool is true
	
	//show the contents of the point in timeline such as position and rotation.
	
}

#endif
