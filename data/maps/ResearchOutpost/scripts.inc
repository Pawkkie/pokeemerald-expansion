ResearchOutpost_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, ResearchOutpost_OnResume
	.byte 0

ResearchOutpost_OnResume:
	setstepcallback STEP_CB_SNOW
	end

ResearchOutpost_EventScript_CitySign::
	msgbox ResearchOutpost_Text_Sign, MSGBOX_SIGN
	end

ResearchOutpost_Text_Sign:
	.string "Now THIS\n"
	.string "Is a placeholder!$"