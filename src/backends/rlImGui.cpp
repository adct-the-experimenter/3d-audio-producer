/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   rlImGui * basic ImGui integration
*
*   LICENSE: ZLIB
*
*   Copyright (c) 2020 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/
#include "rlImGui.h"

#include "imgui.h"
#include "raylib.h"
#include "rlgl.h"

#ifdef PLATFORM_DESKTOP
#include <GLFW/glfw3.h>
#endif

#include <math.h>
#include <map>

#ifndef NO_FONT_AWESOME
#include "extras/FA6FreeSolidFontData.h"
#endif

static Texture2D FontTexture;

static ImGuiMouseCursor CurrentMouseCursor = ImGuiMouseCursor_COUNT;
static MouseCursor MouseCursorMap[ImGuiMouseCursor_COUNT];

static std::map<KeyboardKey, ImGuiKey> RaylibKeyMap;

static bool LastFrameFocused = false;

static bool LastControlPressed = false;
static bool LastShiftPressed = false;
static bool LastAltPressed = false;
static bool LastSuperPressed = false;

bool rlImGuiIsControlDown() { return IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL); }
bool rlImGuiIsShiftDown() { return IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT); }
bool rlImGuiIsAltDown() { return IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT); }
bool rlImGuiIsSuperDown() { return IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER); }

static const char* rlImGuiGetClipText(void*) 
{
	return GetClipboardText();
}

static void rlImGuiSetClipText(void*, const char* text)
{
	SetClipboardText(text);
}

static void rlImGuiNewFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	if (IsWindowFullscreen())
	{
		int monitor = GetCurrentMonitor();
		io.DisplaySize.x = float(GetMonitorWidth(monitor));
		io.DisplaySize.y = float(GetMonitorHeight(monitor));
	}
	else
	{
		io.DisplaySize.x = float(GetScreenWidth());
		io.DisplaySize.y = float(GetScreenHeight());
	}

	int width = rlGetFramebufferWidth();
	int height = rlGetFramebufferHeight();
	if (width > 0 && height > 0) {
		io.DisplayFramebufferScale = ImVec2(width / io.DisplaySize.x, height / io.DisplaySize.y);
	}
	else {
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}

	io.DeltaTime = GetFrameTime();

	if (io.WantSetMousePos)
	{
		SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y);
	}
	else
	{
		io.MousePos.x = (float)GetMouseX();
		io.MousePos.y = (float)GetMouseY();
	}

	io.MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	io.MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
	io.MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

	{
		Vector2 mouseWheel = GetMouseWheelMoveV();
		io.MouseWheel += mouseWheel.y;
		io.MouseWheelH += mouseWheel.x;
	}

	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	{
		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor != CurrentMouseCursor || io.MouseDrawCursor)
		{
			CurrentMouseCursor = imgui_cursor;
			if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
			{
				HideCursor();
			}
			else
			{
				ShowCursor();

				if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
				{
					SetMouseCursor((imgui_cursor > -1 && imgui_cursor < ImGuiMouseCursor_COUNT) ? MouseCursorMap[imgui_cursor] : MOUSE_CURSOR_DEFAULT);
				}
			}
		}
	}
}

static void rlImGuiEvents()
{
	ImGuiIO& io = ImGui::GetIO();

	bool focused = IsWindowFocused();
	if (focused != LastFrameFocused)
		io.AddFocusEvent(focused);
	LastFrameFocused = focused;

	// handle the modifyer key events so that shortcuts work
	bool ctrlDown = rlImGuiIsControlDown();
	if (ctrlDown != LastControlPressed)
		io.AddKeyEvent(ImGuiMod_Ctrl, ctrlDown);
	LastControlPressed = ctrlDown;

	bool shiftDown = rlImGuiIsShiftDown();
	if (shiftDown != LastShiftPressed)
		io.AddKeyEvent(ImGuiMod_Shift, shiftDown);
	LastShiftPressed = shiftDown;

	bool altDown = rlImGuiIsAltDown();
	if (altDown != LastAltPressed)
		io.AddKeyEvent(ImGuiMod_Alt, altDown);
	LastAltPressed = altDown;

	bool superDown = rlImGuiIsSuperDown();
	if (superDown != LastSuperPressed)
		io.AddKeyEvent(ImGuiMod_Super, superDown);
	LastSuperPressed = superDown;

	// get the pressed keys, they are in event order
	int keyId = GetKeyPressed();
	while (keyId != 0)
	{
		auto keyItr = RaylibKeyMap.find(KeyboardKey(keyId));
		if (keyItr != RaylibKeyMap.end())
			io.AddKeyEvent(keyItr->second, true);
		keyId = GetKeyPressed();
	}

 	for (auto keyItr : RaylibKeyMap)
 		io.KeysData[keyItr.second].Down = IsKeyDown(keyItr.first);

	// look for any keys that were down last frame and see if they were down and are released
	for (const auto keyItr : RaylibKeyMap)
	{
		if (IsKeyReleased(keyItr.first))
			io.AddKeyEvent(keyItr.second, false);
	}

	// add the text input in order
	unsigned int pressed = GetCharPressed();
	while (pressed != 0)
	{
		io.AddInputCharacter(pressed);
		pressed = GetCharPressed();
	}
}

static void rlImGuiTriangleVert(ImDrawVert& idx_vert)
{
	Color* c;
	c = (Color*)&idx_vert.col;
	rlColor4ub(c->r, c->g, c->b, c->a);
	rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
	rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
}

static void rlImGuiRenderTriangles(unsigned int count, int indexStart, const ImVector<ImDrawIdx>& indexBuffer, const ImVector<ImDrawVert>& vertBuffer, void* texturePtr)
{
	if (count < 3)
		return;

	Texture* texture = (Texture*)texturePtr;

	unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

	rlBegin(RL_TRIANGLES);
	rlSetTexture(textureId);

	for (unsigned int i = 0; i <= (count - 3); i += 3)
	{
		if (rlCheckRenderBatchLimit(3))
		{
			rlBegin(RL_TRIANGLES);
			rlSetTexture(textureId);
		}

		ImDrawIdx indexA = indexBuffer[indexStart + i];
		ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
		ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

		ImDrawVert vertexA = vertBuffer[indexA];
		ImDrawVert vertexB = vertBuffer[indexB];
		ImDrawVert vertexC = vertBuffer[indexC];

		rlImGuiTriangleVert(vertexA);
		rlImGuiTriangleVert(vertexB);
		rlImGuiTriangleVert(vertexC);
	}
	rlEnd();
}

static void EnableScissor(float x, float y, float width, float height)
{
	rlEnableScissorTest();
	ImGuiIO& io = ImGui::GetIO();
	rlScissor((int)(x * io.DisplayFramebufferScale.x),
		int((GetScreenHeight() - (int)(y + height)) * io.DisplayFramebufferScale.y),
		(int)(width * io.DisplayFramebufferScale.x),
		(int)(height * io.DisplayFramebufferScale.y));
}

static void rlRenderData(ImDrawData* data)
{
	rlDrawRenderBatchActive();
	rlDisableBackfaceCulling();

	for (int l = 0; l < data->CmdListsCount; ++l)
	{
		const ImDrawList* commandList = data->CmdLists[l];

		for (const auto& cmd : commandList->CmdBuffer)
		{
			EnableScissor(cmd.ClipRect.x - data->DisplayPos.x, cmd.ClipRect.y - data->DisplayPos.y, cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x), cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y));
			if (cmd.UserCallback != nullptr)
			{
				cmd.UserCallback(commandList, &cmd);

				continue;
			}

			rlImGuiRenderTriangles(cmd.ElemCount, cmd.IdxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
			rlDrawRenderBatchActive();
		}
	}

	rlSetTexture(0);
	rlDisableScissorTest();
	rlEnableBackfaceCulling();
}

void SetupMouseCursors()
{
	MouseCursorMap[ImGuiMouseCursor_Arrow] = MOUSE_CURSOR_ARROW;
	MouseCursorMap[ImGuiMouseCursor_TextInput] = MOUSE_CURSOR_IBEAM;
	MouseCursorMap[ImGuiMouseCursor_Hand] = MOUSE_CURSOR_POINTING_HAND;
	MouseCursorMap[ImGuiMouseCursor_ResizeAll] = MOUSE_CURSOR_RESIZE_ALL;
	MouseCursorMap[ImGuiMouseCursor_ResizeEW] = MOUSE_CURSOR_RESIZE_EW;
	MouseCursorMap[ImGuiMouseCursor_ResizeNESW] = MOUSE_CURSOR_RESIZE_NESW;
	MouseCursorMap[ImGuiMouseCursor_ResizeNS] = MOUSE_CURSOR_RESIZE_NS;
	MouseCursorMap[ImGuiMouseCursor_ResizeNWSE] = MOUSE_CURSOR_RESIZE_NWSE;
	MouseCursorMap[ImGuiMouseCursor_NotAllowed] = MOUSE_CURSOR_NOT_ALLOWED;
}

void rlImGuiEndInitImGui()
{
	SetupMouseCursors();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = "imgui_impl_raylib";

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

	io.MousePos = ImVec2(0, 0);

	io.SetClipboardTextFn = rlImGuiSetClipText;
	io.GetClipboardTextFn = rlImGuiGetClipText;

	io.ClipboardUserData = nullptr;

	rlImGuiReloadFonts();
}

void rlSetupKeymap()
{
	if (!RaylibKeyMap.empty())
		return;

	// build up a map of raylib keys to ImGuiKeys
	RaylibKeyMap[KEY_APOSTROPHE] = ImGuiKey_Apostrophe;
	RaylibKeyMap[KEY_COMMA] = ImGuiKey_Comma;
	RaylibKeyMap[KEY_MINUS] = ImGuiKey_Minus;
	RaylibKeyMap[KEY_PERIOD] = ImGuiKey_Period;
	RaylibKeyMap[KEY_SLASH] = ImGuiKey_Slash;
	RaylibKeyMap[KEY_ZERO] = ImGuiKey_0;
	RaylibKeyMap[KEY_ONE] = ImGuiKey_1;
	RaylibKeyMap[KEY_TWO] = ImGuiKey_2;
	RaylibKeyMap[KEY_THREE] = ImGuiKey_3;
	RaylibKeyMap[KEY_FOUR] = ImGuiKey_4;
	RaylibKeyMap[KEY_FIVE] = ImGuiKey_5;
	RaylibKeyMap[KEY_SIX] = ImGuiKey_6;
	RaylibKeyMap[KEY_SEVEN] = ImGuiKey_7;
	RaylibKeyMap[KEY_EIGHT] = ImGuiKey_8;
	RaylibKeyMap[KEY_NINE] = ImGuiKey_9;
	RaylibKeyMap[KEY_SEMICOLON] = ImGuiKey_Semicolon;
	RaylibKeyMap[KEY_EQUAL] = ImGuiKey_Equal;
	RaylibKeyMap[KEY_A] = ImGuiKey_A;
	RaylibKeyMap[KEY_B] = ImGuiKey_B;
	RaylibKeyMap[KEY_C] = ImGuiKey_C;
	RaylibKeyMap[KEY_D] = ImGuiKey_D;
	RaylibKeyMap[KEY_E] = ImGuiKey_E;
	RaylibKeyMap[KEY_F] = ImGuiKey_F;
	RaylibKeyMap[KEY_G] = ImGuiKey_G;
	RaylibKeyMap[KEY_H] = ImGuiKey_H;
	RaylibKeyMap[KEY_I] = ImGuiKey_I;
	RaylibKeyMap[KEY_J] = ImGuiKey_J;
	RaylibKeyMap[KEY_K] = ImGuiKey_K;
	RaylibKeyMap[KEY_L] = ImGuiKey_L;
	RaylibKeyMap[KEY_M] = ImGuiKey_M;
	RaylibKeyMap[KEY_N] = ImGuiKey_N;
	RaylibKeyMap[KEY_O] = ImGuiKey_O;
	RaylibKeyMap[KEY_P] = ImGuiKey_P;
	RaylibKeyMap[KEY_Q] = ImGuiKey_Q;
	RaylibKeyMap[KEY_R] = ImGuiKey_R;
	RaylibKeyMap[KEY_S] = ImGuiKey_S;
	RaylibKeyMap[KEY_T] = ImGuiKey_T;
	RaylibKeyMap[KEY_U] = ImGuiKey_U;
	RaylibKeyMap[KEY_V] = ImGuiKey_V;
	RaylibKeyMap[KEY_W] = ImGuiKey_W;
	RaylibKeyMap[KEY_X] = ImGuiKey_X;
	RaylibKeyMap[KEY_Y] = ImGuiKey_Y;
	RaylibKeyMap[KEY_Z] = ImGuiKey_Z;
	RaylibKeyMap[KEY_SPACE] = ImGuiKey_Space;
	RaylibKeyMap[KEY_ESCAPE] = ImGuiKey_Escape;
	RaylibKeyMap[KEY_ENTER] = ImGuiKey_Enter;
	RaylibKeyMap[KEY_TAB] = ImGuiKey_Tab;
	RaylibKeyMap[KEY_BACKSPACE] = ImGuiKey_Backspace;
	RaylibKeyMap[KEY_INSERT] = ImGuiKey_Insert;
	RaylibKeyMap[KEY_DELETE] = ImGuiKey_Delete;
	RaylibKeyMap[KEY_RIGHT] = ImGuiKey_RightArrow;
	RaylibKeyMap[KEY_LEFT] = ImGuiKey_LeftArrow;
	RaylibKeyMap[KEY_DOWN] = ImGuiKey_DownArrow;
	RaylibKeyMap[KEY_UP] = ImGuiKey_UpArrow;
	RaylibKeyMap[KEY_PAGE_UP] = ImGuiKey_PageUp;
	RaylibKeyMap[KEY_PAGE_DOWN] = ImGuiKey_PageDown;
	RaylibKeyMap[KEY_HOME] = ImGuiKey_Home;
	RaylibKeyMap[KEY_END] = ImGuiKey_End;
	RaylibKeyMap[KEY_CAPS_LOCK] = ImGuiKey_CapsLock;
	RaylibKeyMap[KEY_SCROLL_LOCK] = ImGuiKey_ScrollLock;
	RaylibKeyMap[KEY_NUM_LOCK] = ImGuiKey_NumLock;
	RaylibKeyMap[KEY_PRINT_SCREEN] = ImGuiKey_PrintScreen;
	RaylibKeyMap[KEY_PAUSE] = ImGuiKey_Pause;
	RaylibKeyMap[KEY_F1] = ImGuiKey_F1;
	RaylibKeyMap[KEY_F2] = ImGuiKey_F2;
	RaylibKeyMap[KEY_F3] = ImGuiKey_F3;
	RaylibKeyMap[KEY_F4] = ImGuiKey_F4;
	RaylibKeyMap[KEY_F5] = ImGuiKey_F5;
	RaylibKeyMap[KEY_F6] = ImGuiKey_F6;
	RaylibKeyMap[KEY_F7] = ImGuiKey_F7;
	RaylibKeyMap[KEY_F8] = ImGuiKey_F8;
	RaylibKeyMap[KEY_F9] = ImGuiKey_F9;
	RaylibKeyMap[KEY_F10] = ImGuiKey_F10;
	RaylibKeyMap[KEY_F11] = ImGuiKey_F11;
	RaylibKeyMap[KEY_F12] = ImGuiKey_F12;
	RaylibKeyMap[KEY_LEFT_SHIFT] = ImGuiKey_LeftShift;
	RaylibKeyMap[KEY_LEFT_CONTROL] = ImGuiKey_LeftCtrl;
	RaylibKeyMap[KEY_LEFT_ALT] = ImGuiKey_LeftAlt;
	RaylibKeyMap[KEY_LEFT_SUPER] = ImGuiKey_LeftSuper;
	RaylibKeyMap[KEY_RIGHT_SHIFT] = ImGuiKey_RightShift;
	RaylibKeyMap[KEY_RIGHT_CONTROL] = ImGuiKey_RightCtrl;
	RaylibKeyMap[KEY_RIGHT_ALT] = ImGuiKey_RightAlt;
	RaylibKeyMap[KEY_RIGHT_SUPER] = ImGuiKey_RightSuper;
	RaylibKeyMap[KEY_KB_MENU] = ImGuiKey_Menu;
	RaylibKeyMap[KEY_LEFT_BRACKET] = ImGuiKey_LeftBracket;
	RaylibKeyMap[KEY_BACKSLASH] = ImGuiKey_Backslash;
	RaylibKeyMap[KEY_RIGHT_BRACKET] = ImGuiKey_RightBracket;
	RaylibKeyMap[KEY_GRAVE] = ImGuiKey_GraveAccent;
	RaylibKeyMap[KEY_KP_0] = ImGuiKey_Keypad0;
	RaylibKeyMap[KEY_KP_1] = ImGuiKey_Keypad1;
	RaylibKeyMap[KEY_KP_2] = ImGuiKey_Keypad2;
	RaylibKeyMap[KEY_KP_3] = ImGuiKey_Keypad3;
	RaylibKeyMap[KEY_KP_4] = ImGuiKey_Keypad4;
	RaylibKeyMap[KEY_KP_5] = ImGuiKey_Keypad5;
	RaylibKeyMap[KEY_KP_6] = ImGuiKey_Keypad6;
	RaylibKeyMap[KEY_KP_7] = ImGuiKey_Keypad7;
	RaylibKeyMap[KEY_KP_8] = ImGuiKey_Keypad8;
	RaylibKeyMap[KEY_KP_9] = ImGuiKey_Keypad9;
	RaylibKeyMap[KEY_KP_DECIMAL] = ImGuiKey_KeypadDecimal;
	RaylibKeyMap[KEY_KP_DIVIDE] = ImGuiKey_KeypadDivide;
	RaylibKeyMap[KEY_KP_MULTIPLY] = ImGuiKey_KeypadMultiply;
	RaylibKeyMap[KEY_KP_SUBTRACT] = ImGuiKey_KeypadSubtract;
	RaylibKeyMap[KEY_KP_ADD] = ImGuiKey_KeypadAdd;
	RaylibKeyMap[KEY_KP_ENTER] = ImGuiKey_KeypadEnter;
	RaylibKeyMap[KEY_KP_EQUAL] = ImGuiKey_KeypadEqual;
}

void rlImGuiBeginInitImGui()
{
	rlSetupKeymap();
	ImGui::CreateContext(nullptr);
}

void rlImGuiSetup(bool dark)
{
	LastFrameFocused = IsWindowFocused();
	LastControlPressed = false;
	LastShiftPressed = false;
	LastAltPressed = false;
	LastSuperPressed = false;

	rlImGuiBeginInitImGui();

	if (dark)
		ImGui::StyleColorsDark();
	else
		ImGui::StyleColorsLight();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

#ifndef NO_FONT_AWESOME
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryCompressedTTF((void*)fa_solid_900_compressed_data, fa_solid_900_compressed_size, FONT_AWESOME_ICON_SIZE, &icons_config, icons_ranges);
#endif

	rlImGuiEndInitImGui();
}

void rlImGuiReloadFonts()
{
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels = nullptr;

	int width;
	int height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, nullptr);
	Image image = GenImageColor(width, height, BLANK);
	memcpy(image.data, pixels, width * height * 4);

	if (FontTexture.id != 0)
		UnloadTexture(FontTexture);

	FontTexture = LoadTextureFromImage(image);
	UnloadImage(image);
	io.Fonts->TexID = &FontTexture;
}

void rlImGuiBegin()
{
	rlImGuiNewFrame();
	rlImGuiEvents();
	ImGui::NewFrame();
}

void rlImGuiEnd()
{
	ImGui::Render();
	rlRenderData(ImGui::GetDrawData());
}

void rlImGuiShutdown()
{
	UnloadTexture(FontTexture);

	ImGui::DestroyContext();
}

void rlImGuiImage(const Texture* image)
{
	ImGui::Image((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

bool rlImGuiImageButton(const char* name, const Texture* image)
{
	return ImGui::ImageButton(name, (ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

bool rlImGuiImageButtonSize(const char* name, const Texture* image, ImVec2 size)
{
	return ImGui::ImageButton(name, (ImTextureID)image, size);
}

void rlImGuiImageSize(const Texture* image, int width, int height)
{
	ImGui::Image((ImTextureID)image, ImVec2(float(width), float(height)));
}

void rlImGuiImageRect(const Texture* image, int destWidth, int destHeight, Rectangle sourceRect)
{
	ImVec2 uv0;
	ImVec2 uv1;

	if (sourceRect.width < 0)
	{
		uv0.x = -((float)sourceRect.x / image->width);
		uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / image->width));
	}
	else
	{
		uv0.x = (float)sourceRect.x / image->width;
		uv1.x = uv0.x + (float)(sourceRect.width / image->width);
	}

	if (sourceRect.height < 0)
	{
		uv0.y = -((float)sourceRect.y / image->height);
		uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / image->height));
	}
	else
	{
		uv0.y = (float)sourceRect.y / image->height;
		uv1.y = uv0.y + (float)(sourceRect.height / image->height);
	}

	ImGui::Image((ImTextureID)image, ImVec2(float(destWidth), float(destHeight)), uv0, uv1);
}
