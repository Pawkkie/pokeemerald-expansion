.set LOCALID_RIVAL, 1

LittlerootTown_BrendansHouse_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_BrendansHouse_2F_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_BrendansHouse_2F_OnWarp
	.byte 0

LittlerootTown_BrendansHouse_2F_OnTransition:
	call_if_lt VAR_LITTLEROOT_RIVAL_STATE, 2, LittlerootTown_BrendansHouse_2F_EventScript_CheckSetReadyToMeetBrendan
	call_if_ge VAR_LITTLEROOT_RIVAL_STATE, 3, LittlerootTown_BrendansHouse_2F_EventScript_CheckShouldUpdateBrendanPos
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 4, PlayersHouse_2F_EventScript_BlockStairsUntilClockIsSet
	call SecretBase_EventScript_SetDecorationFlags
	setvar VAR_SECRET_BASE_INITIALIZED, 0
	end

LittlerootTown_BrendansHouse_2F_EventScript_CheckShouldUpdateBrendanPos::
	goto_if_set FLAG_MET_RIVAL_LILYCOVE, LittlerootTown_BrendansHouse_2F_EventScript_TryUpdateBrendanPos
	goto_if_ge VAR_BIRCH_LAB_STATE, 2, LittlerootTown_BrendansHouse_2F_EventScript_Ret
	goto LittlerootTown_BrendansHouse_2F_EventScript_TryUpdateBrendanPos

LittlerootTown_BrendansHouse_2F_EventScript_TryUpdateBrendanPos::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LittlerootTown_BrendansHouse_2F_EventScript_Ret
	@ Odd that the MaysHouse equivalent was used below instead
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, LittlerootTown_MaysHouse_2F_EventScript_Ret
	setobjectxyperm LOCALID_RIVAL, 0, 2
	setobjectmovementtype LOCALID_RIVAL, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_BrendansHouse_2F_EventScript_Ret::
	return

LittlerootTown_BrendansHouse_2F_GamecubeEnterCheatCode::
	lockall
	msgbox EnterCheatCode_EnterCheatCodeText, MSGBOX_YESNO
	compare VAR_RESULT, 0
	goto_if_eq CheatCodeExit
	special EnterCheatCode
	waitstate
	special GetCheatCodeFeedback
	goto_if_eq VAR_RESULT, 0, CheatCodeFailed
	goto_if_eq VAR_RESULT, 1, CheatCodeCandyJar
	goto_if_eq VAR_RESULT, 2, CheatCodeNeed4Speed
	goto_if_eq VAR_RESULT, 3, CheatCodeSeenEmAll
	goto_if_eq VAR_RESULT, 4, CheatCodeCaughtEmAll
	goto_if_eq VAR_RESULT, 5, CheatCodeBestBall
	goto_if_eq VAR_RESULT, 6, CheatCodeEviesDayOff
	goto_if_eq VAR_RESULT, 7, CheatCodeIvysDayOff
	goto_if_eq VAR_RESULT, 8, CheatCodeUnHatted
	end

CheatCodeFailed::
	msgbox EnterCheatCode_FailedText, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeCandyJar::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	giveitem ITEM_CANDY_JAR, 1
	releaseall
	end

CheatCodeNeed4Speed::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	goto_if_unset FLAG_NEED4SPEED, CheatCodeNeed4SpeedOn
	goto_if_set FLAG_NEED4SPEED, CheatCodeNeed4SpeedOff
	end

CheatCodeNeed4SpeedOn::
	setflag FLAG_NEED4SPEED
	msgbox CheatCodeNeed4SpeedOn_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeNeed4SpeedOff::
	clearflag FLAG_NEED4SPEED
	msgbox CheatCodeNeed4SpeedOff_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeSeenEmAll::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	msgbox CheatCodeSeenEmAll_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeCaughtEmAll::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	msgbox CheatCodeCaughtEmAll_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeBestBall::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	goto_if_unset FLAG_BEST_BALL, CheatCodeBestBallOff
	goto_if_set FLAG_BEST_BALL, CheatCodeBestBallOn
	end

CheatCodeBestBallOn::
	msgbox CheatCodeBestBallOn_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeBestBallOff::
	msgbox CheatCodeBestBallOff_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeEviesDayOff::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	goto_if_unset FLAG_EVS_DISABLED, CheatCodeEviesDayOffOff
	goto_if_set FLAG_EVS_DISABLED, CheatCodeEviesDayOffOn
	end

CheatCodeEviesDayOffOn::
	msgbox CheatCodeEviesDayOffOn_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeEviesDayOffOff::
	msgbox CheatCodeEviesDayOffOff_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeIvysDayOff::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	goto_if_unset FLAG_IVS_DISABLED, CheatCodeIvysDayOffOff
	goto_if_set FLAG_IVS_DISABLED, CheatCodeIvysDayOffOn
	end

CheatCodeIvysDayOffOn::
	msgbox CheatCodeIvysDayOffOn_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeIvysDayOffOff::
	msgbox CheatCodeIvysDayOffOff_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeUnHatted::
	msgbox EnterCheatCode_SucceededText, MSGBOX_DEFAULT
	goto_if_unset FLAG_NO_LEVEL_CAP, CheatCodeUnhattedOff
	goto_if_set FLAG_NO_LEVEL_CAP, CheatCodeUnhattedOn

CheatCodeUnhattedOff::
	msgbox CheatCodeUnhattedOff_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeUnhattedOn::
	msgbox CheatCodeUnhattedOn_Text, MSGBOX_DEFAULT
	releaseall
	end

CheatCodeExit::
	releaseall
	end

EnterCheatCode_EnterCheatCodeText:
	.string "It's a Nintendo GameCube.\p"
	.string "Enter a cheat code?$"
EnterCheatCode_FailedText:
	.string "...nothing happened.$"
EnterCheatCode_SucceededText:
	.string "The code worked!$"

CheatCodeNeed4SpeedOn_Text:
	.string "You feel a strong wind at your back.\n"
	.string "You can run at mach speed!$"

CheatCodeNeed4SpeedOff_Text:
	.string "The wind at your back peters out.\n"
	.string "Your run speed returns to normal.$"

CheatCodeSeenEmAll_Text:
	.string "Dozens of Pokémon rush through your\n"
	.string "head. It's like you've seen 'em all!$"

CheatCodeCaughtEmAll_Text:
	.string "Encyclopedic knowledge fills your head.\n"
	.string "It's like you've caught 'em all!$"

CheatCodeBestBallOn_Text:
	.string "Something clicks in your bag. Your\n"
	.string "Poké Balls all seem enhanced!$"

CheatCodeBestBallOff_Text:
	.string "Something clicks in your bag. Your\n"
	.string "Poké Balls return to normal.$"

CheatCodeEviesDayOffOn_Text:
	.string "You get the sense Evie is taking some\n"
	.string "time off. Effect of EVs disabled.$"

CheatCodeEviesDayOffOff_Text:
	.string "You get the sense Evie has returned from\n"
	.string "her time off. Effect of EVs enabled.$"

CheatCodeIvysDayOffOn_Text:
	.string "You get the sense Ivy is taking some\n"
	.string "time off. Effect of EVs disabled.$"

CheatCodeIvysDayOffOff_Text:
	.string "You get the sense Ivy has returned from\n"
	.string "her time off. Effect of EVs enabled.$"

CheatCodeUnhattedOff_Text:
	.string "The rush of energy subsides.\n"
	.string "Gym leader level caps reinstated.$"

CheatCodeUnhattedOn_Text:
	.string "A rush of energy lets you surpass your\n"
	.string "limits! Gym leader level caps removed.$"


LittlerootTown_BrendansHouse_2F_EventScript_CheckSetReadyToMeetBrendan::
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_BrendansHouse_2F_EventScript_SetReadyToMeetBrendan
	return

LittlerootTown_BrendansHouse_2F_EventScript_SetReadyToMeetBrendan::
	setvar VAR_LITTLEROOT_RIVAL_STATE, 2
	return

LittlerootTown_BrendansHouse_2F_OnWarp:
	map_script_2 VAR_SECRET_BASE_INITIALIZED, 0, LittlerootTown_BrendansHouse_2F_EventScript_CheckInitDecor
	.2byte 0

LittlerootTown_BrendansHouse_2F_EventScript_CheckInitDecor::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, SecretBase_EventScript_InitDecorations
	end

LittlerootTown_BrendansHouse_2F_EventScript_RivalsPokeBall::
	lockall
	goto_if_eq VAR_LITTLEROOT_RIVAL_STATE, 2, LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendan
	msgbox RivalsHouse_2F_Text_ItsRivalsPokeBall, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendan::
	delay 10
	addobject LOCALID_RIVAL
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanEnters
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RIVAL, Common_Movement_Delay48
	waitmovement 0
	delay 10
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	call_if_eq VAR_FACING, DIR_NORTH, LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanNorth
	call_if_eq VAR_FACING, DIR_SOUTH, LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanSouth
	call_if_eq VAR_FACING, DIR_WEST, LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanWest
	call_if_eq VAR_FACING, DIR_EAST, LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanEast
	setvar VAR_LITTLEROOT_RIVAL_STATE, 3
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F_POKE_BALL
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_BEDROOM
	setvar VAR_LITTLEROOT_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanNorth::
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerNorth
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox RivalsHouse_2F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanNorth
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCNorth
	waitmovement 0
	return

LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanSouth::
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerSouth
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox RivalsHouse_2F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanSouth
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCSouth
	waitmovement 0
	return

LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanWest::
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerWest
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox RivalsHouse_2F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanWest
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCWest
	waitmovement 0
	return

LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendanEast::
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerEast
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox RivalsHouse_2F_Text_BrendanWhoAreYou, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_RIVAL, LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCEast
	waitmovement 0
	return

LittlerootTown_BrendansHouse_2F_Movement_BrendanEnters:
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerNorth:
	walk_left
	walk_left
	walk_down
	walk_down
	walk_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCNorth:
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanNorth:
	delay_16
	walk_in_place_faster_up
	delay_16
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerSouth:
	walk_left
	walk_left
	walk_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCSouth:
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanSouth:
	delay_16
	walk_in_place_faster_up
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerWest:
	walk_left
	walk_left
	walk_down
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCWest:
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanWest:
	delay_8
	delay_16
	walk_in_place_faster_up
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanApproachPlayerEast:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_in_place_faster_down
	step_end

LittlerootTown_BrendansHouse_2F_Movement_BrendanWalkToPCEast:
	walk_up
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

@ Unused, the player is already facing this direction so its unneeded
LittlerootTown_BrendansHouse_2F_Movement_PlayerWatchBrendanEast:
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

LittlerootTown_BrendansHouse_2F_EventScript_PC::
	lockall
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LittlerootTown_BrendansHouse_2F_EventScript_CheckPlayersPC
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_BrendansHouse_2F_EventScript_CheckRivalsPC
	end

LittlerootTown_BrendansHouse_2F_EventScript_CheckPlayersPC::
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox gText_PlayerHouseBootPC, MSGBOX_DEFAULT
	special BedroomPC
	waitstate
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_TurnOffPlayerPC::
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	end

LittlerootTown_BrendansHouse_2F_EventScript_CheckRivalsPC::
	msgbox gText_PokemonTrainerSchoolEmail, MSGBOX_DEFAULT
	releaseall
	end

PlayersHouse_2F_EventScript_Notebook::
	msgbox PlayersHouse_2F_Text_Notebook, MSGBOX_SIGN
	end

PlayersHouse_2F_EventScript_GameCube::
	msgbox PlayersHouse_2F_Text_ItsAGameCube, MSGBOX_SIGN
	end

PlayersHouse_2F_Text_ClockIsStopped:
	.string "The clock is stopped…\p"
	.string "Better set it and start it!$"

PlayersHouse_2F_Text_HowDoYouLikeYourRoom:
	.string "MOM: {PLAYER}, how do you like your\n"
	.string "new room?\p"
	.string "Good! Everything's put away neatly!\p"
	.string "They finished moving everything in\n"
	.string "downstairs, too.\p"
	.string "POKéMON movers are so convenient!\p"
	.string "Oh, you should make sure that\n"
	.string "everything's all there on your desk.$"

PlayersHouse_2F_Text_Notebook:
	.string "{PLAYER} flipped open the notebook.\p"
	.string "ADVENTURE RULE NO. 1\n"
	.string "Open the MENU with START.\p"
	.string "ADVENTURE RULE NO. 2\n"
	.string "Record your progress with SAVE.\p"
	.string "The remaining pages are blank…$"

Common_Text_LookCloserAtMap:
	.string "{PLAYER} took a closer look at the\n"
	.string "HOENN region map.$"

PlayersHouse_2F_Text_ItsAGameCube:
	.string "It's a Nintendo GameCube.\p"
	.string "A Game Boy Advance is connected to\n"
	.string "serve as the Controller.$"
