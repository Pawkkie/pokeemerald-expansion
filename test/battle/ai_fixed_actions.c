#include "global.h"
#include "test/battle.h"

AI_SINGLE_BATTLE_TEST("AI_FLAG_FIXED_ACTIONS: AI will use moves at turns specified by AI_FixedActions")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_TACKLE); }
        OPPONENT(SPECIES_BISHARP) { Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_MAGIKARP) {Moves(MOVE_EXPLOSION); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_LEER); }
        TURN { EXPECT_MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_FIXED_ACTIONS: AI will switch out at turn specified by ShouldSwitchIfFixedAction")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_TACKLE); }
        OPPONENT(SPECIES_BISHARP) { Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_MAGIKARP) {Moves(MOVE_EXPLOSION); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_LEER); }
        TURN { EXPECT_MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
        TURN { EXPECT_SWITCH(opponent, 2); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_FIXED_ACTIONS: AI will switch in mon specified after a KO")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_TACKLE); }
        OPPONENT(SPECIES_BISHARP) { Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_MAGIKARP) {Moves(MOVE_EXPLOSION); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT);EXPECT_MOVE(opponent, MOVE_LEER); }
        TURN { EXPECT_MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
        TURN { EXPECT_SWITCH(opponent, 2); }
        TURN { MOVE(player, MOVE_PROTECT); EXPECT_MOVE(opponent, MOVE_EXPLOSION); }
    } SCENE {
        MESSAGE("{PKMN} TRAINER LEAF sent out Castform!");
    }
}
