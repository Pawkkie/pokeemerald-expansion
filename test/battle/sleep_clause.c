#include "global.h"
#include "test/battle.h"

AI_SINGLE_BATTLE_TEST("Sleep Clause: AI will not use sleep moves while sleep clause is active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BRELOOM) { Moves(MOVE_SPORE, MOVE_MACH_PUNCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SPORE); }
        TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); }
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Sleep moves fail when sleep clause is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
        TURN { MOVE(player, MOVE_SPORE); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            MESSAGE("Foe Wobbuffet fell asleep!");
            STATUS_ICON(opponent, sleep: TRUE);
        }
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Rest does not activate sleep clause")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_REST); }
        TURN { MOVE(player, MOVE_SPORE); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Rest can still be used when sleep clause is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
        TURN { MOVE(player, MOVE_TACKLE); SWITCH(opponent, 1); }
        TURN { MOVE(opponent, MOVE_REST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        STATUS_ICON(opponent, sleep: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REST, opponent);
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Psycho Shift'ing sleep will fail if sleep clause is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET)
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SLEEP_TALK, MOVE_PSYCHO_SHIFT); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
        TURN { MOVE(opponent, MOVE_SPORE); MOVE(player, MOVE_SLEEP_TALK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLEEP_TALK, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHO_SHIFT, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            STATUS_ICON(opponent, sleep: TRUE);
        }
        MESSAGE("But it failed!");
    } 
}

AI_SINGLE_BATTLE_TEST("Sleep Clause: AI will not use Yawn while sleep clause is active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BRELOOM) { Moves(MOVE_YAWN, MOVE_MACH_PUNCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_YAWN); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); }
        TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); }
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Yawn will fail when sleep clause is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_YAWN); }
        TURN { }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_YAWN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_YAWN, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        MESSAGE("Wobbuffet fell asleep!");
        STATUS_ICON(player, sleep: TRUE);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_YAWN, opponent);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
            STATUS_ICON(player, sleep: TRUE);
        }
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Effect Spore causes sleep 11% of the time with sleep clause active")
{
    PASSES_RANDOMLY(11, 100, RNG_EFFECT_SPORE);
    GIVEN {
        ASSUME(B_ABILITY_TRIGGER_CHANCE >= GEN_5);
        ASSUME(gMovesInfo[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BRELOOM) { Ability(ABILITY_EFFECT_SPORE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPORE); }
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_EFFECT_SPORE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        MESSAGE("Foe Breloom's Effect Spore made Wobbuffet sleep!");
        STATUS_ICON(player, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Sleep from Effect Spore will not activate sleep clause")
{
    PASSES_RANDOMLY(11, 100, RNG_EFFECT_SPORE);
    GIVEN {
        ASSUME(B_ABILITY_TRIGGER_CHANCE >= GEN_5);
        ASSUME(gMovesInfo[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BRELOOM) { Ability(ABILITY_EFFECT_SPORE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN {}
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPORE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_EFFECT_SPORE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        MESSAGE("Foe Breloom's Effect Spore made Wobbuffet sleep!");
        STATUS_ICON(player, sleep: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        MESSAGE("Wobbuffet fell asleep!");
        STATUS_ICON(player, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Moves with sleep effect chance will activate sleep clause")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RELIC_SONG); }
        TURN { MOVE(player, MOVE_SPORE); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RELIC_SONG, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        STATUS_ICON(opponent, sleep: TRUE);
        MESSAGE("Wobbuffet used Spore!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            MESSAGE("Foe Wobbuffet fell asleep!");
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Sleep Clause: Moves with sleep effect chance will still do damage when sleep clause active, but won't sleep")
{
    PASSES_RANDOMLY(100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
        TURN { MOVE(player, MOVE_RELIC_SONG); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        STATUS_ICON(opponent, sleep: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RELIC_SONG, player);
        HP_BAR(opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            MESSAGE("Foe Wobbuffet fell asleep!");
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

// I added a dire claw test because im too lazy to read what MOVE_EFFECT_DIRE_CLAW does and i suspect it could be
// different than a normal "secondary sleep chance" effect -- feel free to delete this comment also
// or even this test as it might be redundant
ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_DIRE_CLAW, MOVE_EFFECT_DIRE_CLAW) == TRUE);
}

SINGLE_BATTLE_TEST("Sleep Clause: Dire Claw cannot sleep a mon when sleep clause is active")
{
    PASSES_RANDOMLY(100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
        TURN { MOVE(player, MOVE_DIRE_CLAW); SWITCH(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        STATUS_ICON(opponent, sleep: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIRE_CLAW, player);
        HP_BAR(opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
            MESSAGE("Foe Wobbuffet fell asleep!");
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

DOUBLE_BATTLE_TEST("Sleep Clause: Dark Void can only sleep one opposing mon if sleep clause is active")
{
    // Source: https://bulbapedia.bulbagarden.net/wiki/Dark_Void_(move)
    GIVEN {
        PLAYER(SPECIES_DARKRAI);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DARK_VOID); }
        TURN { MOVE(playerLeft, MOVE_DARK_VOID); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_VOID, playerLeft);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponentLeft);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponentLeft, sleep: TRUE);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponentRight);
            STATUS_ICON(opponentRight, sleep: TRUE);
            MESSAGE("Foe Wobbuffet fell asleep!");
        }
    }
}

DOUBLE_BATTLE_TEST("Sleep Clause: G-Max Befuddle can only sleep one opposing mon if sleep clause is active")
{
    GIVEN {
        ASSUME(gMovesInfo[MOVE_G_MAX_BEFUDDLE].argument == MAX_EFFECT_EFFECT_SPORE_FOES);
        PLAYER(SPECIES_BUTTERFREE) { GigantamaxFactor(TRUE); }
        PLAYER(SPECIES_CATERPIE);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_BUG_BITE, target: opponentLeft, gimmick: GIMMICK_DYNAMAX,
               WITH_RNG(RNG_G_MAX_BEFUDDLE, STATUS1_SLEEP)); }
    } SCENE {
        MESSAGE("Butterfree used G-Max Befuddle!");
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponentLeft);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponentLeft, sleep: TRUE);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponentRight);
            STATUS_ICON(opponentRight, sleep: TRUE);
            MESSAGE("Foe Wobbuffet fell asleep!");
        }
    }
}

//honestly some of these are probably covered by just normal sleep tests 
// also possible that these should be done in doubles since its kinda just a nomral "mon woke up" test
TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon wakes up");
// something like:
    // player has wob
    // ai has wob
    // player uses spore, AI wob falls asleep
    // player celebrates until sleeping wob wakes up
    // player uses spore, AI wob falls asleep

// OR something like
    // player has wob
    // ai has wob and wob
    // player uses spore, AI wob falls asleep
    // player celebrates until sleeping wob wakes up
    // AI switches, player uses spore, AI wob 2 falls asleep


TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up with aromatherapy");
// something like:
    // player has wob
    // ai has wob and wob
    // player uses spore, AI wob falls asleep
    // AI switches, player uses spore, it fails
    // player uses spore, it fails (or any move that can succeed)
    // AI wob uses aromatherapy, cures sleeping wob
    // player uses spore, it succeeds

// might be redundant if they use the same effect
TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up with heal bell");
// something like:
    // player has wob
    // ai has wob and wob
    // player uses spore, AI wob falls asleep
    // AI switches, player uses spore, it fails
    // player uses spore, it fails (or any move that can succeed)
    // AI wob uses heal bell, cures sleeping wob
    // player uses spore, it succeeds

//I think doubles
TO_DO_BATTLE_TEST("DOUBLES: Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up with g-max sweetness whatever the fuck that is");
// something like:
    // player has wob and wob
    // ai has wob and wob
    // someone puts someone to sleep and someone on the sleeping team uses gmax sweetness, which should cure the party of status
    // can sleep again after that

// now that im writing the pseudo for this test it feels wack
TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up forcefully by a move from an opponent");
// something like:
    // player has wob
    // ai has wob and wob
    // player uses spore, AI wob falls asleep
    // AI switches, player uses spore, it fails
    // AI switches back to sleeping wob, player uses wake-up-slap/uproar, which wakes up enemy wob
    // AI switches AGAIN, player uses spore, it succeeds

// this feels like a horribly complicated scenario that would probably not happen in a million runs but unfortunately i thought of it so i will
// write it down
TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by using sleep talk into psycho shift");
// replace psycho shift with the following moves:
    // jungle healing, lunar blessing, refresh, purify (maybe?), take heart

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by the ability hydration in the rain");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by the ability natural cure");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by the ability shed skin");

TO_DO_BATTLE_TEST("DOUBLES TEST: Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by the ability healer");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by the item awakening");
// replace awakening with the following items:
    // blue flute, chesto berry, big malasada, casteliacone, full heal, full restore, heal powder, lava cookie, lum berry
    // lumiose galett, miracle berry?, old gateau, pewter Crunchies, rage candy bar, shalour sable
    // idk wtf half these items even are and id imagine a lot of them share effects so i doubt a test will be needed for all of them

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by gaining the ability Insomnia");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by gaining the ability Vital Spirit");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon faints");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping uses sleep talk -> explosion"); // holy fuck if this requires special handling im uninstalling

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping uses sleep talk -> move that KOs a mon that has used destiny bond");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping uses sleep talk -> role play/skill swap");


TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping is sent out, has trace, and traces insomnia/vital spirit");

TO_DO_BATTLE_TEST("Sleep Clause: sleep clause is deactivated when a sleeping mon is woken up by a held item, like chesto");
