#include "global.h"
#include "battle.h"
#include "constants/battle_ai.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "constants/hold_effects.h"
#include "battle_setup.h"
#include "data.h"
#include "item.h"
#include "party_menu.h"
#include "pokemon.h"
#include "random.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/item_effects.h"
#include "constants/battle_move_effects.h"
#include "constants/items.h"
#include "constants/moves.h"

// this file's functions
static bool8 HasSuperEffectiveMoveAgainstOpponents(bool8 noRng);
static bool8 FindMonWithFlagsAndSuperEffective(u16 flags, u8 moduloPercent);
static bool8 ShouldUseItem(void);
static bool32 AiExpectsToFaintPlayer(void);
static bool32 AI_ShouldHeal(u32 healAmount);
static bool32 AI_OpponentCanFaintAiWithMod(u32 healAmount);
static bool32 IsAiPartyMonOHKOBy(u32 battlerAtk, struct Pokemon *aiMon);
static bool8 IsMonHealthyEnoughToSwitch(void);
static u32 CalculateHazardDamage(void);

static bool32 IsAceMon(u32 battlerId, u32 monPartyId)
{
    if (AI_THINKING_STRUCT->aiFlags & AI_FLAG_ACE_POKEMON
        && !(gBattleStruct->forcedSwitch & gBitTable[battlerId])
        && monPartyId == CalculateEnemyPartyCount()-1)
            return TRUE;
    return FALSE;
}

void GetAIPartyIndexes(u32 battlerId, s32 *firstId, s32 *lastId)
{
    if (BATTLE_TWO_VS_ONE_OPPONENT && (battlerId & BIT_SIDE) == B_SIDE_OPPONENT)
    {
        *firstId = 0, *lastId = PARTY_SIZE;
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_TOWER_LINK_MULTI))
    {
        if ((battlerId & BIT_FLANK) == B_FLANK_LEFT)
            *firstId = 0, *lastId = PARTY_SIZE / 2;
        else
            *firstId = PARTY_SIZE / 2, *lastId = PARTY_SIZE;
    }
    else
    {
        *firstId = 0, *lastId = PARTY_SIZE;
    }
}

// Note that as many return statements as possible are INTENTIONALLY put after all of the loops;
// the function can take a max of about 0.06s to run, and this prevents the player from identifying 
// whether the mon will switch or not by seeing how long the delay is before they select a move
static bool8 HasBadOdds()
{
    //Variable initialization
	u8 opposingPosition, opposingBattler, atkType1, atkType2, defType1, defType2, effectiveness;
    s32 i, damageDealt = 0, maxDamageDealt = 0, damageTaken = 0, maxDamageTaken = 0;
    u32 aiMove, playerMove;
    bool8 getsOneShot = FALSE, hasStatusMove = FALSE, hasSuperEffectiveMove = FALSE;
	struct Pokemon *party = NULL;
	u16 typeDmg = UQ_4_12(1.0); //baseline typing damage

    // Only use this if AI_FLAG_SMART_SWITCHING is set for the trainer
    if (!(AI_THINKING_STRUCT->aiFlags & AI_FLAG_SMART_SWITCHING))
        return FALSE;

    // Won't bother configuring this for double battles
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) 
        return FALSE;
	
	opposingPosition = BATTLE_OPPOSITE(GetBattlerPosition(gActiveBattler));
    opposingBattler = GetBattlerAtPosition(opposingPosition);
	
    // Gets types of player (opposingBattler) and computer (gActiveBattler)
	atkType1 = gBattleMons[opposingBattler].type1;
	atkType2 = gBattleMons[opposingBattler].type2;
	defType1 = gBattleMons[gActiveBattler].type1;
	defType2 = gBattleMons[gActiveBattler].type2;

    // Check AI moves for damage dealt
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        aiMove = gBattleMons[gActiveBattler].moves[i];
        if (aiMove != MOVE_NONE)
        {
            // Check if mon has an "important" status move
            if (aiMove == MOVE_REFLECT || aiMove == MOVE_LIGHT_SCREEN 
            || aiMove == MOVE_SPIKES || aiMove == MOVE_TOXIC_SPIKES || aiMove == MOVE_STEALTH_ROCK || aiMove == MOVE_STICKY_WEB || aiMove == MOVE_LEECH_SEED
            || aiMove == MOVE_EXPLOSION || aiMove == MOVE_SELF_DESTRUCT 
            || aiMove == MOVE_SLEEP_POWDER || aiMove == MOVE_YAWN || aiMove == MOVE_LOVELY_KISS || aiMove == MOVE_GRASS_WHISTLE || aiMove == MOVE_HYPNOSIS 
            || aiMove == MOVE_TOXIC || aiMove == MOVE_BANEFUL_BUNKER 
            || aiMove == MOVE_WILL_O_WISP 
            || aiMove == MOVE_TRICK || aiMove == MOVE_TRICK_ROOM || aiMove== MOVE_WONDER_ROOM || aiMove ==  MOVE_PSYCHO_SHIFT || aiMove == MOVE_FAKE_OUT
            || aiMove == MOVE_STUN_SPORE || aiMove == MOVE_THUNDER_WAVE || aiMove == MOVE_NUZZLE || aiMove == MOVE_GLARE
            )
            {
                hasStatusMove = TRUE;
            }

            // Only check damage if move has power
            if (gBattleMoves[aiMove].power != 0)
            {
                // Check if mon has a super effective move
                if (AI_GetTypeEffectiveness(aiMove, gActiveBattler, opposingBattler) >= UQ_4_12(2.0))
                    hasSuperEffectiveMove = TRUE;

                // Get maximum damage mon can deal
                damageDealt = AI_CalcDamage(aiMove, gActiveBattler, opposingBattler, &effectiveness, FALSE);
                if(damageDealt > maxDamageDealt)
                    maxDamageDealt = damageDealt;
            }
        }
    }

    // Calculate type advantage
	MulModifier(&typeDmg, GetTypeModifier(atkType1, defType1));
	if (atkType2!=atkType1)
		MulModifier(&typeDmg, GetTypeModifier(atkType2, defType1));
	if (defType2!=defType1)
	{
		MulModifier(&typeDmg, GetTypeModifier(atkType1, defType2));
		if (atkType2!=atkType1)
			MulModifier(&typeDmg, GetTypeModifier(atkType2, defType2));
	}

    // Get max damage mon could take
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        playerMove = gBattleMons[opposingBattler].moves[i];
        if (playerMove != MOVE_NONE && gBattleMoves[playerMove].power != 0)
        {
            damageTaken = AI_CalcDamage(playerMove, opposingBattler, gActiveBattler, &effectiveness, FALSE);
            if (damageTaken > maxDamageTaken)
                maxDamageTaken = damageTaken;
        }
    }

    // Check if mon gets one shot
    if(maxDamageTaken > gBattleMons[gActiveBattler].hp)
    {
        getsOneShot = TRUE;
    }

    // Check if current mon can outspeed and KO in spite of bad matchup, and don't switch out if it can
    if(damageDealt > gBattleMons[opposingBattler].hp)
    {
        if (gBattleMons[gActiveBattler].speed > gBattleMons[opposingBattler].speed || gBattleMoves[aiMove].priority > 0)
            return FALSE;
    }

    // If we don't have any other viable options, don't switch out
    if (AI_THINKING_STRUCT->mostSuitableMonId == PARTY_SIZE)
        return FALSE;

    // Start assessing whether or not mon has bad odds
    // Jump straight to swtiching out in cases where mon gets OHKO'd
    if (((getsOneShot && gBattleMons[opposingBattler].speed > gBattleMons[gActiveBattler].speed) // If the player OHKOs and outspeeds OR OHKOs, doesn't outspeed but isn't 2HKO'd
            || (getsOneShot && gBattleMons[opposingBattler].speed <= gBattleMons[gActiveBattler].speed && maxDamageDealt < gBattleMons[opposingBattler].hp / 2)) 
        && (gBattleMons[gActiveBattler].hp >= gBattleMons[gActiveBattler].maxHP/2 // And the current mon has at least 1/2 their HP, or 1/4 HP and Regenerator
            || (gBattleMons[gActiveBattler].ability == ABILITY_REGENERATOR 
            && gBattleMons[gActiveBattler].hp >= gBattleMons[gActiveBattler].maxHP/4))) 
    {
        // 50% chance to stay in regardless
        if (Random() % 2 == 0) 
            return FALSE;

        // Switch mon out
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE; 
        BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
        return TRUE;
    }

    // General bad type matchups have more wiggle room
	if (typeDmg>=UQ_4_12(2.0)) // If the player has at least a 2x type advantage
	{
		if (!hasSuperEffectiveMove // If the AI doesn't have a super effective move
		&& (gBattleMons[gActiveBattler].hp >= gBattleMons[gActiveBattler].maxHP/2 // And the current mon has at least 1/2 their HP, or 1/4 HP and Regenerator
            || (gBattleMons[gActiveBattler].ability == ABILITY_REGENERATOR 
            && gBattleMons[gActiveBattler].hp >= gBattleMons[gActiveBattler].maxHP/4))) 
		{
            // Then check if they have an important status move, which is worth using even in a bad matchup
            if(hasStatusMove)
                return FALSE;

            // 50% chance to stay in regardless
            if (Random() % 2 == 0) 
                return FALSE;

            // Switch mon out
			*(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE; 
			BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
			return TRUE;
		}
	}
	return FALSE;
}

static bool8 ShouldSwitchIfAllBadMoves(void)
{
    if (gBattleResources->ai->switchMon)
    {
        gBattleResources->ai->switchMon = 0;
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool8 ShouldSwitchIfWonderGuard(void)
{
    u8 opposingPosition;
    u8 opposingBattler;
    s32 i, j;
    s32 firstId;
    s32 lastId; // + 1
    struct Pokemon *party = NULL;
    u16 move;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        return FALSE;

    opposingPosition = BATTLE_OPPOSITE(GetBattlerPosition(gActiveBattler));

    if (GetBattlerAbility(GetBattlerAtPosition(opposingPosition)) != ABILITY_WONDER_GUARD)
        return FALSE;

    // Check if Pokemon has a super effective move.
    for (opposingBattler = GetBattlerAtPosition(opposingPosition), i = 0; i < MAX_MON_MOVES; i++)
    {
        move = gBattleMons[gActiveBattler].moves[i];
        if (move != MOVE_NONE)
        {
            if (AI_GetTypeEffectiveness(move, gActiveBattler, opposingBattler) >= UQ_4_12(2.0))
                return FALSE;
        }
    }

    // Get party information.
    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    // Find a Pokemon in the party that has a super effective move.
    for (i = firstId; i < lastId; i++)
    {
        if (!IsValidForBattle(&party[i]))
            continue;
        if (i == gBattlerPartyIndexes[gActiveBattler])
            continue;
        if (IsAceMon(gActiveBattler, i))
            continue;

        for (opposingBattler = GetBattlerAtPosition(opposingPosition), j = 0; j < MAX_MON_MOVES; j++)
        {
            move = GetMonData(&party[i], MON_DATA_MOVE1 + j);
            if (move != MOVE_NONE)
            {
                if (AI_GetTypeEffectiveness(move, gActiveBattler, opposingBattler) >= UQ_4_12(2.0) && Random() % 3 < 2)
                {
                    // We found a mon.
                    *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = i;
                    BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);
                    return TRUE;
                }
            }
        }
    }

    return FALSE; // There is not a single Pokemon in the party that has a super effective move against a mon with Wonder Guard.
}

static bool8 FindMonThatAbsorbsOpponentsMove(void)
{
    u8 battlerIn1, battlerIn2;
    u8 numAbsorbingAbilities = 0; 
    u16 absorbingTypeAbilities[3]; // Array size is maximum number of absorbing abilities for a single type
    s32 firstId;
    s32 lastId; // + 1
    struct Pokemon *party;
    s32 i, j;

    if (HasSuperEffectiveMoveAgainstOpponents(TRUE) && Random() % 2 == 0)
        return FALSE;
    if (gLastLandedMoves[gActiveBattler] == MOVE_NONE)
        return FALSE;
    if (gLastLandedMoves[gActiveBattler] == MOVE_UNAVAILABLE)
        return FALSE;
    if (IS_MOVE_STATUS(gLastLandedMoves[gActiveBattler]))
        return FALSE;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        battlerIn1 = gActiveBattler;
        if (gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))])
            battlerIn2 = gActiveBattler;
        else
            battlerIn2 = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)));
    }
    else
    {
        battlerIn1 = gActiveBattler;
        battlerIn2 = gActiveBattler;
    }

    // Create an array of possible absorb abilities so the AI considers all of them
    if (gBattleMoves[gLastLandedMoves[gActiveBattler]].type == TYPE_FIRE)
    {
        absorbingTypeAbilities[0] = ABILITY_FLASH_FIRE;
        numAbsorbingAbilities = 1;
    }
    else if (gBattleMoves[gLastLandedMoves[gActiveBattler]].type == TYPE_WATER)
    {
        absorbingTypeAbilities[0] = ABILITY_WATER_ABSORB;
        absorbingTypeAbilities[1] = ABILITY_STORM_DRAIN;
        absorbingTypeAbilities[2] = ABILITY_DRY_SKIN;
        numAbsorbingAbilities = 3;
    }
    else if (gBattleMoves[gLastLandedMoves[gActiveBattler]].type == TYPE_ELECTRIC)
    {
        absorbingTypeAbilities[0] = ABILITY_VOLT_ABSORB;
        absorbingTypeAbilities[1] = ABILITY_MOTOR_DRIVE;
        absorbingTypeAbilities[2] = ABILITY_LIGHTNING_ROD;
        numAbsorbingAbilities = 3;
    }
    else if (gBattleMoves[gLastLandedMoves[gActiveBattler]].type == TYPE_GRASS)
    {
        absorbingTypeAbilities[0] = ABILITY_SAP_SIPPER;
        numAbsorbingAbilities = 1;
    }
    else
        return FALSE;

    // Check current mon for all absorbing abilities
    for (i = 0; i < numAbsorbingAbilities; i++)
    {
        if (AI_DATA->abilities[gActiveBattler] == absorbingTypeAbilities[i])
        return FALSE;
    }

    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    for (i = firstId; i < lastId; i++)
    {
        u16 monAbility;

        if (!IsValidForBattle(&party[i]))
            continue;
        if (i == gBattlerPartyIndexes[battlerIn1])
            continue;
        if (i == gBattlerPartyIndexes[battlerIn2])
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn1))
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn2))
            continue;
        if (IsAceMon(gActiveBattler, i))
            continue;

        monAbility = GetMonAbility(&party[i]);

        for (j = 0; j < numAbsorbingAbilities; j++)
        {
            if (absorbingTypeAbilities[j] == monAbility)
            {
                // we found a mon.
                *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = i;
                BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
                return TRUE;
            }
        }
    }
    return FALSE;
}

static bool8 ShouldSwitchIfGameStatePrompt(void)
{
    bool8 switchMon = FALSE;
    u16 monAbility = AI_DATA->abilities[gActiveBattler];
    u16 holdEffect = AI_DATA->holdEffects[gActiveBattler];
    u8 opposingPosition = BATTLE_OPPOSITE(GetBattlerPosition(gActiveBattler));
    u8 opposingBattler = GetBattlerAtPosition(opposingPosition);
    s32 moduloChance = 4; //25% Chance Default
    s32 chanceReducer = 1; //No Reduce default. Increase to reduce
    s32 firstId;
    s32 lastId;
    s32 i;
    struct Pokemon *party;


    if (AnyStatIsRaised(gActiveBattler))
        chanceReducer = 5; // Reduce switchout probability by factor of 5 if setup

    //Perish Song
    if (gStatuses3[gActiveBattler] & STATUS3_PERISH_SONG
        && gDisableStructs[gActiveBattler].perishSongTimer == 0
        && monAbility != ABILITY_SOUNDPROOF)
        switchMon = TRUE;

    if (AI_THINKING_STRUCT->aiFlags & AI_FLAG_SMART_SWITCHING)
    {
        //Yawn
        if (gStatuses3[gActiveBattler] & STATUS3_YAWN
            && AI_CanSleep(gActiveBattler, monAbility)
            && gBattleMons[gActiveBattler].hp > gBattleMons[gActiveBattler].maxHP / 3)
        {
            switchMon = TRUE;

            //Double Battles
            //Check if partner can prevent sleep
            if (IsDoubleBattle())
            {
                if (IsBattlerAlive(BATTLE_PARTNER(gActiveBattler))
                    && (GetAIChosenMove(BATTLE_PARTNER(gActiveBattler)) == MOVE_UPROAR)
                    )
                    switchMon = FALSE;

                if (IsBattlerAlive(BATTLE_PARTNER(gActiveBattler))
                    && (gBattleMoves[AI_DATA->partnerMove].effect == EFFECT_MISTY_TERRAIN
                        || gBattleMoves[AI_DATA->partnerMove].effect == EFFECT_ELECTRIC_TERRAIN)
                    && IsBattlerGrounded(gActiveBattler)
                    )
                    switchMon = FALSE;

                if (*(gBattleStruct->AI_monToSwitchIntoId + BATTLE_PARTNER(gActiveBattler)) != PARTY_SIZE) //Partner is switching
                    {
                        GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

                        if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
                            party = gPlayerParty;

                        for (i = firstId; i < lastId; i++)
                        {
                            if (IsAceMon(gActiveBattler, i))
                                continue;

                            //Look for mon in party that is able to be switched into and has ability that sets terrain
                            if (IsValidForBattle(&party[i])
                                && i != gBattlerPartyIndexes[gActiveBattler]
                                && i != gBattlerPartyIndexes[BATTLE_PARTNER(gActiveBattler)]
                                && IsBattlerGrounded(gActiveBattler)
                                && (GetMonAbility(&party[i]) == ABILITY_MISTY_SURGE
                                    || GetMonAbility(&party[i]) == ABILITY_ELECTRIC_SURGE)) //Ally has Misty or Electric Surge
                                {
                                    *(gBattleStruct->AI_monToSwitchIntoId + BATTLE_PARTNER(gActiveBattler)) = i;
                                    BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);
                                    switchMon = FALSE;
                                    break;
                                }
                        }
                    }
            }

            //Check if Active Pokemon can KO opponent instead of switching
            //Will still fall asleep, but take out opposing Pokemon first
            if (AiExpectsToFaintPlayer())
                switchMon = FALSE;

            //Checks to see if active Pokemon can do something against sleep
            if ((monAbility == ABILITY_NATURAL_CURE
                || monAbility == ABILITY_SHED_SKIN
                || monAbility == ABILITY_EARLY_BIRD)
                || holdEffect == (HOLD_EFFECT_CURE_SLP | HOLD_EFFECT_CURE_STATUS)
                || HasMove(gActiveBattler, MOVE_SLEEP_TALK)
                || (HasMoveEffect(gActiveBattler, MOVE_SNORE) && AI_GetTypeEffectiveness(MOVE_SNORE, gActiveBattler, opposingBattler) >= UQ_4_12(1.0))
                || (IsBattlerGrounded(gActiveBattler)
                    && (HasMove(gActiveBattler, MOVE_MISTY_TERRAIN) || HasMove(gActiveBattler, MOVE_ELECTRIC_TERRAIN)))
                )
                switchMon = FALSE;

            //Check if Active Pokemon evasion boosted and might be able to dodge until awake
            if (gBattleMons[gActiveBattler].statStages[STAT_EVASION] > (DEFAULT_STAT_STAGE + 3)
                && AI_DATA->abilities[opposingBattler] != ABILITY_UNAWARE
                && AI_DATA->abilities[opposingBattler] != ABILITY_KEEN_EYE
                && !(gBattleMons[gActiveBattler].status2 & STATUS2_FORESIGHT)
                && !(gStatuses3[gActiveBattler] & STATUS3_MIRACLE_EYED))
                switchMon = FALSE;

        }

        //Secondary Damage
        if (monAbility != ABILITY_MAGIC_GUARD
            && !AiExpectsToFaintPlayer())
        {
            //Toxic
            moduloChance = 2; //50%
            if (((gBattleMons[gActiveBattler].status1 & STATUS1_TOXIC_COUNTER) >= STATUS1_TOXIC_TURN(2))
                && gBattleMons[gActiveBattler].hp >= (gBattleMons[gActiveBattler].maxHP / 3)
                && (Random() % (moduloChance*chanceReducer)) == 0)
                switchMon = TRUE;

            //Cursed
            moduloChance = 2; //50%
            if (gBattleMons[gActiveBattler].status2 & STATUS2_CURSED
                && (Random() % (moduloChance*chanceReducer)) == 0)
                switchMon = TRUE;

            //Nightmare
            moduloChance = 3; //33.3%
            if (gBattleMons[gActiveBattler].status2 & STATUS2_NIGHTMARE
                && (Random() % (moduloChance*chanceReducer)) == 0)
                switchMon = TRUE;

            //Leech Seed
            moduloChance = 4; //25%
            if (gStatuses3[gActiveBattler] & STATUS3_LEECHSEED
                && (Random() % (moduloChance*chanceReducer)) == 0)
                switchMon = TRUE;
        }

        //Infatuation
        if (gBattleMons[gActiveBattler].status2 & STATUS2_INFATUATION
            && !AiExpectsToFaintPlayer())
            switchMon = TRUE;

        //Todo
        //Pass Wish Heal

        //Semi-Invulnerable
        if (gStatuses3[opposingBattler] & STATUS3_SEMI_INVULNERABLE)
        {
            if (FindMonThatAbsorbsOpponentsMove()) //If find absorber default to switch
                switchMon = TRUE;
            if (!AI_OpponentCanFaintAiWithMod(0)
                && AnyStatIsRaised(gActiveBattler))
                switchMon = FALSE;
            if (AiExpectsToFaintPlayer()
                && !WillAIStrikeFirst()
                && !AI_OpponentCanFaintAiWithMod(0))
                switchMon = FALSE;
        }
    }

    if (switchMon)
    {
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool8 ShouldSwitchIfAbilityBenefit()
{
    s32 monToSwitchId;
    s32 moduloChance = 4; //25% Chance Default
    s32 chanceReducer = 1; //No Reduce default. Increase to reduce
    u8 battlerId = GetBattlerPosition(gActiveBattler);

    if (AnyStatIsRaised(battlerId))
        chanceReducer = 5; // Reduce switchout probability by factor of 5 if setup

    //Check if ability is blocked
    if (gStatuses3[gActiveBattler] & STATUS3_GASTRO_ACID
        ||IsNeutralizingGasOnField())
        return FALSE;

    switch(AI_DATA->abilities[gActiveBattler]) {
        case ABILITY_NATURAL_CURE:
            moduloChance = 4; //25%
            //Attempt to cure bad ailment
            if (gBattleMons[gActiveBattler].status1 & (STATUS1_SLEEP | STATUS1_FREEZE | STATUS1_TOXIC_POISON)
                && AI_THINKING_STRUCT->mostSuitableMonId != PARTY_SIZE)
                break;
            //Attempt to cure lesser ailment
            if ((gBattleMons[gActiveBattler].status1 & STATUS1_ANY)
                && (gBattleMons[gActiveBattler].hp >= gBattleMons[gActiveBattler].maxHP / 2)
                && AI_THINKING_STRUCT->mostSuitableMonId != PARTY_SIZE
                && Random() % (moduloChance*chanceReducer) == 0)
                break;

            return FALSE;

        case ABILITY_REGENERATOR:
            moduloChance = 2; //50%
            //Don't switch if ailment
            if (gBattleMons[gActiveBattler].status1 & STATUS1_ANY)
                return FALSE;
            if ((gBattleMons[gActiveBattler].hp <= ((gBattleMons[gActiveBattler].maxHP * 2) / 3))
                 && AI_THINKING_STRUCT->mostSuitableMonId != PARTY_SIZE
                 && Random() % (moduloChance*chanceReducer) == 0)
                break;

            return FALSE;

        default:
            return FALSE;
    }

    *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
    BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);

    return TRUE;
}

static bool8 HasSuperEffectiveMoveAgainstOpponents(bool8 noRng)
{
    u8 opposingPosition;
    u8 opposingBattler;
    s32 i;
    u16 move;

    opposingPosition = BATTLE_OPPOSITE(GetBattlerPosition(gActiveBattler));
    opposingBattler = GetBattlerAtPosition(opposingPosition);

    if (!(gAbsentBattlerFlags & gBitTable[opposingBattler]))
    {
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            move = gBattleMons[gActiveBattler].moves[i];
            if (move == MOVE_NONE)
                continue;

            if (AI_GetTypeEffectiveness(move, gActiveBattler, opposingBattler) >= UQ_4_12(2.0))
            {
                if (noRng)
                    return TRUE;
                if (Random() % 10 != 0)
                    return TRUE;
            }
        }
    }
    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        return FALSE;

    opposingBattler = GetBattlerAtPosition(BATTLE_PARTNER(opposingPosition));

    if (!(gAbsentBattlerFlags & gBitTable[opposingBattler]))
    {
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            move = gBattleMons[gActiveBattler].moves[i];
            if (move == MOVE_NONE)
                continue;

            if (AI_GetTypeEffectiveness(move, gActiveBattler, opposingBattler) >= UQ_4_12(2.0))
            {
                if (noRng)
                    return TRUE;
                if (Random() % 10 != 0)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static bool8 AreStatsRaised(void)
{
    u8 buffedStatsValue = 0;
    s32 i;

    for (i = 0; i < NUM_BATTLE_STATS; i++)
    {
        if (gBattleMons[gActiveBattler].statStages[i] > DEFAULT_STAT_STAGE)
            buffedStatsValue += gBattleMons[gActiveBattler].statStages[i] - DEFAULT_STAT_STAGE;
    }

    return (buffedStatsValue > 3);
}

static bool8 FindMonWithFlagsAndSuperEffective(u16 flags, u8 moduloPercent)
{
    u8 battlerIn1, battlerIn2;
    s32 firstId;
    s32 lastId; // + 1
    struct Pokemon *party;
    s32 i, j;
    u16 move;

    if (gLastLandedMoves[gActiveBattler] == MOVE_NONE)
        return FALSE;
    if (gLastLandedMoves[gActiveBattler] == MOVE_UNAVAILABLE)
        return FALSE;
    if (gLastHitBy[gActiveBattler] == 0xFF)
        return FALSE;
    if (IS_MOVE_STATUS(gLastLandedMoves[gActiveBattler]))
        return FALSE;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        battlerIn1 = gActiveBattler;
        if (gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))])
            battlerIn2 = gActiveBattler;
        else
            battlerIn2 = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)));
    }
    else
    {
        battlerIn1 = gActiveBattler;
        battlerIn2 = gActiveBattler;
    }

    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    for (i = firstId; i < lastId; i++)
    {
        u16 species, monAbility;

        if (!IsValidForBattle(&party[i]))
            continue;
        if (i == gBattlerPartyIndexes[battlerIn1])
            continue;
        if (i == gBattlerPartyIndexes[battlerIn2])
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn1))
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn2))
            continue;
        if (IsAceMon(gActiveBattler, i))
            continue;

        species = GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG);
        monAbility = GetMonAbility(&party[i]);

        CalcPartyMonTypeEffectivenessMultiplier(gLastLandedMoves[gActiveBattler], species, monAbility);
        if (gMoveResultFlags & flags)
        {
            battlerIn1 = gLastHitBy[gActiveBattler];

            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                move = GetMonData(&party[i], MON_DATA_MOVE1 + j);
                if (move == 0)
                    continue;

                if (AI_GetTypeEffectiveness(move, gActiveBattler, battlerIn1) >= UQ_4_12(2.0) && Random() % moduloPercent == 0)
                {
                    *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = i;
                    BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_SWITCH, 0);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

static bool8 ShouldSwitchIfEncored(void)
{
    if (gDisableStructs[gActiveBattler].encoredMove == MOVE_NONE)
        return FALSE;

    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_DOESNT_AFFECT_FOE, 1))
        return TRUE;
    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_NOT_VERY_EFFECTIVE, 1))
        return TRUE;

    if (Random() & 1)
    {
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
        return TRUE;
    }

    return FALSE;
}

static bool8 ShouldSwitchIfNaturalCure(void)
{
    if (!(gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP))
        return FALSE;
    if (AI_GetAbility(gActiveBattler) != ABILITY_NATURAL_CURE)
        return FALSE;

    if ((gLastLandedMoves[gActiveBattler] == 0 || gLastLandedMoves[gActiveBattler] == 0xFFFF) && Random() & 1)
    {
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
        return TRUE;
    }
    else if (gBattleMoves[gLastLandedMoves[gActiveBattler]].power == 0 && Random() & 1)
    {
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
        return TRUE;
    }

    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_DOESNT_AFFECT_FOE, 1))
        return TRUE;
    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_NOT_VERY_EFFECTIVE, 1))
        return TRUE;

    if (Random() & 1)
    {
        *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
        BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
        return TRUE;
    }

    return FALSE;
}

// AI should switch if it's become setup fodder and has something better to switch to
static bool8 AreAttackingStatsLowered(void)
{
    // Mon is physical attacker and its attack isn't below -1, don't switch
    if (gBattleMons[gActiveBattler].statStages[MON_DATA_ATK - MON_DATA_MAX_HP] > DEFAULT_STAT_STAGE - 2)
    {
        if (gBattleMons[gActiveBattler].attack >= gBattleMons[gActiveBattler].spAttack)
            return FALSE;
    }

    // Mon is special attacker and its special attack isn't below -1, don't switch
    if (gBattleMons[gActiveBattler].statStages[MON_DATA_SPATK - MON_DATA_MAX_HP] > DEFAULT_STAT_STAGE - 2)
    {
        if (gBattleMons[gActiveBattler].spAttack >= gBattleMons[gActiveBattler].attack)
            return FALSE;
    }
    
    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_DOESNT_AFFECT_FOE, 1))
        return TRUE;
    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_NOT_VERY_EFFECTIVE, 1))
        return TRUE;

    *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = PARTY_SIZE;
    BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
    return TRUE;
}

bool32 ShouldSwitch()
{
    u8 battlerIn1, battlerIn2;
    s32 firstId;
    s32 lastId; // + 1
    struct Pokemon *party;
    s32 i;
    s32 availableToSwitch;
    bool32 hasAceMon = FALSE;

    if (gBattleMons[gActiveBattler].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION))
        return FALSE;
    if (gStatuses3[gActiveBattler] & STATUS3_ROOTED)
        return FALSE;
    if (IsAbilityPreventingEscape(gActiveBattler))
        return FALSE;
    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        return FALSE;

    availableToSwitch = 0;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        battlerIn1 = gActiveBattler;
        if (gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))])
            battlerIn2 = gActiveBattler;
        else
            battlerIn2 = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)));
    }
    else
    {
        battlerIn1 = gActiveBattler;
        battlerIn2 = gActiveBattler;
    }

    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    for (i = firstId; i < lastId; i++)
    {
        if (!IsValidForBattle(&party[i]))
            continue;
        if (i == gBattlerPartyIndexes[battlerIn1])
            continue;
        if (i == gBattlerPartyIndexes[battlerIn2])
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn1))
            continue;
        if (i == *(gBattleStruct->monToSwitchIntoId + battlerIn2))
            continue;
        if (IsAceMon(gActiveBattler, i))
        {
            hasAceMon = TRUE;
            continue;
        }

        availableToSwitch++;
    }

    if (availableToSwitch == 0)
    {
        if (hasAceMon) // If the ace mon is the only available mon, use it
            availableToSwitch++;
        else
            return FALSE;
    }

    //NOTE: The sequence of the below functions matter! Do not change unless you have carefully considered the outcome.
    //Since the order is sequencial, and some of these functions prompt switch to specific party members.

    //These Functions can prompt switch to specific party members
    if (ShouldSwitchIfWonderGuard())
        return TRUE;
    if (ShouldSwitchIfGameStatePrompt())
        return TRUE;
    if (FindMonThatAbsorbsOpponentsMove())
        return TRUE;

    // Ported from Inclement Emerald
    if (!IsMonHealthyEnoughToSwitch())
        return FALSE;
    if (ShouldSwitchIfEncored())
        return TRUE;
    if (ShouldSwitchIfNaturalCure())
        return TRUE;

    //These Functions can prompt switch to generic pary members
    if (ShouldSwitchIfAllBadMoves())
        return TRUE;
    if (ShouldSwitchIfAbilityBenefit())
        return TRUE;
    if (HasBadOdds())
		return TRUE;

    //Removing switch capabilites under specific conditions
    //These Functions prevent the "FindMonWithFlagsAndSuperEffective" from getting out of hand.
    if (HasSuperEffectiveMoveAgainstOpponents(FALSE))
        return FALSE;
    if (AreStatsRaised())
        return FALSE;

    // Ported from Inclement Emerald
    if (AreAttackingStatsLowered())
        return TRUE;

    //Default Function
    //Can prompt switch if AI has a pokemon in party that resists current opponent & has super effective move
    if (FindMonWithFlagsAndSuperEffective(MOVE_RESULT_DOESNT_AFFECT_FOE, 2)
        || FindMonWithFlagsAndSuperEffective(MOVE_RESULT_NOT_VERY_EFFECTIVE, 3))
        return TRUE;

    return FALSE;
}

void AI_TrySwitchOrUseItem()
{
    struct Pokemon *party;
    u8 battlerIn1, battlerIn2;
    s32 firstId;
    s32 lastId; // + 1
    u8 battlerIdentity = GetBattlerPosition(gActiveBattler);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        if (ShouldSwitch())
        {
            if (*(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) == PARTY_SIZE)
            {
                u8 monToSwitchId = AI_THINKING_STRUCT->mostSuitableMonId;
                if (monToSwitchId == PARTY_SIZE)
                {
                    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
                    {
                        battlerIn1 = GetBattlerAtPosition(battlerIdentity);
                        battlerIn2 = battlerIn1;
                    }
                    else
                    {
                        battlerIn1 = GetBattlerAtPosition(battlerIdentity);
                        battlerIn2 = GetBattlerAtPosition(BATTLE_PARTNER(battlerIdentity));
                    }

                    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

                    for (monToSwitchId = (lastId-1); monToSwitchId >= firstId; monToSwitchId--)
                    {
                        if (!IsValidForBattle(&party[monToSwitchId]))
                            continue;
                        if (monToSwitchId == gBattlerPartyIndexes[battlerIn1])
                            continue;
                        if (monToSwitchId == gBattlerPartyIndexes[battlerIn2])
                            continue;
                        if (monToSwitchId == *(gBattleStruct->monToSwitchIntoId + battlerIn1))
                            continue;
                        if (monToSwitchId == *(gBattleStruct->monToSwitchIntoId + battlerIn2))
                            continue;
                        if (IsAceMon(gActiveBattler, monToSwitchId))
                            continue;

                        break;
                    }
                }

                *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler) = monToSwitchId;
            }

            *(gBattleStruct->monToSwitchIntoId + gActiveBattler) = *(gBattleStruct->AI_monToSwitchIntoId + gActiveBattler);
            return;
        }
        else if (ShouldUseItem())
        {
            return;
        }
    }

    BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_USE_MOVE, BATTLE_OPPOSITE(gActiveBattler) << 8);
}

// If there are two(or more) mons to choose from, always choose one that has baton pass
// as most often it can't do much on its own.
static u32 GetBestMonBatonPass(struct Pokemon *party, int firstId, int lastId, u8 invalidMons, int aliveCount, u32 opposingBattler)
{
    int i, j, bits = 0;

    for (i = firstId; i < lastId; i++)
    {
        if (invalidMons & gBitTable[i])
            continue;
        if (IsAiPartyMonOHKOBy(opposingBattler, &party[i]))
            continue;

        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (GetMonData(&party[i], MON_DATA_MOVE1 + j, NULL) == MOVE_BATON_PASS)
            {
                bits |= gBitTable[i];
                break;
            }
        }
    }

    if ((aliveCount == 2 || (aliveCount > 2 && Random() % 3 == 0)) && bits)
    {
        do
        {
            i = (Random() % (lastId - firstId)) + firstId;
        } while (!(bits & gBitTable[i]));
        return i;
    }

    return PARTY_SIZE;
}

static u32 GetBestMonTypeMatchup(struct Pokemon *party, int firstId, int lastId, u8 invalidMons, u32 opposingBattler)
{
    int i, bits = 0;
    bool8 checkedAllMonsForSEMoves = FALSE;
    u16 bestResist;
    int bestMonId;
    u16 species;
    u16 typeEffectiveness;
    u8 atkType1;
    u8 atkType2;
    u8 defType1;
    u8 defType2;
    u32 aiMove;

    while (bits != 0x3F) // All mons were checked.
    {
        bestResist = UQ_4_12(1.0);
        bestMonId = PARTY_SIZE;
        // Find the mon whose type is the most suitable defensively.
        for (i = firstId; i < lastId; i++)
        {
            if (!(gBitTable[i] & invalidMons) && !(gBitTable[i] & bits))
            {
                species = GetMonData(&party[i], MON_DATA_SPECIES);
                typeEffectiveness = UQ_4_12(1.0);

                atkType1 = gBattleMons[opposingBattler].type1;
                atkType2 = gBattleMons[opposingBattler].type2;
                defType1 = gSpeciesInfo[species].types[0];
                defType2 = gSpeciesInfo[species].types[1];

                if (IsAiPartyMonOHKOBy(opposingBattler, &party[i]))
                    continue;

                MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType1))); // Multiply type effectiveness by a factor depending on type matchup
                if (atkType2 != atkType1)
                    MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType1)));
                if (defType2 != defType1)
                {
                    MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType2)));
                    if (atkType2 != atkType1)
                        MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType2)));
                }
                if (typeEffectiveness < bestResist)
                {
                    bestResist = typeEffectiveness;
                    bestMonId = i;
                }
            }
        }
        // Have type matchup mon
        if (bestMonId != PARTY_SIZE)
        {
            // Check if it has a super effective move
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                aiMove = GetMonData(&party[bestMonId], MON_DATA_MOVE1 + i);
                if (aiMove != MOVE_NONE && AI_GetTypeEffectiveness(aiMove, gActiveBattler, opposingBattler) >= UQ_4_12(2.0))
                    break;
            }
            // If it has a super effective move or we've already checked other options, it's the best mon
            if (i != MAX_MON_MOVES || checkedAllMonsForSEMoves)
                return bestMonId; 
            // If it doesn't, keep looking
            bits |= gBitTable[bestMonId];
            
            if (bits == 0x3F && !checkedAllMonsForSEMoves)
            {
                bits = 0;
                checkedAllMonsForSEMoves = TRUE;
            }
        // Do not have a defensive mon
        }
        else
        {
            return PARTY_SIZE;
            bits = 0x3F; // No viable mon to switch.
        }
    }
    return PARTY_SIZE;
}

static u32 GetBestMonDefensive(struct Pokemon *party, int firstId, int lastId, u8 invalidMons, u32 opposingBattler)
{
    // Most defensive
    int defensiveMonId = PARTY_SIZE;

    // Variables
    int i, j, bits = 0;
    s32 damageTaken = 0;
    s32 maxDamageTaken = 0;
    s32 maxHitsToKO = 0;
    s32 hitsToKO = 0;
    s32 hp = 0;
    s32 hitKOThreshold = 3; // 3HKO threshold to exceed
    u32 playerMove;

    // Iterate through mons
    for (i = firstId; i < lastId; i++)
    {
        if (!(gBitTable[i] & invalidMons))
        {
            maxDamageTaken = 0;
            // Find most damaging move player could use
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                playerMove = gBattleMons[opposingBattler].moves[j];
                damageTaken = AI_CalcPartyMonDamage(playerMove, opposingBattler, gActiveBattler, &party[i], FALSE);
                if (damageTaken > maxDamageTaken)
                    maxDamageTaken = damageTaken;
            }

            // Get max number of hits to KO mon
            hp = GetMonData(&party[i], MON_DATA_HP);
            hitsToKO = GetNoOfHitsToKO(maxDamageTaken, hp);
            if(hitsToKO > maxHitsToKO)
            {
                maxHitsToKO = hitsToKO;
                defensiveMonId = i;
            }
        }
    }
    
    // Return most defensive mon if isn't 3HKO'd
    if (defensiveMonId != PARTY_SIZE)
    {
        if(maxHitsToKO > hitKOThreshold)
        {
            return defensiveMonId;
        }
        else
        {
            return PARTY_SIZE;
        }
    }
    else
    {
        return PARTY_SIZE;
    }
}

static u32 GetBestMonRevengeKiller(struct Pokemon *party, int firstId, int lastId, u8 invalidMons, u32 opposingBattler)
{
    // Revenge killer
    int revengeKillerId = PARTY_SIZE;
    int slowRevengeKillerId = PARTY_SIZE;
    int fastThreatenId = PARTY_SIZE;
    int slowThreatenId = PARTY_SIZE;

    // Variables
    int i, j = 0;
    s32 maxDamageTaken, damageTaken, hitsToKO, damageDealt, aiMonSpeed = 0;
    s32 playerMonSpeed = gBattleMons[opposingBattler].speed;
    u32 aiMove, playerMove;
        // Iterate through mons
    for (i = firstId; i < lastId; i++)
    {
        if (!(gBitTable[i] & invalidMons))
        {
            maxDamageTaken = 0;
            aiMonSpeed = GetMonData(&party[i], MON_DATA_SPEED);
            // Find most damaging move player could use
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                playerMove = gBattleMons[opposingBattler].moves[j];
                damageTaken = AI_CalcPartyMonDamage(playerMove, opposingBattler, gActiveBattler, &party[i], FALSE);
                if (damageTaken > maxDamageTaken)
                    maxDamageTaken = damageTaken;
            }
            // Get max number of hits to KO mon
            hitsToKO = GetNoOfHitsToKO(maxDamageTaken, GetMonData(&party[i], MON_DATA_HP));

            // Check if current mon can revenge kill
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                aiMove = GetMonData(&party[i], MON_DATA_MOVE1 + j);
                damageDealt = AI_CalcPartyMonDamage(aiMove, gActiveBattler, opposingBattler, &party[i], TRUE);
                // If mon can one shot
                if(damageDealt > gBattleMons[opposingBattler].hp)
                {
                    // If mon is faster
                    if (aiMonSpeed > playerMonSpeed || gBattleMoves[aiMove].priority > 0)
                    {
                        // We have a revenge killer
                        revengeKillerId = i;
                    }

                    // If mon is slower
                    else
                    {
                        // If mon can't be 2HKO'd, have a slow revenge killer
                        if (hitsToKO > 2)
                        {
                            // We have a slow revenge killer
                            slowRevengeKillerId = i;
                        }
                    }
                }

                // If mon can two shot
                if(damageDealt > gBattleMons[opposingBattler].hp / 2)
                {
                    // If mon is faster
                    if (aiMonSpeed > playerMonSpeed || gBattleMoves[aiMove].priority > 0)
                    {
                        // If mon can't be OHKO'd, have a fast threaten
                        if (hitsToKO > 1)
                        {
                            // We have a fast threaten
                            fastThreatenId = i;
                        }
                    }
                    // If mon is slower
                    else
                    {
                        // If mon can't be 2HKO'd, have a slow threaten
                        if (hitsToKO > 2)
                        {
                            // We have a slow threaten
                            slowThreatenId = i;
                        }
                    }
                }
            }
        }
    }

    // Return results in order of effectiveness, where faster and higher damage is better
    if (revengeKillerId != PARTY_SIZE)
    {
        return revengeKillerId;
    }
    else if (slowRevengeKillerId != PARTY_SIZE)
    {
        return slowRevengeKillerId;
    }
    else if (fastThreatenId != PARTY_SIZE)
    {
        return fastThreatenId;
    }
    else if (slowThreatenId != PARTY_SIZE)
    {
        return slowThreatenId;
    }
    else
    {
        return PARTY_SIZE;
    }
}

static u32 GetBestMonDmg(struct Pokemon *party, int firstId, int lastId, u8 invalidMons, u32 opposingBattler)
{
    int i, j;
    int dmg, bestDmg = 0;
    s32 damageDealt, maxDamageDealt = 0;
    int bestMonId = PARTY_SIZE;
    u32 aiMove;

    gMoveResultFlags = 0;
    // If we couldn't find the best mon in terms of typing, find the one that deals most damage.
    for (i = firstId; i < lastId; i++)
    {
        if (gBitTable[i] & invalidMons)
            continue;
        if (IsAiPartyMonOHKOBy(opposingBattler, &party[i]))
            continue;
        maxDamageDealt = 0;
        // Find max damage mon can deal
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            aiMove = GetMonData(&party[i], MON_DATA_MOVE1 + j);
            damageDealt = AI_CalcPartyMonDamage(aiMove, gActiveBattler, opposingBattler, &party[i], TRUE);
            if (damageDealt > maxDamageDealt)
                maxDamageDealt = damageDealt;
                bestMonId = i;
        }
    }

    return bestMonId;
}

// This function integrates GetBestMonTypeMatchup (vanilla with modifications), GetBestMonDefensive (custom), and GetBestMonBatonPass (vanilla with modifications)
// the Baton Pass code will prioritize switching into a mon with Baton Pass if it can get in, boost, and BP out without being KO'd, and randomizes between multiple valid options
// the Type Matchup code will prioritize switching into a mon with the best type matchup and also a super effective move, or just best type matchup if no super effective move is found
// the Most Defensive code will prioritize switching into the mon that takes the most hits to KO, with a minimum of 4 hits required to be considered a valid option
// Everything runs in the same loop to minimize computation time. This makes it harder to read, but hopefully the comments can guide you!
static u32 GetBestMonIntegrated(struct Pokemon *party, int firstId, int lastId, u32 opposingBattler, u8 battlerIn1, u8 battlerIn2)
{
    // Variables
    int batonPassId = PARTY_SIZE, typeMatchupId = PARTY_SIZE, typeMatchupEffectiveId = PARTY_SIZE, defensiveMonId = PARTY_SIZE, aceMonId = PARTY_SIZE;
    int i, j, aliveCount = 0, bits = 0;
    s32 damageTaken = 0, maxDamageTaken = 0, maxHitsToKO = 0, hitsToKO = 0;
    s32 hitKOThreshold = 3; // 3HKO threshold that candidate defensive mons must exceed
    u32 playerMove, aiMove, aiMonSpeed = 0;
    s32 playerMonSpeed = gBattleMons[opposingBattler].speed;
    u16 bestResist = UQ_4_12(1.0), bestResistEffective = UQ_4_12(1.0);
    u16 species, typeEffectiveness;
    u8 atkType1, atkType2, defType1, defType2;

    // Iterate through mons
    for (i = firstId; i < lastId; i++)
    {
        // Check mon validity
        if (!IsValidForBattle(&party[i])
            || gBattlerPartyIndexes[battlerIn1] == i
            || gBattlerPartyIndexes[battlerIn2] == i
            || i == *(gBattleStruct->monToSwitchIntoId + battlerIn1)
            || i == *(gBattleStruct->monToSwitchIntoId + battlerIn2)
            || (GetMonAbility(&party[i]) == ABILITY_TRUANT && IsTruantMonVulnerable(gActiveBattler, opposingBattler))) // While not really invalid per say, not really wise to switch into this mon.
        {
            continue;
        }
        else if (IsAceMon(gActiveBattler, i)) // Save Ace Pokemon for last.
        {
            aceMonId = i;
            continue;
        }
        else
        {
            aliveCount++;
        }

        // Find most damaging move player could use
        maxDamageTaken = 0;
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            playerMove = gBattleMons[opposingBattler].moves[j];
            if (playerMove != MOVE_NONE && gBattleMoves[playerMove].power != 0)
            {
                damageTaken = AI_CalcPartyMonDamage(playerMove, opposingBattler, gActiveBattler, &party[i], FALSE);
                if (damageTaken > maxDamageTaken)
                    maxDamageTaken = damageTaken;
            }
        }

        // Get max number of hits for player to KO mon (GetBestMonDefensive)
        hitsToKO = GetNoOfHitsToKO(maxDamageTaken, GetMonData(&party[i], MON_DATA_HP));
        if(hitsToKO > maxHitsToKO)
        {
            maxHitsToKO = hitsToKO;
            // If hitKOThreshold is exceeded, have candidate for defensive mon
            if(maxHitsToKO > hitKOThreshold)
                defensiveMonId = i;
        }

        // GetBestMonTypeMatchup
        species = GetMonData(&party[i], MON_DATA_SPECIES);
        typeEffectiveness = UQ_4_12(1.0);

        atkType1 = gBattleMons[opposingBattler].type1;
        atkType2 = gBattleMons[opposingBattler].type2;
        defType1 = gSpeciesInfo[species].types[0];
        defType2 = gSpeciesInfo[species].types[1];

        // Multiply type effectiveness by a factor depending on type matchup
        MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType1))); 
        if (atkType2 != atkType1)
            MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType1)));
        if (defType2 != defType1)
        {
            MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType2)));
            if (atkType2 != atkType1)
                MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType2)));
        }
        if (typeEffectiveness < bestResist)
        {
            bestResist = typeEffectiveness;
            if(hitsToKO > 1)
                typeMatchupId = i;
        }

        // Check through current mon's moves
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            aiMove = GetMonData(&party[i], MON_DATA_MOVE1 + j);

            // Check for Baton Pass; hitsToKO requirements mean mon can boost and BP without dying whether it's slower or not
            if (aiMove == MOVE_BATON_PASS && ((hitsToKO > 3 && aiMonSpeed < playerMonSpeed) || (hitsToKO > 2 && aiMonSpeed > playerMonSpeed)))
                bits |= gBitTable[i];

            // Check for mon with resistance and super effective move for GetBestMonTypeMatchup
            if (aiMove != MOVE_NONE && gBattleMoves[aiMove].power != 0)
            {
                if (AI_GetTypeEffectiveness(aiMove, gActiveBattler, opposingBattler) >= UQ_4_12(2.0) && typeMatchupEffectiveId != i)
                {
                    if (typeEffectiveness < bestResistEffective)
                    {
                        bestResistEffective = typeEffectiveness;
                        if(hitsToKO > 1)
                            typeMatchupEffectiveId = i;
                    }
                }
            }
        }
    }

    // Return GetBestMonTypeMatchup > GetBestMonDefensive > GetBestMonBatonPass
    if (typeMatchupEffectiveId != PARTY_SIZE)
        return typeMatchupEffectiveId;

    else if (typeMatchupId != PARTY_SIZE)
        return typeMatchupId;

    else if (defensiveMonId != PARTY_SIZE)
        return defensiveMonId;

    // GetBestMonBatonPass randomly chooses between all mons that met Baton Pass check
    else if ((aliveCount == 2 || (aliveCount > 2 && Random() % 3 == 0)) && bits)
    {
        do
        {
            batonPassId = (Random() % (lastId - firstId)) + firstId;
        } while (!(bits & gBitTable[i]));
        return batonPassId;
    }

    // If ace mon is the last available Pokemon and U-Turn/Volt Switch was used - switch to the mon.
    else if (aceMonId != PARTY_SIZE
        && (gBattleMoves[gLastUsedMove].effect == EFFECT_HIT_ESCAPE || gBattleMoves[gLastUsedMove].effect == EFFECT_PARTING_SHOT))
        return aceMonId;

    else
        return PARTY_SIZE;
}
// This function integrates GetBestMonRevengeKiller (custom), GetBestMonTypeMatchup (vanilla with modifications), GetBestMonBatonPass (vanilla with modifications), and GetBestMonDmg (vanilla)
// the Baton Pass code will prioritize switching into a mon with Baton Pass if it can get in, boost, and BP out without being KO'd, and randomizes between multiple valid options
// the Revenge Killer code will prioritize, in order, OHKO and outspeeds / OHKO, slower but not 2HKO'd / 2HKO, outspeeds and not OHKO'd / 2HKO, slower but not 3HKO'd 
// the Type Matchup code will prioritize switching into a mon with the best type matchup and also a super effective move, or just best type matchup if no super effective move is found
// the Most Damage code will prioritize switching into whatever mon deals the most damage, which is generally not as good as having a good Type Matchup
// Everything runs in the same loop to minimize computation time. This makes it harder to read, but hopefully the comments can guide you!
static u32 GetBestMonAfterKOIntegrated(struct Pokemon *party, int firstId, int lastId, u32 opposingBattler, u8 battlerIn1, u8 battlerIn2)
{
    // Variables
    int batonPassId = PARTY_SIZE, revengeKillerId = PARTY_SIZE, slowRevengeKillerId = PARTY_SIZE, fastThreatenId = PARTY_SIZE;
    int slowThreatenId = PARTY_SIZE, typeMatchupId = PARTY_SIZE, typeMatchupEffectiveId = PARTY_SIZE, damageMonId = PARTY_SIZE, aceMonId = PARTY_SIZE;
    int i, j, aliveCount = 0, bits = 0;
    s32 maxDamageTaken = 0, damageTaken = 0, hitsToKO = 0, maxDamageDealt = 0, damageDealt = 0, aiMonSpeed = 0;
    s32 playerMonSpeed = gBattleMons[opposingBattler].speed;
    u32 aiMove, playerMove;
    u16 bestResist = UQ_4_12(1.0), bestResistEffective = UQ_4_12(1.0);
    u16 species, typeEffectiveness;
    u8 atkType1, atkType2, defType1, defType2;

    // Iterate through mons
    for (i = firstId; i < lastId; i++)
    {
        // Check mon validity
        if (!IsValidForBattle(&party[i])
            || gBattlerPartyIndexes[battlerIn1] == i
            || gBattlerPartyIndexes[battlerIn2] == i
            || i == *(gBattleStruct->monToSwitchIntoId + battlerIn1)
            || i == *(gBattleStruct->monToSwitchIntoId + battlerIn2)
            || (GetMonAbility(&party[i]) == ABILITY_TRUANT && IsTruantMonVulnerable(gActiveBattler, opposingBattler))) // While not really invalid per say, not really wise to switch into this mon.
        {
            continue;
        }
        else if (IsAceMon(gActiveBattler, i))// Save Ace Pokemon for last.
        {
            aceMonId = i;
            continue;
        }
        else
        {
            aliveCount++;
        }

        // Find most damaging move player could use
        maxDamageTaken = 0;
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            playerMove = gBattleMons[opposingBattler].moves[j];
            if (playerMove != MOVE_NONE && gBattleMoves[playerMove].power != 0)
            {
                damageTaken = AI_CalcPartyMonDamage(playerMove, opposingBattler, gActiveBattler, &party[i], FALSE);
                if (damageTaken > maxDamageTaken)
                    maxDamageTaken = damageTaken;
            }
        }

        // Get max number of hits for player to KO mon
        hitsToKO = GetNoOfHitsToKO(maxDamageTaken, GetMonData(&party[i], MON_DATA_HP));

        // GetBestMonTypeMatchup
        species = GetMonData(&party[i], MON_DATA_SPECIES);
        typeEffectiveness = UQ_4_12(1.0);

        atkType1 = gBattleMons[opposingBattler].type1;
        atkType2 = gBattleMons[opposingBattler].type2;
        defType1 = gSpeciesInfo[species].types[0];
        defType2 = gSpeciesInfo[species].types[1];

        // Multiply type effectiveness by a factor depending on type matchup
        MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType1))); 
        if (atkType2 != atkType1)
            MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType1)));
        if (defType2 != defType1)
        {
            MulModifier(&typeEffectiveness, (GetTypeModifier(atkType1, defType2)));
            if (atkType2 != atkType1)
                MulModifier(&typeEffectiveness, (GetTypeModifier(atkType2, defType2)));
        }
        if (typeEffectiveness < bestResist)
        {
            bestResist = typeEffectiveness;
            if(hitsToKO > 1)
                typeMatchupId = i;
        }

        // Check if current mon can revenge kill
        aiMonSpeed = GetMonData(&party[i], MON_DATA_SPEED);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            aiMove = GetMonData(&party[i], MON_DATA_MOVE1 + j);

            // Check for Baton Pass; hitsToKO requirements mean mon can boost and BP without dying whether it's slower or not
            if (aiMove == MOVE_BATON_PASS && ((hitsToKO > 2 && aiMonSpeed < playerMonSpeed) || (hitsToKO > 1 && aiMonSpeed > playerMonSpeed)))
                bits |= gBitTable[i];

            if (aiMove != MOVE_NONE && gBattleMoves[aiMove].power != 0)
            {
                damageDealt = AI_CalcPartyMonDamage(aiMove, gActiveBattler, opposingBattler, &party[i], TRUE);

                // Check for mon with resistance and super effective move for GetBestMonTypeMatchup
                if (AI_GetTypeEffectiveness(aiMove, gActiveBattler, opposingBattler) >= UQ_4_12(2.0) && typeMatchupEffectiveId != i)
                {
                    if (typeEffectiveness < bestResistEffective)
                    {
                        bestResistEffective = typeEffectiveness;
                        if(hitsToKO > 1)
                            typeMatchupEffectiveId = i;
                    }
                }

                // GetBestMonDmg
                if (damageDealt > maxDamageDealt)
                {
                    maxDamageDealt = damageDealt;
                    if(hitsToKO > 1)
                        damageMonId = i;
                }

                // If mon can one shot
                if(damageDealt > gBattleMons[opposingBattler].hp)
                {
                    // If mon is faster
                    if (aiMonSpeed > playerMonSpeed || gBattleMoves[aiMove].priority > 0)
                    {
                        // We have a revenge killer
                        revengeKillerId = i;
                        return revengeKillerId;
                    }

                    // If mon is slower
                    else
                    {
                        // If mon can't be 2HKO'd
                        if (hitsToKO > 2)
                        {
                            // We have a slow revenge killer
                            slowRevengeKillerId = i;
                        }
                    }
                }

                // If mon can two shot without blowing itself up
                if(damageDealt > gBattleMons[opposingBattler].hp / 2 && gBattleMoves[aiMove].effect != EFFECT_EXPLOSION)
                {
                    // If mon is faster
                    if (aiMonSpeed > playerMonSpeed || gBattleMoves[aiMove].priority > 0)
                    {
                        // If mon can't be OHKO'd
                        if (hitsToKO > 1)
                        {
                            // We have a fast threaten
                            fastThreatenId = i;
                        }
                    }
                    // If mon is slower
                    else
                    {
                        // If mon can't be 2HKO'd
                        if (hitsToKO > 2)
                        {
                            // We have a slow threaten
                            slowThreatenId = i;
                        }
                    }
                }
            }
        }
    }

    // Return GetBestMonRevengeKiller >  GetBestMonTypeMatchup > GetBestMonBatonPass > GetBestMonDmg
    if (revengeKillerId != PARTY_SIZE)
        return revengeKillerId;

    else if (slowRevengeKillerId != PARTY_SIZE)
        return slowRevengeKillerId;

    else if (fastThreatenId != PARTY_SIZE)
        return fastThreatenId;

    else if (slowThreatenId != PARTY_SIZE)
        return slowThreatenId;

    else if (typeMatchupEffectiveId != PARTY_SIZE)
        return typeMatchupEffectiveId;

    else if (typeMatchupId != PARTY_SIZE)
        return typeMatchupId;


    // GetBestMonBatonPass randomly chooses between all mons that met Baton Pass check
    else if ((aliveCount == 2 || (aliveCount > 2 && Random() % 3 == 0)) && bits)
    {
        do
        {
            batonPassId = (Random() % (lastId - firstId)) + firstId;
        } while (!(bits & gBitTable[i]));
        return batonPassId;
    }

    else if (damageMonId != PARTY_SIZE)
        return damageMonId;

    else
        return PARTY_SIZE;
}

u8 GetMostSuitableMonToSwitchInto(bool8 switchAfterMonKOd)
{
    u32 opposingBattler = 0;
    u32 bestMonId = PARTY_SIZE;
    u8 battlerIn1 = 0, battlerIn2 = 0;
    s32 firstId = 0;
    s32 lastId = 0; // + 1
    struct Pokemon *party;

    if (*(gBattleStruct->monToSwitchIntoId + gActiveBattler) != PARTY_SIZE)
        return *(gBattleStruct->monToSwitchIntoId + gActiveBattler);
    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        return gBattlerPartyIndexes[gActiveBattler] + 1;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        battlerIn1 = gActiveBattler;
        if (gAbsentBattlerFlags & gBitTable[GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)))])
            battlerIn2 = gActiveBattler;
        else
            battlerIn2 = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gActiveBattler)));

        opposingBattler = BATTLE_OPPOSITE(battlerIn1);
        if (gAbsentBattlerFlags & gBitTable[opposingBattler])
            opposingBattler ^= BIT_FLANK;
    }
    else
    {
        opposingBattler = GetBattlerAtPosition(BATTLE_OPPOSITE(GetBattlerPosition(gActiveBattler)));
        battlerIn1 = gActiveBattler;
        battlerIn2 = gActiveBattler;
    }

    GetAIPartyIndexes(gActiveBattler, &firstId, &lastId);

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    // Split ideal mon decision between after previous mon KO'd (prioritize offensive options) and after switching active mon out (prioritize defensive options), and expand the scope of both.
    // Only use better mon selection if AI_FLAG_SMART_MON_CHOICES is set for the trainer. AI_FLAG_SMART_MON_CHOICES is for smart switches in battle, AI_FLAG_SMART_MON_CHOICES_KO is for after a KO.
    // This will increase the delay before the player turn starts from 0.2s (vanilla) to up to 0.5s
    // in the worst case scenario (AI has 6 mons with 4 attacking moves each and AI_SMART_SWITCHING is enabled).
    // AI_FLAG_SMART_MON_CHOICES will have this delay to start every turn, AI_FLAG_SMART_MON_CHOICES_KO only after a KO. Both can be used together.
    if((AI_THINKING_STRUCT->aiFlags & AI_FLAG_SMART_MON_CHOICES) && !switchAfterMonKOd)
    {
        bestMonId = GetBestMonIntegrated(party, firstId, lastId, opposingBattler, battlerIn1, battlerIn2);
        return bestMonId;           
    }

    if((AI_THINKING_STRUCT->aiFlags & AI_FLAG_SMART_MON_CHOICES_KO) && switchAfterMonKOd)
    {
        bestMonId = GetBestMonAfterKOIntegrated(party, firstId, lastId, opposingBattler, battlerIn1, battlerIn2);
        return bestMonId; 
    }

    // This all handled by the GetBestMonIntegrated functions if the appropriate AI_FLAG_SMART_MON_CHOICES flag is set
    else
    {
        s32 i, j, aliveCount = 0;
        u32 invalidMons = 0, aceMonId = PARTY_SIZE;
        // Get invalid slots ids.
        for (i = firstId; i < lastId; i++)
        {
            if (!IsValidForBattle(&party[i])
                || gBattlerPartyIndexes[battlerIn1] == i
                || gBattlerPartyIndexes[battlerIn2] == i
                || i == *(gBattleStruct->monToSwitchIntoId + battlerIn1)
                || i == *(gBattleStruct->monToSwitchIntoId + battlerIn2)
                || (GetMonAbility(&party[i]) == ABILITY_TRUANT && IsTruantMonVulnerable(gActiveBattler, opposingBattler))) // While not really invalid per say, not really wise to switch into this mon.)
            {
                invalidMons |= gBitTable[i];
            }
            else if (IsAceMon(gActiveBattler, i))// Save Ace Pokemon for last.
            {
                aceMonId = i;
                invalidMons |= gBitTable[i];
            }
            else
            {
                aliveCount++;
            }
        }
        bestMonId = GetBestMonBatonPass(party, firstId, lastId, invalidMons, aliveCount, opposingBattler);
        if (bestMonId != PARTY_SIZE)
            return bestMonId;

        bestMonId = GetBestMonTypeMatchup(party, firstId, lastId, invalidMons, opposingBattler);
        if (bestMonId != PARTY_SIZE)
            return bestMonId;

        bestMonId = GetBestMonDmg(party, firstId, lastId, invalidMons, opposingBattler);
        if (bestMonId != PARTY_SIZE)
            return bestMonId;

        // If ace mon is the last available Pokemon and switch move was used - switch to the mon.
        if (aceMonId != PARTY_SIZE)
            return aceMonId;

        return PARTY_SIZE;
    }
}

static bool32 AiExpectsToFaintPlayer(void)
{
    bool32 canFaintPlayer;
    u32 i;
    u8 target = gBattleStruct->aiChosenTarget[gActiveBattler];

    if (gBattleStruct->aiMoveOrAction[gActiveBattler] > 3)
        return FALSE; // AI not planning to use move

    if (GetBattlerSide(target) != GetBattlerSide(gActiveBattler)
      && CanIndexMoveFaintTarget(gActiveBattler, target, gBattleStruct->aiMoveOrAction[gActiveBattler], 0)
      && AI_WhoStrikesFirst(gActiveBattler, target, GetAIChosenMove(gActiveBattler)) == AI_IS_FASTER) {
        // We expect to faint the target and move first -> dont use an item
        return TRUE;
    }

    return FALSE;
}

static bool8 ShouldUseItem(void)
{
    struct Pokemon *party;
    s32 i;
    u8 validMons = 0;
    bool8 shouldUse = FALSE;

    // If teaming up with player and Pokemon is on the right, or Pokemon is currently held by Sky Drop
    if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && GetBattlerPosition(gActiveBattler) == B_POSITION_PLAYER_RIGHT)
       || gStatuses3[gActiveBattler] & STATUS3_SKY_DROPPED)
        return FALSE;

    if (gStatuses3[gActiveBattler] & STATUS3_EMBARGO)
        return FALSE;

    if (AiExpectsToFaintPlayer())
        return FALSE;

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (IsValidForBattle(&party[i]))
        {
            validMons++;
        }
    }

    for (i = 0; i < MAX_TRAINER_ITEMS; i++)
    {
        u16 item;
        const u8 *itemEffects;
        u8 paramOffset;
        u8 battlerSide;

        item = gBattleResources->battleHistory->trainerItems[i];
        if (item == ITEM_NONE)
            continue;
        itemEffects = GetItemEffect(item);
        if (itemEffects == NULL)
            continue;

        switch (ItemId_GetBattleUsage(item))
        {
        case EFFECT_ITEM_HEAL_AND_CURE_STATUS:
            shouldUse = AI_ShouldHeal(0);
            break;
        case EFFECT_ITEM_RESTORE_HP:
            shouldUse = AI_ShouldHeal(itemEffects[GetItemEffectParamOffset(item, 4, 4)]);
            break;
        case EFFECT_ITEM_CURE_STATUS:
            if (itemEffects[3] & ITEM3_SLEEP && gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP)
                shouldUse = TRUE;
            if (itemEffects[3] & ITEM3_POISON && (gBattleMons[gActiveBattler].status1 & STATUS1_POISON
                                               || gBattleMons[gActiveBattler].status1 & STATUS1_TOXIC_POISON))
                shouldUse = TRUE;
            if (itemEffects[3] & ITEM3_BURN && gBattleMons[gActiveBattler].status1 & STATUS1_BURN)
                shouldUse = TRUE;
            if (itemEffects[3] & ITEM3_FREEZE && (gBattleMons[gActiveBattler].status1 & STATUS1_FREEZE || gBattleMons[gActiveBattler].status1 & STATUS1_FROSTBITE))
                shouldUse = TRUE;
            if (itemEffects[3] & ITEM3_PARALYSIS && gBattleMons[gActiveBattler].status1 & STATUS1_PARALYSIS)
                shouldUse = TRUE;
            if (itemEffects[3] & ITEM3_CONFUSION && gBattleMons[gActiveBattler].status2 & STATUS2_CONFUSION)
                shouldUse = TRUE;
            break;
        case EFFECT_ITEM_INCREASE_STAT:
        case EFFECT_ITEM_INCREASE_ALL_STATS:
            if (!gDisableStructs[gActiveBattler].isFirstTurn
                || AI_OpponentCanFaintAiWithMod(0))
                break;
            shouldUse = TRUE;
            break;
        case EFFECT_ITEM_SET_FOCUS_ENERGY:
            if (!gDisableStructs[gActiveBattler].isFirstTurn
                || gBattleMons[gActiveBattler].status2 & STATUS2_FOCUS_ENERGY
                || AI_OpponentCanFaintAiWithMod(0))
                break;
            shouldUse = TRUE;
            break;
        case EFFECT_ITEM_SET_MIST:
            battlerSide = GetBattlerSide(gActiveBattler);
            if (gDisableStructs[gActiveBattler].isFirstTurn && gSideTimers[battlerSide].mistTimer == 0)
                shouldUse = TRUE;
            break;
        case EFFECT_ITEM_REVIVE:
            gBattleStruct->itemPartyIndex[gActiveBattler] = GetFirstFaintedPartyIndex(gActiveBattler);
            if (gBattleStruct->itemPartyIndex[gActiveBattler] != PARTY_SIZE) // Revive if possible.
                shouldUse = TRUE;
            break;
        default:
            return FALSE;
        }
        if (shouldUse)
        {
            // Set selected party ID to current battler if none chosen.
            if (gBattleStruct->itemPartyIndex[gActiveBattler] == PARTY_SIZE)
                gBattleStruct->itemPartyIndex[gActiveBattler] = gBattlerPartyIndexes[gActiveBattler];
            BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_USE_ITEM, 0);
            gBattleStruct->chosenItem[gActiveBattler] = item;
            gBattleResources->battleHistory->trainerItems[i] = 0;
            return shouldUse;
        }
    }

    return FALSE;
}

static bool32 AI_ShouldHeal(u32 healAmount)
{
    bool32 shouldHeal = FALSE;

    if (gBattleMons[gActiveBattler].hp < gBattleMons[gActiveBattler].maxHP / 4
     || gBattleMons[gActiveBattler].hp == 0
     || (healAmount != 0 && gBattleMons[gActiveBattler].maxHP - gBattleMons[gActiveBattler].hp > healAmount)) {
        // We have low enough HP to consider healing
        shouldHeal = !AI_OpponentCanFaintAiWithMod(healAmount); // if target can kill us even after we heal, why bother
    }

    return shouldHeal;
}

static bool32 AI_OpponentCanFaintAiWithMod(u32 healAmount)
{
    u32 i;
    // Check special cases to NOT heal
    for (i = 0; i < gBattlersCount; i++) {
        if (GetBattlerSide(i) == B_SIDE_PLAYER) {
            if (CanTargetFaintAiWithMod(i, gActiveBattler, healAmount, 0)) {
                // Target is expected to faint us
                return TRUE;
            }
        }
    }
    return FALSE;
}

static bool32 IsAiPartyMonOHKOBy(u32 battlerAtk, struct Pokemon *aiMon)
{
    bool32 ret = FALSE;
    int i;
    struct BattlePokemon *savedBattleMons;
    s32 hp = GetMonData(aiMon, MON_DATA_HP);
    s32 damageTaken, maxDamageTaken = 0;

    // Find most damaging move player could use
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u32 playerMove = gBattleMons[battlerAtk].moves[i];
        damageTaken = AI_CalcPartyMonDamage(playerMove, battlerAtk, gActiveBattler, aiMon, FALSE);
        if (damageTaken > maxDamageTaken)
            maxDamageTaken = damageTaken;
    }

    // Check if OHKO'd
    switch (GetNoOfHitsToKO(maxDamageTaken, hp))
    {
    case 1:
        ret = TRUE;
        break;
    case 2: // if AI mon is faster allow 2 turns
        savedBattleMons = AllocSaveBattleMons();
        PokemonToBattleMon(aiMon, &gBattleMons[gActiveBattler]);
        if (AI_WhoStrikesFirst(gActiveBattler, battlerAtk, 0) == AI_IS_SLOWER)
            ret = TRUE;
        else
            ret = FALSE;
        FreeRestoreBattleMons(savedBattleMons);
        break;
    }

    return ret;
}

// Doesn't account for max moves as I don't intend to use those
static u32 CalculateHazardDamage(void)
{
    u32 totalHazardDmg = 0;
    s32 stealthHazardDmg = 0;
    u32 spikesDmg = 0;
    u32 holdEffect = GetBattlerHoldEffect(gActiveBattler, TRUE);

    if (gBattleMons[gActiveBattler].ability == ABILITY_MAGIC_GUARD || holdEffect == HOLD_EFFECT_HEAVY_DUTY_BOOTS)
        return totalHazardDmg;

    if ((gSideTimers[GetBattlerSide(gActiveBattler)].spikesAmount > 0) 
       && gBattleMons[gActiveBattler].ability != ABILITY_LEVITATE
       && holdEffect != HOLD_EFFECT_AIR_BALLOON
       && !IS_BATTLER_OF_TYPE(gActiveBattler, TYPE_FLYING)
       )
    {
        spikesDmg = (5 - gSideTimers[GetBattlerSide(gActiveBattler)].spikesAmount) * 2;
        spikesDmg = gBattleMons[gActiveBattler].maxHP / (spikesDmg);
    }

    if (gSideStatuses[GetBattlerSide(gActiveBattler)] & SIDE_STATUS_STEALTH_ROCK)
        stealthHazardDmg = GetStealthHazardDamage(gBattleMoves[MOVE_STEALTH_ROCK].type, gActiveBattler);

    totalHazardDmg = spikesDmg + stealthHazardDmg;
    
    return totalHazardDmg;
}

static bool8 IsMonHealthyEnoughToSwitch(void)
{
    u32 battlerHp = gBattleMons[gActiveBattler].hp;

    if (gBattleMons[gActiveBattler].ability == ABILITY_REGENERATOR)
        battlerHp = (battlerHp * 130) / 100; // Account for Regenerator healing
    
    if (CalculateHazardDamage() > battlerHp) // Battler will die to hazards
        return FALSE;

    if (battlerHp < gBattleMons[gActiveBattler].maxHP / 8) // Mon unlikey to be useful, at least for the AI
        return FALSE;
    
    return TRUE;
}


