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
