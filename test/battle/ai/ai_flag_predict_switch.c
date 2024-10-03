#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI will predict switches with Pursuit")
{
    u32 predictionFlag;
    PARAMETRIZE { predictionFlag = 0; }
    PARAMETRIZE { predictionFlag = AI_FLAG_PREDICT_SWITCH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER | predictionFlag);
        PLAYER(SPECIES_BRONZONG) { Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_CONKELDURR) { Moves(MOVE_HAMMER_ARM); }
        OPPONENT(SPECIES_TYRANITAR) { Moves(MOVE_PURSUIT, MOVE_CRUNCH); }
    } WHEN {
        if (predictionFlag == 0)
            TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_CRUNCH); }
        else
            TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_PURSUIT); }
    }
}
AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI would switch out in Pursuit scenario")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER);
        PLAYER(SPECIES_TYRANITAR) { Moves(MOVE_PURSUIT, MOVE_CRUNCH); }
        OPPONENT(SPECIES_BRONZONG) { Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_CONKELDURR) { Moves(MOVE_HAMMER_ARM); }
    } WHEN {
        TURN { MOVE(player, MOVE_PURSUIT); EXPECT_SWITCH(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI will predict switches with Wonder Guard")
{
    u32 predictionFlag;
    PARAMETRIZE { predictionFlag = 0; }
    PARAMETRIZE { predictionFlag = AI_FLAG_PREDICT_SWITCH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER | predictionFlag);
        PLAYER(SPECIES_BRONZONG) { Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_SWELLOW) { Moves(MOVE_PECK); }
        OPPONENT(SPECIES_SHEDINJA) { Moves(MOVE_PURSUIT, MOVE_CRUNCH); }
    } WHEN {
        if (predictionFlag == 0)
            TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_CRUNCH); }
        else
            TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_PURSUIT); }
    }
}
AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI would switch out in Wonder Guard scenario")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER);
        PLAYER(SPECIES_SHEDINJA) { Moves(MOVE_PURSUIT, MOVE_CRUNCH); }
        OPPONENT(SPECIES_BRONZONG) { Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_SWELLOW) { Moves(MOVE_PECK); }
    } WHEN {
        TURN { MOVE(player, MOVE_PURSUIT); EXPECT_SWITCH(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI will score against predicted incoming mon when switch predicted")
{
    PASSES_RANDOMLY(50, 100, RNG_AI_SWITCH_HASBADODDS);
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_RHYDON].types[0] == TYPE_GROUND);
        ASSUME(gSpeciesInfo[SPECIES_PELIPPER].types[0] == TYPE_WATER);
        ASSUME(gSpeciesInfo[SPECIES_PELIPPER].types[1] == TYPE_FLYING);
        ASSUME(gMovesInfo[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
        ASSUME(gMovesInfo[MOVE_EARTHQUAKE].type == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_SMART_TRAINER | AI_FLAG_PREDICT_SWITCH);
        PLAYER(SPECIES_PELIPPER) { Moves(MOVE_EARTHQUAKE); }
        PLAYER(SPECIES_RHYDON) { Moves(MOVE_EARTHQUAKE); Ability(ABILITY_ROCK_HEAD); }
        OPPONENT(SPECIES_ELECTRODE) { Moves(MOVE_THUNDERBOLT, MOVE_ABSORB); }
    } WHEN {
        TURN { SWITCH(player, 1) ; EXPECT_MOVE(opponent, MOVE_ABSORB); }
    }
}

AI_SINGLE_BATTLE_TEST("AI_FLAG_PREDICT_SWITCH: AI would switch out in incoming mons scenario 50% of the time")
{
    PASSES_RANDOMLY(50, 100, RNG_AI_SWITCH_HASBADODDS);
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_RHYDON].types[0] == TYPE_GROUND);
        ASSUME(gSpeciesInfo[SPECIES_PELIPPER].types[0] == TYPE_WATER);
        ASSUME(gSpeciesInfo[SPECIES_PELIPPER].types[1] == TYPE_FLYING);
        ASSUME(gMovesInfo[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
        ASSUME(gMovesInfo[MOVE_EARTHQUAKE].type == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_SMART_TRAINER);
        PLAYER(SPECIES_ELECTRODE) { Moves(MOVE_THUNDERBOLT, MOVE_ABSORB); }
        OPPONENT(SPECIES_PELIPPER) { Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_RHYDON) { Moves(MOVE_EARTHQUAKE); Ability(ABILITY_ROCK_HEAD); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT) ; EXPECT_SWITCH(opponent, 1); }
    }
}
