#pragma once
#include <iostream>
#include <math.h>
#include "../UI/UI.hpp"

#include "../taunt_toggles.h"
#define MULTILINE(...) #__VA_ARGS__

using namespace ImGui;

class TrainingMenu : public UiItem
{
public:
    void DrawMain()
    {
        char buffer[100];
        TrainingModpackMenu* menu_addr = 0;
        Log = "\nLog\n";

        FILE* f = fopen("sdmc:/SaltySD/training_modpack.log", "r");
        if (f) {
            int read_bytes = fread(buffer, 1, 100, f);
            fclose(f);
            buffer[read_bytes] = '\0';
            menu_addr = (TrainingModpackMenu*) strtoul(buffer, NULL, 16);
        } else Log += "Failed to open log file.\n";

        if (menu_addr) {
            Result rc;
            Handle debug;

            u64 title;
            u64 pid;
            pmdmntGetApplicationPid(&pid);
            pminfoGetTitleId(&title, pid);

            if (title == 0x01006A800016E000) {
                rc = svcDebugActiveProcess(&debug, pid);
                if (R_SUCCEEDED(rc)) {
                    rc = svcReadDebugProcessMemory(&menu, debug, (u64)menu_addr, sizeof(menu));
                    if (R_SUCCEEDED(rc)) {
                        if (menu.print_buffer_len > 0)
                            Log += menu.print_buffer;
                    } else Log += "Failed to read process memory.\n";

                    Checkbox("Hitbox Visualization", &menu.HITBOX_VIS);
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Mash Toggles", &menu.MASH_STATE, mash_items, IM_ARRAYSIZE(mash_items));
                    if (menu.MASH_STATE == MASH_ATTACK) {
                        PushItemWidth(GetWindowWidth() * 0.5f);
                        Combo("Attack", &menu.ATTACK_STATE, attack_items, IM_ARRAYSIZE(attack_items));
                    }
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Ledge Options", &menu.LEDGE_STATE, ledge_items, IM_ARRAYSIZE(ledge_items));
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Tech Options", &menu.TECH_STATE, tech_items, IM_ARRAYSIZE(tech_items));
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Defensive Options", &menu.DEFENSIVE_STATE, defensive_items, IM_ARRAYSIZE(defensive_items));
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Shield Options", &menu.SHIELD_STATE, shield_items, IM_ARRAYSIZE(shield_items));
                    PushItemWidth(GetWindowWidth() * 0.5f);
                    Combo("Set DI", &menu.DI_STATE, di_items, IM_ARRAYSIZE(di_items));

                    if (Button("Help")) {
                        OpenPopup("Help");
                    }
                    SetNextWindowPos(ImVec2(0,0));
                    if (BeginPopupModal("Help", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        Text(HelpTexts[HelpTxtIndex]);
                        Spacing();
                        if (Button("Next"))
                            HelpTxtIndex = (HelpTxtIndex + 1) % IM_ARRAYSIZE(HelpTexts);
                        Spacing();
                        if (Button("Close"))
                            CloseCurrentPopup();
                        EndPopup();
                    }

                    rc = svcWriteDebugProcessMemory(debug, &menu, (u64)menu_addr, sizeof(menu));
                    if (R_FAILED(rc)) Log += "Failed to write process memory.\n";

                    svcCloseHandle(debug);
                } else Log += "Failed to debug process.\n";
            } else Log += "Application is not Smash.\n";
        } else Log += "Struct address is null.\n";

        // Text(Log.c_str());
    }

	bool Draw() override
	{		
		if(renderDirty > 0)
		{
		if (!ImGui::Begin("Training Menu", &Running) || !Running)
		{
			ImGui::End();
			return Running;
		}
        
        DrawMain();

		ImGui::End();
		}
		return true;
	}
private:
	bool Running = true;
    std::string Log;

    size_t HelpTxtIndex = 0;
    const char* HelpTexts[7] = { R""""(Hitbox Visualization

Currently, hitboxes and grabboxes are supported.)"""",
    R""""(Mash Toggles

Airdodge
CPUs will mash airdodge on the first frame out of hitstun.
CPUs will also shield quickly if they are hit and remain grounded.

Jump
CPUs will mash jump on the first frame out of hitstun.

Attack
CPUs will mash an attack on the first frame out of hitstun and when landing.

Random
CPUs will mash an aerial or grounded option on the first frame out of hitstun and when landing. 

The aerial options include: 
    Airdodge, jump, all aerials, all specials
The grounded options include: 
    Jump, jab, all tilts, all smashes, all specials, 
    grab, spotdodge, and rolls)"""",
    R""""(Tech Options

CPUs will perform a random tech option. 
Specific tech options can be chosen and include:
    In place, roll, and miss tech
CPUs will also perform a defensive option after getting up.)"""",
    R""""(Ledge Options

CPUs will perform a ledge option. 
Specific ledge options can be chosen and include:
    Normal, roll, jump, and attack
CPUs will also perform a defensive option after getting up.
)"""",
    R""""(Defensive Options

Choose the defensive option a CPU will perform after teching or
getting up from the ledge. 
Specific options include:
    Flash shield, spotdodge, and jab
)"""",
    R""""(Shield Options

Infinite
CPUs will hold a shield that does not deteriorate over time or by damage.

Hold
CPUs will hold a normal shield.)"""",
    R""""(Set DI

Specified Direction
CPUs DI in the direction specified (relative to the player's facing position).

Random Direction
CPUs DI randomly in or away.)"""" };
};