ResearchOutpost_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, ResearchOutpost_OnResume
	.byte 0

ResearchOutpost_OnResume:
	setstepcallback STEP_CB_SNOW
	end

ResearchOutpost_EventScript_HouseSign::
	msgbox ResearchOutpost_Text_HouseSign, MSGBOX_SIGN
	end

ResearchOutpost_EventScript_RoofScientistSign::
	msgbox ResearchOutpost_Text_RoofScientistSign, MSGBOX_SIGN
	end

ResearchOutpost_Text_HouseSign:
	.string "Now THIS\n"
	.string "Is a placeholder!$"

ResearchOutpost_Text_RoofScientistSign:
	.string "I AM STUCK\n"
	.string "ON THIS ROOF!$"
