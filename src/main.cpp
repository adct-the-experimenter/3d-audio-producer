
#include "raylib.h"
#include "main_gui_editor.h"

MainGuiEditor editor;



const std::int16_t screenWidth = 850;
const std::int16_t screenHeight = 600;

void InitRaylibSystem();
void CloseRaylibSystem();

void ApplicationLoop();

int main(int argc, char* args[])
{
	if(!editor.OnInit())
	{
		std::cout << "Failed to initialize core editor of the program!\n";
		return -1;
	}
	
	InitRaylibSystem();
	
	editor.InitCamera();
	
	bool quit = false;
	
	while (!quit)
	{
		// Detect window close button or ESC key
		if(WindowShouldClose())
		{
			quit = true;
		}    
		
		// run Application loop
		ApplicationLoop();
	}
	
	CloseRaylibSystem();
	
	return 0;
}

void ApplicationLoop()
{
	//handle events
	editor.HandleEvents();
	
	//logic 
	UpdateCamera(editor.GetPointerToCamera());
	editor.logic();
	
	//draw
	BeginDrawing();

	ClearBackground(RAYWHITE);
	
	//draw 3d elements
	BeginMode3D(*editor.GetPointerToCamera());
	
	editor.Draw3DModels();
	
	EndMode3D();
	
	//draw GUI elements
	editor.DrawGUI_Items();
	
	
	
	EndDrawing();
}

void InitRaylibSystem()
{
	
	SetConfigFlags(FLAG_MSAA_4X_HINT || FLAG_VSYNC_HINT);  // Set MSAA 4X hint before windows creation
	
    InitWindow(screenWidth, screenHeight, "3d Audio Producer");
	
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    

}

void CloseRaylibSystem()
{
    CloseWindow();        // Close window and OpenGL context
    
}
