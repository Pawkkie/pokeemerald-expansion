#include "global.h"
#include "test/battle.h"

AI_SINGLE_BATTLE_TEST("AI_FLAG_FIXED_ACTIONS: AI will use moves at turns specified by AI_FixedActions")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_ZIGZAGOON);
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
        PLAYER(SPECIES_ZIGZAGOON);
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

AI_SINGLE_BATTLE_TEST("AI_FLAG_FIXED_ACTIONS: AI will switch in mon after a KO specified by GetNextMonFixedActions")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_ZIGZAGOON);
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

AI_DOUBLE_BATTLE_TEST("[Doubles] AI_FLAG_FIXED_ACTIONS: AI will use moves at turns specified by AI_FixedActions")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_ZIGZAGOON);
        PLAYER(SPECIES_LINOONE);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_EXPLOSION); }
        OPPONENT(SPECIES_GYARADOS) { Moves(MOVE_EXPLOSION); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerLeft);}
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerRight); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight);}
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight);}
    }
}

AI_DOUBLE_BATTLE_TEST("[Doubles] AI_FLAG_FIXED_ACTIONS: AI will switch out at turn specified by ShouldSwitchIfFixedAction")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_ZIGZAGOON);
        PLAYER(SPECIES_LINOONE);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_EXPLOSION); }
        OPPONENT(SPECIES_GYARADOS) { Moves(MOVE_EXPLOSION); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerLeft); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerRight); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight); }
        TURN { EXPECT_SWITCH(opponentLeft, 3); EXPECT_SWITCH(opponentRight, 2); }
    }
}

AI_DOUBLE_BATTLE_TEST("[Doubles] AI_FLAG_FIXED_ACTIONS: AI will switch in mon after a KO specified by GetNextMonFixedActions")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FIXED_ACTIONS);
        PLAYER(SPECIES_ZIGZAGOON);
        PLAYER(SPECIES_LINOONE);
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_CASTFORM) { Moves(MOVE_LEER, MOVE_STEALTH_ROCK, MOVE_LICK); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_EXPLOSION); }
        OPPONENT(SPECIES_GYARADOS) { Moves(MOVE_EXPLOSION); }
        OPPONENT(SPECIES_ZIGZAGOON);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerLeft); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerRight); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_LICK, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_LICK, target: playerRight); }
        TURN { EXPECT_SWITCH(opponentLeft, 3); EXPECT_SWITCH(opponentRight, 2); }
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); EXPECT_MOVE(opponentLeft, MOVE_EXPLOSION); EXPECT_MOVE(opponentRight, MOVE_EXPLOSION); }
    } SCENE {
        MESSAGE("{PKMN} TRAINER LEAF sent out Zigzagoon!");
        MESSAGE("{PKMN} TRAINER LEAF sent out Castform!");
    }
}
