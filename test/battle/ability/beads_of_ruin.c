#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Beads of Ruin reduces Sp. Def", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_BEADS_OF_RUIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        if (ability == ABILITY_BEADS_OF_RUIN) {
            ABILITY_POPUP(player, ABILITY_BEADS_OF_RUIN);
            MESSAGE("Wobbuffet's Beads of Ruin weakened the Sp. Def of all surrounding Pokémon!");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Beads of Ruin does not reduce Sp. Def if opposing mon has the same ability", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_BEADS_OF_RUIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BEADS_OF_RUIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        if (ability == ABILITY_BEADS_OF_RUIN) {
            ABILITY_POPUP(player, ABILITY_BEADS_OF_RUIN);
            MESSAGE("Wobbuffet's Beads of Ruin weakened the Sp. Def of all surrounding Pokémon!");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Beads of Ruin's message displays correctly after all battlers fainted - Player")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
        PLAYER(SPECIES_WOBBUFFET) { HP(1);}
        PLAYER(SPECIES_CHI_YU);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EXPLOSION); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_RUINATION); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, opponent);
        // Everyone faints.
        MESSAGE("Go! Chi-Yu!");
        ABILITY_POPUP(player, ABILITY_BEADS_OF_RUIN);
        MESSAGE("Chi-Yu's Beads of Ruin weakened the Sp. Def of all surrounding Pokémon!");
        MESSAGE("2 sent out Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Beads of Ruin's message displays correctly after all battlers fainted - Opponent")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1);}
        OPPONENT(SPECIES_CHI_YU);
    } WHEN {
        TURN { MOVE(player, MOVE_EXPLOSION); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_RUINATION); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, player);
        // Everyone faints.
        MESSAGE("Go! Wobbuffet!");
        MESSAGE("2 sent out Chi-Yu!");
        ABILITY_POPUP(opponent, ABILITY_BEADS_OF_RUIN);
        MESSAGE("Foe Chi-Yu's Beads of Ruin weakened the Sp. Def of all surrounding Pokémon!");
    }
}
