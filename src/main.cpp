
#include "raylib.h"
#include "main_gui_editor.h"

#include "backends/rlImGui.h" //for DearImGUI backend

MainGuiEditor editor;

const std::int16_t screenWidth = 850;
const std::int16_t screenHeight = 640;

void InitRaylibSystem();
void CloseRaylibSystem();

void ApplicationLoop();

static std::string release_ver = "5.0.0";
static std::string release_title = "3D Audio Producer v " + release_ver; 

int main(int argc, char* args[])
{
	if(!editor.OnInit())
	{
		std::cout << "Failed to initialize core editor of the program!\n";
		return -1;
	}
	
	InitRaylibSystem();
	
	editor.InitCamera();
	
	editor.Init3DSceneWindow();
	editor.InitGUIWindow();
	
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
	
	editor.Close3DSceneWindow();
	editor.CloseGUIWindow();
	
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
	
	editor.UpdateTexture3DSceneWindow();
	editor.UpdateTextureGUIWindow();
	
	//draw
	BeginDrawing();
	
	ClearBackground(RAYWHITE);
	// starts the ImGui content mode. Make all ImGui calls after this
	rlImGuiBegin();
	
	//draw 3d scene view
	editor.Draw3DSceneWindow();
	
	//draw GUI elements
	editor.DrawGUIWindow();
	
	// ends the ImGui content mode. Make all ImGui calls before this
	rlImGuiEnd();
	
	EndDrawing();
}

void InitRaylibSystem()
{
	
	// Window configuration flags
	// Set MSAA 4X hint before windows creation, multi-sample anti aliasing
	// set v sync
	//set resizeable window
	//set to high DPI for very high resolutions
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI); 
	
    InitWindow(screenWidth, screenHeight, release_title.c_str());
	
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    //disable escape key as close window key
    SetExitKey(0);
    
    // sets up ImGui with ether a dark or light default theme
    rlImGuiSetup(true); 	

}

void CloseRaylibSystem()
{
	//cleans up imgui
	rlImGuiShutdown();
    CloseWindow();        // Close window and OpenGL context
}
