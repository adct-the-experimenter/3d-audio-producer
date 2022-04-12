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

//for drawing general timeline ticks and frame line
typedef struct 
{
	bool editMode;
	
	bool valueChanged;
	bool addPoint;
	bool showPropertiesBox;
	
	int scroll_value; //sets leftmost and rightmost frame
	int max_scroll_value; //max value for scrolling
	int* leftmost_frame_ptr; //frame that is on the left, use with gui scroll panel
	int* rightmost_frame_ptr; //frame that is on the right, use with gui scroll panel
	int max_num_frames_displayed; //max number of frames to be displayed on screen
	int frame_scroll_value; //value of frame that scroll adds
	int frame_scroll_rate; //rate at which 1 scroll value increases frame_scroll_value
	
	//frame selection variables, 
	size_t current_timeline_frame;
	bool frameSelected;
	float frameDrawX;
	
	//area that mouse can click to add timeline points
	Rectangle mouseArea;
	bool mouse_control;
	
	size_t max_num_frames;
	
	
} TimelineSettings;

//for drawing timeline points
typedef struct 
{
		
	size_t max_num_frames;
	
	int* leftmost_frame_ptr; //frame that is on the left, use with gui scroll panel
	int* rightmost_frame_ptr; //frame that is on the right, use with gui scroll panel
	int max_num_frames_displayed; //max number of frames to be displayed on screen
	
	bool* array_points_ptr; //array of points to render
	
	bool showPropertiesBox;
	
	//where to draw points
	int draw_start_x;
	int draw_y;
	
	Color draw_color;
	
} TimelineParameterSettings;



#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

TimelineSettings InitTimelineSettings(size_t max_num_frames);
TimelineParameterSettings InitTimelineParameterSettings(size_t max_num_frames, bool* points_ptr, int x, int y,
														int* left_frame_ptr, int* right_frame_ptr, int max_num_frames_to_display);

//Draws timeline 
//returns if timeline was clicked on
bool Gui_Timeline(TimelineSettings* settings);

bool Gui_Timeline_Parameter(TimelineParameterSettings* settings);


#ifdef __cplusplus
}
#endif

#endif

#if defined(GUI_3D_OBJECT_TIMELINE)

TimelineSettings InitTimelineSettings(size_t max_num_frames, int* left_frame_ptr, int* right_frame_ptr,int max_num_frames_to_display)
{
	TimelineSettings settings = {0};
	
	settings.editMode = true;
	settings.current_timeline_frame = 0;
	settings.max_num_frames = max_num_frames;
	settings.mouse_control = true;
	
	settings.max_num_frames_displayed = max_num_frames_to_display;
	
	settings.leftmost_frame_ptr = left_frame_ptr;
	settings.rightmost_frame_ptr = right_frame_ptr;
	settings.scroll_value = 0;
	settings.frame_scroll_value = 0;
	settings.frame_scroll_rate = 20;
	settings.max_scroll_value = (max_num_frames / settings.frame_scroll_rate) - 1;
	
	return settings;
}

TimelineParameterSettings InitTimelineParameterSettings(size_t max_num_frames, bool* points_ptr, int x, int y,
														int* left_frame_ptr, int* right_frame_ptr, int max_num_frames_to_display)
{
	TimelineParameterSettings settings = { 0 };
	
	settings.array_points_ptr = points_ptr;
	settings.max_num_frames = max_num_frames;
	
	settings.draw_start_x = x;
	settings.draw_y = y;
	
	settings.draw_color = BLACK;
	
	settings.leftmost_frame_ptr = left_frame_ptr;
	settings.rightmost_frame_ptr = right_frame_ptr;
	
	settings.max_num_frames_displayed = max_num_frames_to_display;
	
	return settings;
}

//only show every 20th frame
static int show_frame_modulo_factor = 20;

bool Gui_Timeline(TimelineSettings* settings)
{
	//draw timeline
	//horizontal axis is the time
	//vertical axis is nothing

	float screen_height = GetScreenHeight();
	
	settings->scroll_value = GuiScrollBar((Rectangle){settings->mouseArea.x,settings->mouseArea.y - 30,settings->mouseArea.width,30}, settings->scroll_value, 0, settings->max_scroll_value);
	settings->frame_scroll_value = settings->scroll_value * settings->frame_scroll_rate;
	
	if(settings->frame_scroll_value > settings->max_num_frames - settings->max_num_frames_displayed)
	{
		settings->frame_scroll_value = settings->max_num_frames - settings->max_num_frames_displayed;
	}
	
	*settings->leftmost_frame_ptr = settings->frame_scroll_value;
	*settings->rightmost_frame_ptr = settings->frame_scroll_value + settings->max_num_frames_displayed;
	
	//draw frame marks and frame number
	for(int i = *settings->leftmost_frame_ptr; i < *settings->rightmost_frame_ptr; i += 10)
	{
		//tick mark
		DrawRectangle(settings->mouseArea.x + 2*(i - *settings->leftmost_frame_ptr), settings->mouseArea.y, 2,10, BLACK);
		
		//frame number
		if(i % show_frame_modulo_factor == 0)
		{
			char c_buffer[4];
			sprintf(c_buffer,"%ld",i);
			DrawText(c_buffer,settings->mouseArea.x + 2*(i - *settings->leftmost_frame_ptr) + 4,settings->mouseArea.y + 10,12,BLACK);
		}
		
	}
	
	
	
	//if in edit mode
	if(settings->editMode && settings->mouse_control)
	{
		//draw cursor line where mouse pointer is
	
		Vector2 mousePoint = GetMousePosition();

		//select frame based on where mouse pointer is clicked on 
		if (CheckCollisionPointRec(mousePoint, settings->mouseArea))
		{
			float cursor_x = mousePoint.x;
			if(cursor_x < settings->mouseArea.x){cursor_x = settings->mouseArea.x;}

			Color cursor_color = BLACK;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) )
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
				
				if(settings->frameDrawX >= 200)
				{
					settings->current_timeline_frame = static_cast <size_t> ( (settings->frameDrawX - 200) / 2) + settings->frame_scroll_value;
				}
			}
			
			DrawRectangle(cursor_x,400,2,screen_height,cursor_color);
		}
		else
		{
			if(settings->frameSelected)
			{
				Color cursor_color = YELLOW;
				DrawRectangle(settings->frameDrawX,400,2,screen_height,cursor_color);
				
				if(settings->frameDrawX >= 200)
				{
					settings->current_timeline_frame = static_cast <size_t> ( (settings->frameDrawX - 200) / 2) + settings->frame_scroll_value;
				}
				
			}
			
			
		}
	}
	//if in edit mode, and no mouse countrol
	else if(settings->editMode && !settings->mouse_control)
	{
		Color cursor_color = BLACK;
		if(settings->frameSelected)
		{
			cursor_color = YELLOW;
		}
		
		float cursor_x = ((settings->current_timeline_frame - settings->frame_scroll_value) * 2) + 200;
		
		DrawRectangle(cursor_x,400,2,screen_height,cursor_color);
	}
	//else if not in edit mode, i.e. playback mode
	else
	{
		//draw cursor at current timeline frame
		float cursor_x = ((settings->current_timeline_frame - settings->frame_scroll_value) * 2) + 200;
		
		DrawRectangle(cursor_x,400,2,screen_height,BLACK);
	}
}

bool Gui_Timeline_Parameter(TimelineParameterSettings* settings)
{

	//draw points
	if(settings->max_num_frames >= *settings->rightmost_frame_ptr &&
		settings->max_num_frames > *settings->leftmost_frame_ptr)
	{
		bool* element_ptr = settings->array_points_ptr + *settings->leftmost_frame_ptr;
		int i = 0;
		
		for(i = *settings->leftmost_frame_ptr; i < *settings->rightmost_frame_ptr; i++)
		{
			if(*element_ptr)
			{
				DrawCircle(settings->draw_start_x + 2*i, settings->draw_y, 2, settings->draw_color);
			}
			
			element_ptr++;
		}
	}

	
}

#endif
