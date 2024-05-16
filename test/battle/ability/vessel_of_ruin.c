#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Vessel of Ruin reduces Sp. Atk", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_VESSEL_OF_RUIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        if (ability == ABILITY_VESSEL_OF_RUIN) {
            ABILITY_POPUP(player, ABILITY_VESSEL_OF_RUIN);
            MESSAGE("Wobbuffet's Vessel of Ruin weakened the Sp. Atk of all surrounding Pokémon!");
        }
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Vessel of Ruin does not reduce Sp. Atk if opposing mon has the same ability", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_VESSEL_OF_RUIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_VESSEL_OF_RUIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        if (ability == ABILITY_VESSEL_OF_RUIN) {
            ABILITY_POPUP(player, ABILITY_VESSEL_OF_RUIN);
            MESSAGE("Wobbuffet's Vessel of Ruin weakened the Sp. Atk of all surrounding Pokémon!");
        }
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Vessel of Ruin's message displays correctly after all battlers fainted - Player")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
        PLAYER(SPECIES_WOBBUFFET) { HP(1);}
        PLAYER(SPECIES_TING_LU);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EXPLOSION); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_RUINATION); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, opponent);
        // Everyone faints.
        MESSAGE("Go! Ting-Lu!");
        ABILITY_POPUP(player, ABILITY_VESSEL_OF_RUIN);
        MESSAGE("Ting-Lu's Vessel of Ruin weakened the Sp. Atk of all surrounding Pokémon!");
        MESSAGE("2 sent out Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Vessel of Ruin's message displays correctly after all battlers fainted - Opponent")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1);}
        OPPONENT(SPECIES_TING_LU);
    } WHEN {
        TURN { MOVE(player, MOVE_EXPLOSION); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_RUINATION); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, player);
        // Everyone faints.
        MESSAGE("Go! Wobbuffet!");
        MESSAGE("2 sent out Ting-Lu!");
        ABILITY_POPUP(opponent, ABILITY_VESSEL_OF_RUIN);
        MESSAGE("Foe Ting-Lu's Vessel of Ruin weakened the Sp. Atk of all surrounding Pokémon!");
    }
}
