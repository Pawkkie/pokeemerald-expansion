.set LOCALID_MOVE_RELEARNER, 1

MauvilleCity_House1_MapScripts::
	.byte 0

MauvilleCity_House1_EventScript_RockSmashDude::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM_ROCK_SMASH, MauvilleCity_House1_EventScript_ReceivedRockSmash
	msgbox MauvilleCity_House1_Text_ImRockSmashDudeTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_HM_ROCK_SMASH
	setflag FLAG_RECEIVED_HM_ROCK_SMASH
	setflag FLAG_HIDE_ROUTE_111_ROCK_SMASH_TIP_GUY
	msgbox MauvilleCity_House1_Text_ExplainRockSmash, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House1_EventScript_ReceivedRockSmash::
	msgbox MauvilleCity_House1_Text_MonCanFlyOutOfSmashedRock, MSGBOX_DEFAULT
	release
	end

MauvilleCity_MoveRelearnersHouse_EventScript_MoveRelearner::
	lockall
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	waitmovement 0
	goto_if_set FLAG_TEMP_1, MauvilleCity_MoveRelearnersHouse_EventScript_AskTeachMove
	msgbox MauvilleCity_MoveRelearnersHouse_Text_ImTheMoveTutor, MSGBOX_DEFAULT
	setflag FLAG_TEMP_1
	goto MauvilleCity_MoveRelearnersHouse_EventScript_AskTeachMove
	end

MauvilleCity_MoveRelearnersHouse_EventScript_AskTeachMove::
	checkitem ITEM_HEART_SCALE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	msgbox MauvilleCity_MoveRelearnersHouse_Text_ThatsAHeartScaleWantMeToTeachMove, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, MauvilleCity_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	goto MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMon
	end

MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMon::
	msgbox MauvilleCity_MoveRelearnersHouse_Text_TutorWhichMon, MSGBOX_DEFAULT
	special ChooseMonForMoveRelearner
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, MauvilleCity_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_MoveRelearnersHouse_EventScript_CantTeachEgg
	goto_if_eq VAR_0x8005, 0, MauvilleCity_MoveRelearnersHouse_EventScript_NoMoveToTeachMon
	goto MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMove
	end

MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMove::
	msgbox MauvilleCity_MoveRelearnersHouse_Text_TeachWhichMove, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	waitstate
	goto_if_eq VAR_0x8004, 0, MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMon
	msgbox MauvilleCity_MoveRelearnersHouse_Text_HandedOverHeartScale, MSGBOX_DEFAULT
	removeitem ITEM_HEART_SCALE
	goto MauvilleCity_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	end

MauvilleCity_MoveRelearnersHouse_EventScript_NoMoveToTeachMon::
	msgbox MauvilleCity_MoveRelearnersHouse_Text_DontHaveMoveToTeachPokemon, MSGBOX_DEFAULT
	goto MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMon
	end

MauvilleCity_MoveRelearnersHouse_EventScript_CantTeachEgg::
	msgbox MauvilleCity_MoveRelearnersHouse_Text_CantTeachEgg, MSGBOX_DEFAULT
	goto MauvilleCity_MoveRelearnersHouse_EventScript_ChooseMon
	end

MauvilleCity_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale::
	msgbox MauvilleCity_MoveRelearnersHouse_Text_ComeBackWithHeartScale, MSGBOX_DEFAULT
	releaseall
	end

MauvilleCity_House1_Text_ImRockSmashDudeTakeThis:
	.string "Woohoo!\p"
	.string "I hear people call me the ROCK SMASH\n"
	.string "GUY, but I find that sort of degrading.\p"
	.string "I think I deserve a bit more respect,\n"
	.string "like maybe the ROCK SMASH DUDE.\p"
	.string "Woohoo!\p"
	.string "Anyways, your POKéMON look pretty\n"
	.string "strong.\p"
	.string "I like that!\n"
	.string "Here, take this HIDDEN MACHINE!$"

MauvilleCity_House1_Text_ExplainRockSmash:
	.string "That HM contains ROCK SMASH.\p"
	.string "If you come across large boulders\n"
	.string "that block your path…\p"
	.string "Well, use that HM move and smash\n"
	.string "them right out of your way!\p"
	.string "Yes, sir! Smash rocks aside, I say!\n"
	.string "Woohoo!$"

MauvilleCity_House1_Text_MonCanFlyOutOfSmashedRock:
	.string "Oh, yes, if you smash a rock, a POKéMON\n"
	.string "could come flying out of hiding.\p"
	.string "Woohoo!$"

MauvilleCity_MoveRelearnersHouse_Text_ImTheMoveTutor:
	.string "I'm the MOVE TUTOR.\p"
	.string "I know all the moves that POKéMON\n"
	.string "learn--every one of them--and I can\l"
	.string "teach POKéMON those moves.\p"
	.string "I can teach a move to a POKéMON\n"
	.string "of yours if you'd like.\p"
	.string "I'll do it for a HEART SCALE.\n"
	.string "I'm collecting those now.$"

MauvilleCity_MoveRelearnersHouse_Text_ThatsAHeartScaleWantMeToTeachMove:
	.string "Oh! That's it! That's an honest to\n"
	.string "goodness HEART SCALE!\p"
	.string "Let me guess, you want me to teach\n"
	.string "a move?$"

MauvilleCity_MoveRelearnersHouse_Text_TutorWhichMon:
	.string "Which POKéMON needs tutoring?$"

MauvilleCity_MoveRelearnersHouse_Text_TeachWhichMove:
	.string "Which move should I teach?$"

MauvilleCity_MoveRelearnersHouse_Text_DontHaveMoveToTeachPokemon:
	.string "Sorry…\p"
	.string "It doesn't appear as if I have any move\n"
	.string "I can teach that POKéMON.$"

MauvilleCity_MoveRelearnersHouse_Text_HandedOverHeartScale:
	.string "{PLAYER} handed over one HEART SCALE\n"
	.string "in exchange.$"

MauvilleCity_MoveRelearnersHouse_Text_ComeBackWithHeartScale:
	.string "If your POKéMON need to learn a move,\n"
	.string "come back with a HEART SCALE.$"

MauvilleCity_MoveRelearnersHouse_Text_CantTeachEgg:
	.string "Hunh? There isn't a single move that\n"
	.string "I can teach an EGG.$"
