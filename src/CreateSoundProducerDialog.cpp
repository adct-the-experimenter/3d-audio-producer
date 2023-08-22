#include "CreateSoundProducerDialog.h"

#include "global_variables.h"

#include "raygui/raygui.h"

#include "raygui/gui_valid_float.h"

#include "imgui.h"
#include "backends/rlImGui.h"
#include "backends/imfilebrowser.h"

#include <cstring>

CreateSoundProducerDialog::CreateSoundProducerDialog(const std::string& title)
{
	xPosition = 0.0; yPosition = 0.0; zPosition = 0.0;
	okClicked = false; cancelClicked = false;
	tempFreeRoamBool = false;
	m_sound_bank_ptr = nullptr;
	sound_choices = "";
}


void CreateSoundProducerDialog::SetPointerToSoundBank(SoundBank* thisSoundBank){m_sound_bank_ptr = thisSoundBank;}

static float xValue = 0.0f;
static float yValue = 0.0f;
static float zValue = 0.0f;

static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static std::uint8_t account_num;

static char char_name[32] = "name here";
static bool name_box_pressed = false;

static bool free_roam_box_stat = false;

static bool dropDownSoundMode = false;
static int dropDownSoundActive = 0;

static std::array <std::string,10> sound_items;

void CreateSoundProducerDialog::DrawDialog()
{	
	ImGui::OpenPopup("Create Sound Producer");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Create Sound Producer", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		//display name input box
		if(ImGui::InputText("Name", char_name, 32))
		{
			name_box_pressed = !name_box_pressed;
		}
		
		//display x value float input box
		ImGui::InputFloat("x", &xValue, 0.01f, 1.0f, "%.3f");
		
		//display y value float input box
		ImGui::InputFloat("y", &yValue, 0.01f, 1.0f, "%.3f");
		
		//display z value float input box
		ImGui::InputFloat("z", &zValue, 0.01f, 1.0f, "%.3f");
		
		ImGui::Separator();
		
		ImGui::Checkbox("Free Roam",&free_roam_box_stat);
		
		ImGui::Separator();
		
		if(m_sound_bank_ptr)
		{
			static int item_current_idx = 0; // Here we store our selection data as an index.
			const char* combo_preview_value = sound_items[item_current_idx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
			static ImGuiComboFlags flags = 0;
						
			if (ImGui::BeginCombo("Sound Account", combo_preview_value, flags))
			{
				for (int n = 0; n < sound_items.size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(sound_items[n].c_str(), is_selected))
					{
						item_current_idx = n;
						account_num = item_current_idx;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			
		}
		
		//display OK and Cancel button on the same lines
		if (ImGui::Button("OK", ImVec2(120, 0))) 
		{ 
			okClicked = true;
			ImGui::CloseCurrentPopup(); 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) 
		{
			cancelClicked = true; 
			ImGui::CloseCurrentPopup(); 
		}
		
		ImGui::EndPopup();
	}
	
	
	
	if(okClicked)
	{
		name = std::string(char_name);
		xPosition = xValue;
		yPosition = yValue;
		zPosition = zValue;
		tempFreeRoamBool = free_roam_box_stat;
		sound_bank_account_num = account_num;
	}
}

std::string CreateSoundProducerDialog::getNewName(){return name;}

void CreateSoundProducerDialog::getNewPosition(float& x, float& y, float& z)
{
	x = xPosition;
	y = yPosition;
	z = zPosition;
}


bool CreateSoundProducerDialog::OkClickedOn(){ return okClicked;}

bool CreateSoundProducerDialog::CancelClickedOn(){ return cancelClicked;}

bool CreateSoundProducerDialog::getFreeRoamBool(){return tempFreeRoamBool;}

std::uint8_t CreateSoundProducerDialog::getAccountNumber(){return sound_bank_account_num;}

void CreateSoundProducerDialog::resetConfig()
{
	xPosition = 0;
	yPosition = 0;
	zPosition = 0;
	
	xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

	yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

	zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");
	
	sound_bank_account_num = 0;
	okClicked = false;
	cancelClicked = false;
	tempFreeRoamBool = false;
	
	memset(char_name, 0, sizeof(char_name));
	strncpy(char_name, "name here", 20);
	char_name[19] = '\0';
}

void CreateSoundProducerDialog::InitSoundBankChoices()
{
	
	if(m_sound_bank_ptr)
	{
		//clear previous sound choices
		sound_choices = "";
		
		//put choices into specific format
		sound_items = m_sound_bank_ptr->GetAccountLookupTable();
		
		//add text so that imgui does not complain about unique labels
		for(size_t i = 0; i < sound_items.size(); i++)
		{
			sound_items[i] = sound_items[i] + " ( " + std::to_string(i) + " )";
		}
		
	}
}
