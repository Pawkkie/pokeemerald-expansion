#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cud Chuw will activate Kee Berry effect again on the next turn")
{
    GIVEN {
        ASSUME(gItems[ITEM_KEE_BERRY].holdEffect == HOLD_EFFECT_KEE_BERRY);
        ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TAUROS_PALDEAN_COMBAT_BREED) { Ability(ABILITY_CUD_CHEW); Item(ITEM_KEE_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE);}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(opponent, ABILITY_CUD_CHEW);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Cud Chuw will activate Oran Berry effect again on the next turn")
{
    GIVEN {
        ASSUME(gItems[ITEM_ORAN_BERRY].holdEffect == HOLD_EFFECT_RESTORE_HP);
        ASSUME(gItems[ITEM_ORAN_BERRY].holdEffectParam == 10);
        ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
        // ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].argument == 40);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TAUROS_PALDEAN_COMBAT_BREED) { MaxHP(60); HP(60); Ability(ABILITY_CUD_CHEW); Item(ITEM_ORAN_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE);}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(opponent, ABILITY_CUD_CHEW);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, 40);
    }
}