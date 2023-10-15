ResearchOutpost_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, ResearchOutpost_OnResume
	.byte 0

ResearchOutpost_OnResume:
	setstepcallback STEP_CB_SNOW
	end