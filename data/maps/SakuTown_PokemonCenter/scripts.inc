SakuTown_PokemonCenter_MapScripts::
# 2 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, SakuTown_PokemonCenter_MapScripts_MAP_SCRIPT_ON_TRANSITION
	.byte 0

SakuTown_PokemonCenter_MapScripts_MAP_SCRIPT_ON_TRANSITION:
# 3 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	setrespawn HEAL_LOCATION_SAKU_TOWN
	return


SakuTown_PokemonCenter_EventScript_Nurse::
# 8 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	setvar VAR_0x800B, LOCALID_NURSE
# 9 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	call Common_EventScript_PkmnCenterNurse_CutTheBullshit
# 10 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	waitmessage
# 11 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	waitbuttonpress
# 12 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	release
	end


SakuTown_PokemonCenter_EventScript_Traveler::
# 17 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	lock
# 18 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	msgbox SakuTown_PokemonCenter_EventScript_Traveler_Text_0
# 23 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	closemessage
# 24 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	release
	end


SakuTown_PokemonCenter_EventScript_Traveler_Text_0:
# 19 "data/maps/SakuTown_PokemonCenter/scripts.pory"
	.string "This Pokémon refuge is brand-new,\n"
	.string "courtesy of the Pokémon League!\p"
	.string "As you can see, it's still being built.\n"
	.string "Watch out for the falling bricks, ha\l"
	.string "ha, just kidding!\p"
	.string "Seriously though, do watch out. We're\n"
	.string "held responsible if anything happens\l"
	.string "to you.$"