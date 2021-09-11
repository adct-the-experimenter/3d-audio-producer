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

//3d Object Timeline

#ifndef GUI_3D_OBJECT_TIMELINE_H
#define GUI_3D_OBJECT_TIMELINE_H

typedef struct 
{
	bool editMode;
	
	bool valueChanged;
	bool addPoint;
	bool showPropertiesBox;
	
	//frame selection variables, 
	size_t current_timeline_frame;
	bool frameSelected;
	float frameDrawX;
	
} TimelineSettings;

typedef struct 
{
	bool editMode;
	
	
	size_t max_num_frames;
	
	size_t* array_points_ptr; //array of points to render
	
	bool valueChanged;
	bool addPoint;
	bool showPropertiesBox;
	
	//frame selection variables, 
	size_t current_timeline_frame;
	bool frameSelected;
	float frameDrawX;
	
} TimelineParameterSettings;



#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

TimelineSettings InitTimelineSettings();
TimelineParameterSettings InitTimelineParameterSettings(size_t max_num_frames, size_t* points_ptr);

//Draws timeline 
//returns if timeline was clicked on
bool Gui_Timeline(TimelineSettings* settings);

bool Gui_Timeline_Parameter(TimelineParameterSettings* settings);

#ifdef __cplusplus
}
#endif

#endif

#if defined(GUI_3D_OBJECT_TIMELINE)

TimelineSettings InitTimelineSettings()
{
	TimelineSettings settings = {0};
}

TimelineParameterSettings InitTimelineParameterSettings(size_t max_num_frames, size_t* points_ptr)
{
	TimelineParameterSettings settings = { 0 };
	
	settings.array_points_ptr = points_ptr;
	settings.max_num_frames = max_num_frames;
	settings.frameSelected = false;
}

bool Gui_Timeline(TimelineSettings* settings)
{
	//draw timeline
	//horizontal axis is the time
	//vertical axis is nothing

	//draw box with background color
	float screen_height = GetScreenHeight();
	Rectangle drawAreaRect = {0, 400 , GetScreenWidth(), screen_height};

	DrawRectangleRec(drawAreaRect, Fade(GRAY, 0.8f));

	float leftBound = 200;
	Rectangle mouseArea = {leftBound, 400 , GetScreenWidth(), screen_height};

	//draw cursor line where mouse pointer is

	Vector2 mousePoint = GetMousePosition();

	//select frame based on where mouse pointer is clicked on 
	if (CheckCollisionPointRec(mousePoint, mouseArea))
	{
		float cursor_x = mousePoint.x;
		if(cursor_x < leftBound){cursor_x = leftBound;}

		Color cursor_color = BLACK;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			settings->frameSelected = !settings->frameSelected;
			
			if(settings->frameSelected)
			{
				settings->frameDrawX = cursor_x;
			}
		}
		
		if(settings->frameSelected)
		{
			cursor_x = settings->frameDrawX;
			cursor_color = YELLOW;
		}
		
		DrawRectangle(cursor_x,400,2,screen_height,cursor_color);
	}
	else
	{
		if(settings->frameSelected)
		{
			float cursor_x = settings->frameDrawX;
			Color cursor_color = YELLOW;
			DrawRectangle(cursor_x,400,2,screen_height,cursor_color);
		}
		
		
	}
}

bool Gui_Timeline_Parameter(TimelineParameterSettings* settings)
{


//draw points

//if mouse click on point in timeline and add point is false
	//show properties box to true
		
//draw properties box
	
//if show properties box bool is true
	
	//show the contents of the point in timeline such as position and rotation.
	
}

#endif
