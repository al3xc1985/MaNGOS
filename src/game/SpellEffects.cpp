/*
* Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
* Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos-zero>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "SkillExtraItems.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Pet.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "CreatureAI.h"
#include "BattleGroundMgr.h"
#include "BattleGround.h"
#include "BattleGroundWS.h"
#include "Language.h"
#include "SocialMgr.h"
#include "VMapFactory.h"
#include "Util.h"
#include "TemporarySummon.h"
#include "ScriptMgr.h"
#include "MoveMap.h"
#include "MoveMapSharedDefines.h"
#include "PathFinder.h"
#include "PointMovementGenerator.h"
#include "GridMap.h"
#include "DestinationHolder.h"
#include "Traveller.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS]=
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused from pre-1.2.1
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectUnused,                                   // 12 SPELL_EFFECT_PORTAL                   unused from pre-1.2.1, exist 2 spell, but not exist any data about its real usage
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused from pre-1.2.1
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectEmpty,                                    // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectEmpty,                                    // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectEmpty,                                    // 25 SPELL_EFFECT_WEAPON                   spell per weapon type, in ItemSubclassmask store mask that can be used for usability check at equip, but current way using skill also work.
    &Spell::EffectEmpty,                                    // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummon,                                   // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectLeapForward,                              // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectEmpty,                                    // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectEmpty,                                    // 39 SPELL_EFFECT_LANGUAGE                 misc store lang id
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectSummonWild,                               // 41 SPELL_EFFECT_SUMMON_WILD
    &Spell::EffectSummonGuardian,                           // 42 SPELL_EFFECT_SUMMON_GUARDIAN
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectAddHonor,                                 // 45 SPELL_EFFECT_ADD_HONOR                honor/pvp related
    &Spell::EffectNULL,                                     // 46 SPELL_EFFECT_SPAWN                    spawn/login animation, expected by spawn unit cast, also base points store some dynflags
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused from pre-1.2.1
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectOpenLock,                                 // 59 SPELL_EFFECT_OPEN_LOCK_ITEM
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectUnused,                                   // 65 SPELL_EFFECT_HEALTH_FUNNEL            unused
    &Spell::EffectUnused,                                   // 66 SPELL_EFFECT_POWER_FUNNEL             unused from pre-1.2.1
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectSummonGuardian,                           // 73 SPELL_EFFECT_SUMMON_POSSESSED
    &Spell::EffectSummonTotem,                              // 74 SPELL_EFFECT_SUMMON_TOTEM
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectSummonTotem,                              // 87 SPELL_EFFECT_SUMMON_TOTEM_SLOT1
    &Spell::EffectSummonTotem,                              // 88 SPELL_EFFECT_SUMMON_TOTEM_SLOT2
    &Spell::EffectSummonTotem,                              // 89 SPELL_EFFECT_SUMMON_TOTEM_SLOT3
    &Spell::EffectSummonTotem,                              // 90 SPELL_EFFECT_SUMMON_TOTEM_SLOT4
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectUnused,                                   // 93 SPELL_EFFECT_SUMMON_PHANTASM
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectCharge,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectSummonCritter,                            // 97 SPELL_EFFECT_SUMMON_CRITTER
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonObject,                             //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectSummonDemon,                              //112 SPELL_EFFECT_SUMMON_DEMON
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPlayerPull,                               //124 SPELL_EFFECT_PLAYER_PULL              opposite of knockback effect (pulls player twoard caster)
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectUnused,                                   //126 SPELL_EFFECT_126                      future spell steal effect? now only used one test spell
    &Spell::EffectUnused,                                   //127 SPELL_EFFECT_127                      future Prospecting spell, not have spells
    &Spell::EffectUnused,                                   //128 SPELL_EFFECT_128                      future SPELL_EFFECT_APPLY_AREA_AURA_FRIEND, not have spells
    &Spell::EffectUnused,                                   //129 SPELL_EFFECT_129                      future SPELL_EFFECT_APPLY_AREA_AURA_ENEMY, now only one test spell
};

void Spell::EffectEmpty(SpellEffectIndex /*eff_idx*/)
{
    // NOT NEED ANY IMPLEMENTATION CODE, EFFECT POSISBLE USED AS MARKER OR CLIENT INFORM
}

void Spell::EffectNULL(SpellEffectIndex /*eff_idx*/)
{
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffectIndex /*eff_idx*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN MANGOS
}

void Spell::EffectResurrectNew(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->isAlive())
        return;

    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!unitTarget->IsInWorld())
        return;

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->EffectMiscValue[eff_idx];
    pTarget->setResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectInstaKill(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || !unitTarget->isAlive())
        return;

    // Demonic Sacrifice
    if (m_spellInfo->Id==18788 && unitTarget->GetTypeId()==TYPEID_UNIT)
    {
        uint32 entry = unitTarget->GetEntry();
        uint32 spellID;
        switch (entry)
        {
        case   416:
            spellID=18789;
            break;               //imp
        case   417:
            spellID=18792;
            break;               //fellhunter
        case  1860:
            spellID=18790;
            break;               //void
        case  1863:
            spellID=18791;
            break;               //succubus
        default:
            sLog.outError("EffectInstaKill: Unhandled creature entry (%u) case.", entry);
            return;
        }

        m_caster->CastSpell(m_caster, spellID, true);
    }
    if (m_caster == unitTarget)                             // prevent interrupt message
        finish();

    WorldObject* caster = GetCastingObject();               // we need the original casting object

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, (8+8+4));
    data << (caster && caster->GetTypeId() != TYPEID_GAMEOBJECT ? m_caster->GetObjectGuid() : ObjectGuid()); // Caster GUID
    data << unitTarget->GetObjectGuid();                    // Victim GUID
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    InstakillEvent *pEvent = new InstakillEvent(*unitTarget);

    unitTarget->m_Events.AddEvent(pEvent, unitTarget->m_Events.CalculateTime(0));
}

void Spell::EffectEnvironmentalDMG(SpellEffectIndex eff_idx)
{
    uint32 absorb = 0;
    uint32 resist = 0;

    // Note: this hack with damage replace required until GO casting not implemented
    // environment damage spells already have around enemies targeting but this not help in case nonexistent GO casting support
    // currently each enemy selected explicitly and self cast damage, we prevent apply self casted spell bonuses/etc
    damage = m_spellInfo->CalculateSimpleValue(eff_idx);

    PartialResistInfo resistInfo = m_caster->MagicSpellPartialResistResult(m_caster,m_spellInfo);
    m_caster->CalculateDamageAbsorbAndResist(m_caster, GetSpellSchoolMask(m_spellInfo), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, false,resistInfo);

    m_caster->SendSpellNonMeleeDamageLog(m_caster, m_spellInfo->Id, damage, GetSpellSchoolMask(m_spellInfo), absorb, resist, false, 0, false);
    if(m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->EnvironmentalDamage(DAMAGE_FIRE, damage);
}

void Spell::EffectSchoolDMG(SpellEffectIndex effect_idx)
{
    if( unitTarget && unitTarget->isAlive())
    {
        switch(m_spellInfo->SpellFamilyName)
        {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)                     // better way to check unknown
            {
                // Wrath of Ragnaros - threat wipe
            case 20566:
            {
                if (m_caster && m_caster->GetTypeId() != TYPEID_PLAYER)
                    if (Creature* creatureCaster = (Creature*)m_caster)
                        creatureCaster->getThreatManager().modifyThreatPercent(unitTarget,-99);
                break;
            }
            // Meteor like spells (divided damage to targets)
            case 24340:
            case 26558:
            case 28884:     // Meteor
            case 26789:                             // Shard of the Fallen Star
            {
                uint32 count = 0;
                for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if(ihit->effectMask & (1<<effect_idx))
                        ++count;

                damage /= count;                    // divide to all targets
                break;
            }
            // percent from health with min
            case 25599:                             // Thundercrash
            {
                damage = unitTarget->GetHealth() / 2;
                if(damage < 200)
                    damage = 200;
                break;
            }
            }
            break;
        }

        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARRIOR:
        {
            // Bloodthirst
            if (m_spellInfo->SpellIconID==38 && m_spellInfo->SpellFamilyFlags & UI64LIT(0x2000000))
            {
                damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
            }
            // Shield Slam
            else if (m_spellInfo->SpellIconID==413 && m_spellInfo->SpellFamilyFlags & UI64LIT(0x2000000))
                damage += int32(m_caster->GetShieldBlockValue());
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Conflagrate - consumes Immolate
            if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000000200))
            {
                // for caster applied auras only
                Unit::AuraList const &mPeriodic = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                for(Unit::AuraList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
                {
                    if ((*i)->GetCasterGuid() == m_caster->GetObjectGuid() &&
                            // Immolate
                            (*i)->GetSpellProto()->IsFitToFamily(SPELLFAMILY_WARLOCK, UI64LIT(0x0000000000000004)))
                    {
                        unitTarget->RemoveAurasByCasterSpell((*i)->GetId(), m_caster->GetObjectGuid());
                        break;
                    }
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Ferocious Bite
            if((m_spellInfo->SpellFamilyFlags & UI64LIT(0x000800000)) && m_spellInfo->SpellVisual==6587)
            {
                // converts each extra point of energy into ($f1+$AP/630) additional damage
                float multiple = m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 630 + m_spellInfo->DmgMultiplier[effect_idx];
                damage += int32(m_caster->GetPower(POWER_ENERGY) * multiple);
                m_caster->SetPower(POWER_ENERGY,0);
            }

            Unit* caster = GetCaster();		// For the druids' Idol of the Moon.
            if (caster)
            {
                Unit::AuraList const& caster_auras = caster->GetAurasByType(AuraType::SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for (Aura* current_aura : caster_auras)
                    if (current_aura->GetId() == 28854)
                        damage *= 1.17;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Eviscerate
            if ((m_spellInfo->SpellFamilyFlags & UI64LIT(0x00020000)) && m_caster->GetTypeId()==TYPEID_PLAYER)
            {
                if(uint32 combo = ((Player*)m_caster)->GetComboPoints())
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * (combo + 1) * 0.03f);
                }
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
            break;
        case SPELLFAMILY_PALADIN:
            break;
        }

        if(damage >= 0)
            m_damage += damage;
    }
}

void Spell::EffectDummy(SpellEffectIndex eff_idx)
{
    if (!unitTarget && !gameObjTarget && !itemTarget)
        return;

    // selection by spell family
    switch(m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
    {
        switch(m_spellInfo->Id)
        {
        case 11402: // Shay Leafrunner's bell
        {
            if(m_caster && m_caster->GetClosestCreatureWithEntry(m_caster, 7774, 80.0f) &&
                    m_caster->GetClosestCreatureWithEntry(m_caster, 7774, 80.0f)->AI()->isPaused())
            {
                if(Creature* pShay = (Creature*)unitTarget)
                {
                    pShay->StopMoving();
                    pShay->GetMotionMaster()->Clear();
                    pShay->GetMotionMaster()->MoveIdle();
                    pShay->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                }
            }
            return;
        }
        case 3360:                                  // Curse of the Eye
        {
            if(!unitTarget)
                return;

            uint32 spell_id = (unitTarget->getGender() == GENDER_MALE) ? 10651: 10653;

            m_caster->CastSpell(unitTarget, spell_id, true);
            return;
        }
        case 4043:									// Land Mine Detonation
        {
            Creature* mine = (Creature*)m_caster;
            if (mine)
            {
                mine->ForcedDespawn();
                mine->RemoveFromWorld();
            }
        }
        case 7671:                                  // Transformation (human<->worgen)
        {
            if (!unitTarget)
                return;

            // Transform Visual
            unitTarget->CastSpell(unitTarget, 24085, true);
            return;
        }
        case 8063:                                  // Deviate Fish
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spell_id = 0;
            switch(urand(1,5))
            {
            case 1:
                spell_id = 8064;
                break;     // Sleepy
            case 2:
                spell_id = 8065;
                break;     // Invigorate
            case 3:
                spell_id = 8066;
                break;     // Shrink
            case 4:
                spell_id = 8067;
                break;     // Party Time!
            case 5:
                spell_id = 8068;
                break;     // Healthy Spirit
            }
            m_caster->CastSpell(m_caster, spell_id, true, NULL);
            return;
        }
        case 8213:                                  // Savory Deviate Delight
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spell_id = 0;
            switch(urand(1,2))
            {
                // Flip Out - ninja
            case 1:
                spell_id = (m_caster->getGender() == GENDER_MALE ? 8219 : 8220);
                break;
                // Yaaarrrr - pirate
            case 2:
                spell_id = (m_caster->getGender() == GENDER_MALE ? 8221 : 8222);
                break;
            }

            m_caster->CastSpell(m_caster,spell_id,true,NULL);
            return;
        }
        case 8344:                                  // Gnomish Universal remote.
        {
            if (!unitTarget)
                return;

            if(urand(0, 100) > 25)                  // 25 % success rate.
            {
                m_caster->CastSpell(unitTarget, 8345, true);
                return;
            }

            unitTarget->AddThreat(m_caster, 100.0f);  // Chase and attack the player if the remote fails.
            unitTarget->SetInCombatWith(m_caster);
            unitTarget->GetMotionMaster()->MoveChase(m_caster);
            m_caster->SetInCombatWith(unitTarget);

            break;
        }
        case 8593:                                  // Symbol of life (restore creature to life)
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                return;

            ((Creature*)unitTarget)->SetDeathState(JUST_ALIVED);
            return;
        }
        case 8897: // Destruction of the Goblin Rocket Boots.
        {
            if (!unitTarget)
                return;

            Player* player_caster = dynamic_cast<Player*>(m_caster);

            if(!player_caster) // Make sure the caster is a player.
                return;

            uint32 noSpaceForCount = 0;
            ItemPosCountVec dest;
            player_caster->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, 7190, 1, &noSpaceForCount ); // See if the broken boots can be added.

            if(noSpaceForCount == 0)
            {
                Item* item = player_caster->StoreNewItem( dest, 7190, true); // Add the broken boots.
                player_caster->SendNewItem(item,1,false,true);
            }

            return;
        }
        case 9976:                                  // Polly Eats the E.C.A.C.
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                return;

            // Summon Polly Jr.
            unitTarget->CastSpell(unitTarget, 9998, true);

            ((Creature*)unitTarget)->ForcedDespawn(100);
            return;
        }
        case 10254:                                 // Stone Dwarf Awaken Visual
        {
            if (m_caster->GetTypeId() != TYPEID_UNIT)
                return;

            // see spell 10255 (aura dummy)
            m_caster->clearUnitState(UNIT_STAT_ROOT);
            // m_caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            return;
        }
        case 11885:									//Muisek Vessel spells
        case 11886:
        case 11887:
        case 11888:
        case 11889:
        {
            if (!unitTarget)
                return;

            Creature* creatureTarget = (Creature*)unitTarget;
            if (creatureTarget)
            {
                creatureTarget->SaveRespawnTime();
                creatureTarget->SetDeathState(JUST_DIED);
                creatureTarget->RemoveCorpse();
            }
            return;
        }
        case 12162:                                 // Deep wounds
        case 12850:                                 // (now good common check for this spells)
        case 12868:
        {
            if (!unitTarget)
                return;

            float damage;
            // DW should benefit of attack power, damage percent mods etc.
            // TODO: check if using offhand damage is correct and if it should be divided by 2
            if (m_caster->haveOffhandWeapon() && m_caster->getAttackTimer(BASE_ATTACK) > m_caster->getAttackTimer(OFF_ATTACK))
                damage = (m_caster->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE))/2;
            else
                damage = (m_caster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXDAMAGE))/2;

            switch (m_spellInfo->Id)
            {
            case 12162:
                damage *= 0.2f;
                break;
            case 12850:
                damage *= 0.4f;
                break;
            case 12868:
                damage *= 0.6f;
                break;
            default:
                sLog.outError("Spell::EffectDummy: Spell %u not handled in DW",m_spellInfo->Id);
                return;
            };

            int32 deepWoundsDotBasePoints0 = int32(damage / 4);
            m_caster->CastCustomSpell(unitTarget, 12721, &deepWoundsDotBasePoints0, NULL, NULL, true, NULL);
            return;
        }
        case 12975:                                 //Last Stand
        {
            int32 healthModSpellBasePoints0 = int32(m_caster->GetMaxHealth()*0.3);
            m_caster->CastCustomSpell(m_caster, 12976, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
            return;
        }
        case 13006:                                 // Gnomish Shrink Ray
        {
            Player* player_caster = dynamic_cast<Player*>(m_caster);
            Group* player_group = player_caster->GetGroup();

            if(!player_caster || !unitTarget)
                return;

            uint32 roll = urand(0, 99);

            if (roll < 50)
                player_caster->CastSpell(unitTarget, 13003, true); // 50 % chance of success and shrinking the target.
            else if (roll < 70)
                player_caster->CastSpell(unitTarget, 13004, true); // 20 % risk of growing the target.
            else if (roll < 80)
                player_caster->CastSpell(player_caster, 13003, true); // 10 % risk of shrinking the caster.
            else if (roll < 85)
                player_caster->CastSpell(player_caster, 13004, true); // 5 % chance of growing the caster.
            else if (roll < 92)
            {

                if ( !player_group || player_group->GetMembersCount() == 0)
                    player_caster->CastSpell(player_caster, 13003, true); // If there are no partymembers shrink the caster.
                else
                {
                    Group::MemberSlotList const& members = player_group->GetMemberSlots();

                    std::for_each(members.begin(), members.end(), [&](Group::MemberSlot current_member)
                    {
                        Player* current_player = player_caster->GetMap()->GetPlayer(current_member.guid);

                        if(current_player)
                        {
                            uint32 spellId = 0;

                            if (roll < 90)                           // 5 % risk of shrinking the entire party.
                                spellId = 13003;
                            else if (roll < 92)                      // 2 % chance of growing the entire party.
                                spellId = 13004;

                            current_player->CastSpell(current_player, spellId, true);
                        }

                    });

                }
            }
            else
            {
                Unit::AttackerSet attackers = player_caster->getAttackers();

                if (attackers.size() == 0)
                    player_caster->CastSpell(player_caster, 13003, true); // If there are no attackers we shrink the player.
                else
                {
                    std::for_each(attackers.begin(), attackers.end(), [&](Unit* current_attacker)
                    {
                        uint32 spellId;

                        if (roll < 97)                              // 5 % risk to grow all attackers.
                            spellId = 13004;
                        else
                            spellId = 13003;                        // 2 % chance to shrink all attackers.

                        current_attacker->CastSpell(current_attacker, spellId, true);
                    });
                }
            }

            return;

        }
        case 13120:                                 // net-o-matic
        {
            if (!unitTarget)
                return;

            uint32 spell_id = 0;

            uint32 roll = urand(0, 99);

            if (roll < 2)                           // 2% for 30 sec self root (off-like chance unknown)
                spell_id = 16566;
            else if (roll < 4)                      // 2% for 20 sec root, charge to target (off-like chance unknown)
                spell_id = 13119;
            else                                    // normal root
                spell_id = 13099;

            m_caster->CastSpell(unitTarget,spell_id,true,NULL);
            return;
        }
        case 13180:
        {
            if(!unitTarget)
                return;

            // The mind control won't work if target or caster is in combat and they're set in combat by the triggering spell.
            unitTarget->CombatStop(true);
            m_caster->CombatStop(true);

            uint32 chance = urand(1, 100);
            if(chance <= 50)
                m_caster->CastSpell(unitTarget, 13181, true); // 50 % chance to control the target.
            else if(chance > 50 && chance <= 80)
                unitTarget->CastSpell(m_caster, 13181, true); // 30 % chance to be controlled by the target.

            return;
        }
        case 13278:                                 // Gnomish deathray - start the periodic damage.
        {
            m_caster->CastSpell(m_caster, 13493, true);
            return;
        }
        case 13280:                                 // Gnomish Death Ray - perform the neccessary damage calculations and do the damage.
        {
            unitTarget = dynamic_cast<CumulativeEffectAura*>(m_caster->GetAura(13278, EFFECT_INDEX_0))->GetTriggerTarget();

            float cumulativeEffect = dynamic_cast<CumulativeEffectAura*>(m_caster->GetAura(13278, EFFECT_INDEX_0))->GetCumulativeEffect();

            int32 damageMod = 800 + (uint32)(cumulativeEffect * 0.85);  // Do 800 + 85 % of the accumulated damage in final damage.

            if (urand(1, 100) <= 20)                  // A 20 % chance to do more damage.
                damageMod = (int32)((float)damageMod * 1.50f);

            if (urand(1, 100) > 18)                  // A 72 % successrate.
            {
                m_caster->CastCustomSpell(unitTarget, 13279, &damageMod, NULL, NULL, true);  // Cast succeeded.
            }
            else
            {
                m_caster->CastCustomSpell(m_caster, 13279, &damageMod, NULL, NULL, true);    // Backfire!
            }
            return;
        }
        case 13535:                                 // Tame Beast
        {
            if (!m_originalCaster || m_originalCaster->GetTypeId() != TYPEID_PLAYER)
                return;

            Creature* channelTarget = m_originalCaster->GetMap()->GetCreature(m_originalCaster->GetChannelObjectGuid());

            if (!channelTarget)
                return;

            m_originalCaster->CastSpell(channelTarget, 13481, true, NULL, NULL, m_originalCasterGUID, m_spellInfo);
            return;
        }
        case 13567:                                 // Dummy Trigger
        {
            // can be used for different aura triggering, so select by aura
            if (!m_triggeredByAuraSpell || !unitTarget)
                return;

            switch(m_triggeredByAuraSpell->Id)
            {
            case 26467:                         // Persistent Shield
                m_caster->CastCustomSpell(unitTarget, 26470, &damage, NULL, NULL, true);
                break;
            default:
                sLog.outError("EffectDummy: Non-handled case for spell 13567 for triggered aura %u",m_triggeredByAuraSpell->Id);
                break;
            }
            return;
        }
        case 14185:                                 // Preparation Rogue
        {
            if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            //immediately finishes the cooldown on certain Rogue abilities
            const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
            for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
            {
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);

                if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE &&
                        spellInfo->Id != m_spellInfo->Id && GetSpellRecoveryTime(spellInfo) > 0)
                {
                    ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first,true);
                }
                else
                    ++itr;
            }
            return;
        }
        case 14537:                                 // Six Demon Bag
        {
            if (!unitTarget)
                return;

            Unit* newTarget = unitTarget;
            uint32 spell_id = 0;
            uint32 roll = urand(0, 99);
            if (roll < 25)                          // Fireball (25% chance)
                spell_id = 15662;
            else if (roll < 50)                     // Frostbolt (25% chance)
                spell_id = 11538;
            else if (roll < 70)                     // Chain Lighting (20% chance)
                spell_id = 21179;
            else if (roll < 77)                     // Polymorph (10% chance, 7% to target)
                spell_id = 14621;
            else if (roll < 80)                     // Polymorph (10% chance, 3% to self, backfire)
            {
                spell_id = 14621;
                newTarget = m_caster;
            }
            else if (roll < 95)                     // Enveloping Winds (15% chance)
                spell_id = 25189;
            else                                    // Summon Felhund minion (5% chance)
            {
                spell_id = 14642;
                newTarget = m_caster;
            }

            m_caster->CastSpell(newTarget, spell_id, true, m_CastItem);
            return;
        }
        case 15998:                                 // Capture Worg Pup
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                return;

            Creature* creatureTarget = (Creature*)unitTarget;

            creatureTarget->ForcedDespawn();
            return;
        }
        case 16589:                                 // Noggenfogger Elixir
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spell_id = 0;
            switch(urand(1, 3))
            {
            case 1:
                spell_id = 16595;
                break;
            case 2:
                spell_id = 16593;
                break;
            default:
                spell_id = 16591;
                break;
            }

            m_caster->CastSpell(m_caster, spell_id, true, NULL);
            return;
        }
        case 17251:                                 // Spirit Healer Res
        {
            if (!unitTarget)
                return;

            Unit* caster = GetAffectiveCaster();

            if (caster && caster->GetTypeId() == TYPEID_PLAYER)
            {
                WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                data << unitTarget->GetObjectGuid();
                ((Player*)caster)->GetSession()->SendPacket( &data );
            }
            return;
        }
        case 17271:                                 // Test Fetid Skull
        {
            if (!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            uint32 spell_id = roll_chance_i(50)
                              ? 17269                             // Create Resonating Skull
                              : 17270;                            // Create Bone Dust

            m_caster->CastSpell(m_caster, spell_id, true, NULL);
            return;
        }
        case 19395:									//Gordunni Trap
        {
            m_caster->CastSpell(m_caster, 19394, true);
            return;
        }
        case 19512:                                 // Curative Animal Salve
        {
            if(m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                Player *pPlayer = (Player*)m_caster;

                if(pPlayer)
                {
                    if(pPlayer->GetQuestStatus(6124) == QUEST_STATUS_INCOMPLETE)
                    {
                        pPlayer->KilledMonsterCredit(12298);
                                        
                        if(unitTarget)
                        {
                            unitTarget->SetEntry(12299);
                            unitTarget->SetDisplayId(347);
                            unitTarget->RemoveAurasDueToSpell(19502);
                            unitTarget->UpdateVisibilityAndView();
                        }
                    }
                    else if(pPlayer->GetQuestStatus(6129) == QUEST_STATUS_INCOMPLETE)
                    {
                        pPlayer->KilledMonsterCredit(12296);

                        if(unitTarget)
                        {
                            unitTarget->SetEntry(12297);
                            unitTarget->SetDisplayId(1547);
                            unitTarget->RemoveAurasDueToSpell(19502);
                            unitTarget->UpdateVisibilityAndView();
                        }
                    }
                }
            }
            return;
        }
        case 19869:									// The Orb of Domination in BWL.
        {
            m_caster->CastSpell(unitTarget, 19832, true);

            return;
        }
        case 20572:                                 // Blood Fury
        {
            if(m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            m_caster->CastSpell(m_caster, 23230, true);

            damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
            m_caster->CastCustomSpell(m_caster, 23234, &damage, NULL, NULL, true, NULL);
            return;
        }
        case 17770:                                 // Wolfshead Helm Energy
        {
            m_caster->CastSpell(m_caster, 29940, true, NULL);

            Player* player_caster = dynamic_cast<Player*>(m_caster);

            if(!player_caster)
                return;

            player_caster->RemoveSpellCooldown(17770, true); // For some reason a cooldown is applied for the energy increase. Should work correctly as long as it's cleared.
            player_caster->RemoveSpellCooldown(29940, true);
            return;
        }
        case 17950:
        {
            if (!unitTarget)
                return;

            // Shadow Portal
            const uint32 spell_list[6] = {17863, 17939, 17943, 17944, 17946, 17948};

            m_caster->CastSpell(unitTarget, spell_list[urand(0, 5)], true);
            return;
        }
        case 19411:                                 // Lava Bomb
        case 20474:                                 // Lava Bomb
        {
            if (!unitTarget)
                return;

            unitTarget->CastSpell(unitTarget, 20494, true);
            return;
        }
        case 20577:                                 // Cannibalize
        {
            if (unitTarget)
                m_caster->CastSpell(m_caster, 20578, true, NULL);
            return;
        }
        case 21147:                                 // Arcane Vacuum
        {
            if (!unitTarget)
                return;

            // Spell used by Azuregos to teleport all the players to him
            // This also resets the target threat
            if (m_caster->getThreatManager().getThreat(unitTarget))
                m_caster->getThreatManager().modifyThreatPercent(unitTarget, -100);

            // cast summon player
            m_caster->CastSpell(unitTarget, 21150, true);

            return;
        }
        case 21908:									// Lava Burst Randomizer
        {
            uint32 LavaBurst[] = {21886, 21900, 21901, 21902, 21903, 21904, 21905, 21906, 21907};
            m_caster->CastSpell(m_caster,LavaBurst[urand(0,8)],true);

            return;
        }
        case 22276: // Elemental Shield: Vulnerability against one school of magic.
        {
            uint32 spell_id = urand(22277, 22281); // Random spell for vulnerability against a school of magic.

            m_caster->CastSpell(m_caster, spell_id, true);
            return;
        }
        case 22664: // Shadowblink 1
        {
            if (eff_idx == SpellEffectIndex::EFFECT_INDEX_0)
            {
                uint32 random_teleport_spell = urand(22672, 22676);
                unitTarget->CastSpell(unitTarget, random_teleport_spell, true);
            }
            return;
        }
        case 22681: // Shadowblink 2
        {
            if (eff_idx == SpellEffectIndex::EFFECT_INDEX_0)
            {
                uint32 random_teleport_spell = urand(22668, 22671);
                unitTarget->CastSpell(unitTarget, random_teleport_spell, true);
            }
            return;
        }
        case 23019:                                 // Crystal Prison Dummy DND
        {
            if (!unitTarget || !unitTarget->isAlive() || unitTarget->GetTypeId() != TYPEID_UNIT || ((Creature*)unitTarget)->IsPet())
                return;

            Creature* creatureTarget = (Creature*)unitTarget;
            if (creatureTarget->IsPet())
                return;
			
            creatureTarget->CastSpell(creatureTarget, 23022, true);		// added from stock mangos, cast spell Prison Crystal Conjure DND
            creatureTarget->ForcedDespawn();
            return;

            //GameObject* pGameObj = new GameObject;					// wasn't able to loot the crystal with this code.

            //Map *map = creatureTarget->GetMap();

            //// create before death for get proper coordinates
            //if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 179644, map,
            //                      creatureTarget->GetPositionX(), creatureTarget->GetPositionY(), creatureTarget->GetPositionZ(),
            //                      creatureTarget->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY) )
            //{
            //    delete pGameObj;
            //    return;
            //}

            //pGameObj->SetRespawnTime(creatureTarget->GetRespawnTime()-time(NULL));
            //pGameObj->SetOwnerGuid(m_caster->GetObjectGuid() );
            //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel() );
            //pGameObj->SetSpellId(m_spellInfo->Id);

            //creatureTarget->ForcedDespawn();

            //DEBUG_LOG("AddObject at SpellEfects.cpp EffectDummy");
            //map->Add(pGameObj);

            //WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
            //data << ObjectGuid(pGameObj->GetObjectGuid());
            //m_caster->SendMessageToSet(&data, true);

            //return;
        }
        case 23074:                                 // Arcanite Dragonling
        {
            if (!m_CastItem)
                return;

            m_caster->CastSpell(m_caster, 19804, true, m_CastItem);
            return;
        }
        case 23075:                                 // Mithril Mechanical Dragonling
        {
            if (!m_CastItem)
                return;

            m_caster->CastSpell(m_caster, 12749, true, m_CastItem);
            return;
        }
        case 23076:                                 // Mechanical Dragonling
        {
            if (!m_CastItem)
                return;

            m_caster->CastSpell(m_caster, 4073, true, m_CastItem);
            return;
        }
        case 23133:                                 // Gnomish Battle Chicken
        {
            if (!m_CastItem)
                return;

            m_caster->CastSpell(m_caster, 13166, true, m_CastItem);
            return;
        }
        case 23138:                                 // Gate of Shazzrah
        {
            if (!unitTarget)
                return;

            // Effect probably include a threat change, but it is unclear if fully
            // reset or just forced upon target for teleport (SMSG_HIGHEST_THREAT_UPDATE)

            // Gate of Shazzrah
            m_caster->CastSpell(unitTarget, 23139, true);
            return;
        }
        case 23448:                                 // Transporter Arrival - Ultrasafe Transporter: Gadgetzan - backfires
        {
            int32 r = irand(0, 119);
            if (r <= 20)                             // Transporter Malfunction - 1/6 polymorph
                m_caster->CastSpell(m_caster, 23444, true);
            else if (r > 20 && r < 100)                       // Evil Twin               - 4/6 evil twin
                m_caster->CastSpell(m_caster, 23445, true);
            else                                    // Transporter Malfunction - 1/6 miss the target
			{
				Player* pPlayer = dynamic_cast<Player*>(m_caster);
				if (pPlayer)						// teleport the player up in the sky and SW of Gadgetzan
				{
					pPlayer->UnsummonPetTemporaryIfAny();
					pPlayer->TeleportTo(1, -7402.10f, -3493.21f, 512.15f, 2.833f, 0);
					m_caster->CastSpell(m_caster, 23447, true);
				}				
				//m_caster->CastSpell(m_caster, 36902, true);			// old spell, not working
			}

            return;
        }
        case 23453:                                 // Gnomish Transporter - Ultrasafe Transporter: Gadgetzan
        {
            if (roll_chance_i(50))                  // Gadgetzan Transporter         - success
                m_caster->CastSpell(m_caster, 23441, true);
            else                                    // Gadgetzan Transporter Failure - failure
                m_caster->CastSpell(m_caster, 23446, true);

            return;
        }
        case 23645:                                 // Hourglass Sand
            m_caster->RemoveAurasDueToSpell(23170); // Brood Affliction: Bronze
            return;
        case 23725:                                 // Gift of Life (warrior bwl trinket)
            m_caster->CastSpell(m_caster, 23782, true);
            m_caster->CastSpell(m_caster, 23783, true);
            return;
        case 24531:				   // Renataki's Charm of Beast
        {
            Player* pPlayer = dynamic_cast<Player*>(m_caster);

            if (pPlayer)
            {
                uint32 spells[] = { 20904, 25294, 14295, 14287 }; // Aimed Shot, Multi-Shot, Volley, Arcane Shot

                for (unsigned short i = 0; i < 4; i++)
                {
                    pPlayer->RemoveSpellCooldown(spells[i], true);

                    const SpellChainNode* node = sSpellMgr.GetSpellChainNode(spells[i]);
                    while (node && node->prev)
                    {
                        pPlayer->RemoveSpellCooldown(node->prev, true);

                        node = sSpellMgr.GetSpellChainNode(node->prev);
                    }
                }
            }

            break;
        }
        case 24930:                                 // Hallow's End Treat
        {
            uint32 spell_id = 0;

            switch(urand(1,4))
            {
            case 1:
                spell_id = 24924;
                break;    // Larger and Orange
            case 2:
                spell_id = 24925;
                break;    // Skeleton
            case 3:
                spell_id = 24926;
                break;    // Pirate
            case 4:
                spell_id = 24927;
                break;    // Ghost
            }

            m_caster->CastSpell(m_caster, spell_id, true);
            return;
        }
        case 25860:                                 // Reindeer Transformation
        {
            if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                return;

            float speed = m_caster->GetSpeedRate(MOVE_RUN);

            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

            //5 different spells used depending on mounted speed
            if (speed >= 2.0f)
                m_caster->CastSpell(m_caster, 25859, true); //100% ground Reindeer
            else
                // Reindeer
                m_caster->CastSpell(m_caster, 25858, true); //60% ground Reindeer

            return;
        }
        case 26074:                                 // Holiday Cheer
            // implemented at client side
            return;
        case 27798:                                 // Wildheart/Feralheart energy bonus.
        {
            Player* playerTarget = dynamic_cast<Player*>(unitTarget);

            if (!playerTarget)
                return;

            int spellID = 0;
            int powerType = playerTarget->getPowerType();

            switch(powerType)
            {
            case POWER_MANA:
                spellID = 27782;
                break;
            case POWER_RAGE:
                spellID = 27783;
                break;
            case POWER_ENERGY:
                spellID = 27784;
                break;
            }

            if (spellID)
                playerTarget->CastSpell(playerTarget, spellID, true);

            return;

        }
        case 28006:                                 // Arcane Cloaking
        {
            if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER )
                // Naxxramas Entry Flag Effect DND
                m_caster->CastSpell(unitTarget, 29294, true);

            return;
        }
        }

        //All IconID Check in there
        switch(m_spellInfo->SpellIconID)
        {
            // Berserking (troll racial traits)
        case 1661:
        {
            uint32 healthPerc = uint32((float(m_caster->GetHealth())/m_caster->GetMaxHealth())*100);
            int32 melee_mod = 10;
            if (healthPerc <= 40)
                melee_mod = 30;
            if (healthPerc < 100 && healthPerc > 40)
                melee_mod = 10+(100-healthPerc)/3;

            int32 hasteModBasePoints0 = melee_mod;          // (EffectBasePoints[0]+1)-1+(5-melee_mod) = (melee_mod-1+1)-1+5-melee_mod = 5-1
            int32 hasteModBasePoints1 = abs(5-melee_mod);
            int32 hasteModBasePoints2 = 5;

            // FIXME: custom spell required this aura state by some unknown reason, we not need remove it anyway
            m_caster->ModifyAuraState(AURA_STATE_BERSERKING,true);
            m_caster->CastCustomSpell(m_caster, 26635, &hasteModBasePoints0, &hasteModBasePoints1, &hasteModBasePoints2, true, NULL);
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_MAGE:
    {
        switch(m_spellInfo->Id)
        {
        case 11189:                                 // Frost Warding
        case 28332:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // increase reflaction chanced (effect 1) of Frost Ward, removed in aura boosts
            SpellModifier *mod = new SpellModifier(SPELLMOD_EFFECT2, SPELLMOD_FLAT, damage, m_spellInfo->Id, UI64LIT(0x0000000000000100));
            ((Player*)unitTarget)->AddSpellMod(mod, true);
            break;
        }
        case 12472:                                 // Cold Snap
        {
            if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            // immediately finishes the cooldown on Frost spells
            const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
            for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
            {
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);

                if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE &&
                        (GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_FROST) &&
                        spellInfo->Id != m_spellInfo->Id && GetSpellRecoveryTime(spellInfo) > 0)
                {
                    ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first, true);
                }
                else
                    ++itr;
            }
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_WARRIOR:
    {
        // Execute
        if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x20000000))
        {
            if (!unitTarget)
                return;

            int32 basePoints0 = damage+int32(m_caster->GetPower(POWER_RAGE) * m_spellInfo->DmgMultiplier[eff_idx]);
            m_caster->CastCustomSpell(unitTarget, 20647, &basePoints0, NULL, NULL, true, 0);
            return;
        }
        if (m_spellInfo->Id==21977)                     //Warrior's Wrath
        {
            if (!unitTarget)
                return;

            m_caster->CastSpell(unitTarget,21887,true); // spell mod
            return;
        }

        // For the shaman specific spell at Nefarian in BWL.
        if (m_spellInfo->Id == 23424)
        {
            uint32 totems[] = { 14662, 14664, 14663, 14666 };

            float angle =  frand(0, 2 * 3.141592654f);

            CreatureCreatePos pos(m_caster, m_caster->GetOrientation(), 2.0f, angle);

            CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(totems[urand(0, 3)]);
            if (!cinfo)
            {
                sLog.outErrorDb("Creature entry %u does not exist but used in spell %u totem summon.", m_spellInfo->Id, m_spellInfo->EffectMiscValue[eff_idx]);
                return;
            }

            Totem* pTotem = new Totem;

            if (!pTotem->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cinfo, m_originalCaster))
            {
                delete pTotem;
                return;
            }

            pTotem->SetSummonPoint(pos);

            pTotem->SetOwner(m_originalCaster);
            pTotem->SetTypeBySummonSpell(m_spellInfo);              // must be after Create call where m_spells initialized

            pTotem->SetDuration(10 * 60 * 1000);

            if (damage)                                             // if not spell info, DB values used
            {
                pTotem->SetMaxHealth(damage);
                pTotem->SetHealth(damage);
            }

            pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

            pTotem->setFaction(103);

            pTotem->Summon(m_originalCaster);
        }

        break;
    }
    case SPELLFAMILY_WARLOCK:
    {
        // Life Tap
        if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000040000))
        {
            float cost = m_currentBasePoints[EFFECT_INDEX_0];

            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, cost,this);

            int32 dmg = m_caster->SpellDamageBonusDone(m_caster, m_spellInfo, uint32(cost > 0 ? cost : 0), SPELL_DIRECT_DAMAGE);
            dmg = m_caster->SpellDamageBonusTaken(m_caster, m_spellInfo, dmg, SPELL_DIRECT_DAMAGE);

            if (int32(m_caster->GetHealth()) > dmg)
            {
                // Shouldn't Appear in Combat Log
                m_caster->ModifyHealth(-dmg);

                int32 mana = dmg;

                Unit::AuraList const& auraDummy = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for(Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
                {
                    // only Imp. Life Tap have this in combination with dummy aura
                    if((*itr)->GetSpellProto()->SpellFamilyName==SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 208)
                        mana = ((*itr)->GetModifier()->m_amount + 100)* mana / 100;
                }

                m_caster->CastCustomSpell(m_caster,31818,&mana,NULL,NULL,true,NULL);

                // Mana Feed
                int32 manaFeedVal = m_caster->CalculateSpellDamage(m_caster, m_spellInfo, EFFECT_INDEX_1);
                manaFeedVal = manaFeedVal * mana / 100;
                if (manaFeedVal > 0)
                    m_caster->CastCustomSpell(m_caster,32553,&manaFeedVal,NULL,NULL,true,NULL);
            }
            else
                SendCastResult(SPELL_FAILED_FIZZLE);

            return;
        }
        break;
    }
    case SPELLFAMILY_PRIEST:
    {
        switch(m_spellInfo->Id )
        {
        case 28598:                                 // Touch of Weakness triggered spell
        {
            if(!unitTarget || !m_triggeredByAuraSpell)
                return;

            uint32 spellid = 0;
            switch(m_triggeredByAuraSpell->Id)
            {
            case 2652:
                spellid =  2943;
                break; // Rank 1
            case 19261:
                spellid = 19249;
                break; // Rank 2
            case 19262:
                spellid = 19251;
                break; // Rank 3
            case 19264:
                spellid = 19252;
                break; // Rank 4
            case 19265:
                spellid = 19253;
                break; // Rank 5
            case 19266:
                spellid = 19254;
                break; // Rank 6
            case 25461:
                spellid = 25460;
                break; // Rank 7
            default:
                sLog.outError("Spell::EffectDummy: Spell 28598 triggered by unhandeled spell %u",m_triggeredByAuraSpell->Id);
                return;
            }
            m_caster->CastSpell(unitTarget, spellid, true, NULL);
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_DRUID:
        break;
    case SPELLFAMILY_ROGUE:
    {
        switch(m_spellInfo->Id)
        {
        case 5938:                                  // Shiv
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            Player *pCaster = ((Player*)m_caster);

            Item *item = pCaster->GetWeaponForAttack(OFF_ATTACK);
            if (!item)
                return;

            // all poison enchantments is temporary
            uint32 enchant_id = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT);
            if (!enchant_id)
                return;

            SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
            if (!pEnchant)
                return;

            for (int s = 0; s < 3; ++s)
            {
                if (pEnchant->type[s]!=ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                    continue;

                SpellEntry const* combatEntry = sSpellStore.LookupEntry(pEnchant->spellid[s]);
                if (!combatEntry || combatEntry->Dispel != DISPEL_POISON)
                    continue;

                m_caster->CastSpell(unitTarget, combatEntry, true, item);
            }

            m_caster->CastSpell(unitTarget, 5940, true);
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_HUNTER:
    {
        // Steady Shot
        if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x100000000))
        {
            if (!unitTarget || !unitTarget->isAlive())
                return;

            bool found = false;

            // check dazed affect
            Unit::AuraList const& decSpeedList = unitTarget->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
            for(Unit::AuraList::const_iterator iter = decSpeedList.begin(); iter != decSpeedList.end(); ++iter)
            {
                if ((*iter)->GetSpellProto()->SpellIconID==15 && (*iter)->GetSpellProto()->Dispel==0)
                {
                    found = true;
                    break;
                }
            }

            if (found)
                m_damage+= damage;
            return;
        }
        switch(m_spellInfo->Id)
        {
        case 23989:                                 // Readiness talent
        {
            if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            //immediately finishes the cooldown for hunter abilities
            const SpellCooldowns& cm = ((Player*)m_caster)->GetSpellCooldownMap();
            for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
            {
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);

                if (spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && spellInfo->Id != 23989 && GetSpellRecoveryTime(spellInfo) > 0 )
                    ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first,true);
                else
                    ++itr;
            }
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_PALADIN:
    {
        switch(m_spellInfo->SpellIconID)
        {
        case 156:                                   // Holy Shock
        {
            if (!unitTarget)
                return;

            int hurt = 0;
            int heal = 0;

            switch(m_spellInfo->Id)
            {
            case 20473:
                hurt = 25912;
                heal = 25914;
                break;
            case 20929:
                hurt = 25911;
                heal = 25913;
                break;
            case 20930:
                hurt = 25902;
                heal = 25903;
                break;
            default:
                sLog.outError("Spell::EffectDummy: Spell %u not handled in HS",m_spellInfo->Id);
                return;
            }

            if (m_caster->IsFriendlyTo(unitTarget))
                m_caster->CastSpell(unitTarget, heal, true);
            else
                m_caster->CastSpell(unitTarget, hurt, true);

            return;
        }
        case 561:                                   // Judgement of command
        {
            if (!unitTarget)
                return;

            uint32 spell_id = m_currentBasePoints[eff_idx];
            SpellEntry const* spell_proto = sSpellStore.LookupEntry(spell_id);
            if (!spell_proto)
                return;

            m_caster->CastSpell(unitTarget, spell_proto, true, NULL);

            return;
        }
        }
        break;
    }
    case SPELLFAMILY_SHAMAN:
    {
        // Rockbiter Weapon
        if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x400000))
        {
            uint32 spell_id = 0;
            switch(m_spellInfo->Id)
            {
            case  8017:
                spell_id = 36494;
                break;    // Rank 1
            case  8018:
                spell_id = 36750;
                break;    // Rank 2
            case  8019:
                spell_id = 36755;
                break;    // Rank 3
            case 10399:
                spell_id = 36759;
                break;    // Rank 4
            case 16314:
                spell_id = 36763;
                break;    // Rank 5
            case 16315:
                spell_id = 36766;
                break;    // Rank 6
            case 16316:
                spell_id = 36771;
                break;    // Rank 7
            case 25479:
                spell_id = 36775;
                break;    // Rank 8
            case 25485:
                spell_id = 36499;
                break;    // Rank 9
            default:
                sLog.outError("Spell::EffectDummy: Spell %u not handled in RW", m_spellInfo->Id);
                return;
            }

            SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell_id);

            if (!spellInfo)
            {
                sLog.outError("WORLD: unknown spell id %i", spell_id);
                return;
            }

            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            for(int j = BASE_ATTACK; j <= OFF_ATTACK; ++j)
            {
                if (Item* item = ((Player*)m_caster)->GetWeaponForAttack(WeaponAttackType(j)))
                {
                    if (item->IsFitToSpellRequirements(m_spellInfo))
                    {
                        Spell *spell = new Spell(m_caster, spellInfo, true);

                        // enchanting spell selected by calculated damage-per-sec in enchanting effect
                        // at calculation applied affect from Elemental Weapons talent
                        // real enchantment damage
                        spell->m_currentBasePoints[1] = damage;

                        SpellCastTargets targets;
                        targets.setItemTarget( item );
                        spell->prepare(&targets);
                    }
                }
            }
            return;
        }
        // Flametongue Weapon Proc/Totem Effect, Ranks
        if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000400200000))
        {
            if (!m_CastItem)
            {
                sLog.outError("Spell::EffectDummy: spell %i requires cast Item", m_spellInfo->Id);
                return;
            }
            // found spelldamage coefficients of 0.381% per 0.1 speed and 15.244 per 4.0 speed
            // but own calculation say 0.385 gives at most one point difference to published values
            int32 spellDamage = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
            float weaponSpeed = (1.0f/IN_MILLISECONDS) * m_CastItem->GetProto()->Delay;
            int32 totalDamage = int32((damage + 3.85f * spellDamage) * 0.01 * weaponSpeed);

            m_caster->CastCustomSpell(unitTarget, 10444, &totalDamage, NULL, NULL, true, m_CastItem);
            return;
        }

        break;
    }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(m_spellInfo->Id))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }

    // Script based implementation. Must be used only for not good for implementation in core spell effects
    // So called only for not processed cases
    if (gameObjTarget)
        sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, eff_idx, gameObjTarget);
    else if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, eff_idx, (Creature*)unitTarget);
    else if (itemTarget)
        sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, eff_idx, itemTarget);
}

void Spell::EffectTriggerSpell(SpellEffectIndex eff_idx)
{
    // only unit case known
    if (!unitTarget)
    {
        if(gameObjTarget || itemTarget)
            sLog.outError("Spell::EffectTriggerSpell (Spell: %u): Unsupported non-unit case!",m_spellInfo->Id);
        return;
    }

    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[eff_idx];

    // special cases
    switch(triggered_spell_id)
    {
        // Linken's Boomerang proc
    case 15752:
    {
        if(urand(0, 100) < 4)
            m_caster->CastSpell(unitTarget, 15752, true);
        return;
    }
    case 15753:
    {
        if(urand(0, 100) < 4)
            m_caster->CastSpell(unitTarget, 15753, true);
        return;
    }
    // Vanish (not exist)
    case 18461:
    {
        unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
        unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
        unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);

        // if this spell is given to NPC it must handle rest by it's own AI
        if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        // get highest rank of the Stealth spell
        uint32 spellId = 0;
        const PlayerSpellMap& sp_list = ((Player*)unitTarget)->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
        {
            // only highest rank is shown in spell book, so simply check if shown in spell book
            if (!itr->second.active || itr->second.disabled || itr->second.state == PLAYERSPELL_REMOVED)
                continue;

            SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
            if (!spellInfo)
                continue;

            if (spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000000400000)))
            {
                spellId = spellInfo->Id;
                break;
            }
        }

        // no Stealth spell found
        if (!spellId)
            return;

        // reset cooldown on it if needed
        if (((Player*)unitTarget)->HasSpellCooldown(spellId))
            ((Player*)unitTarget)->RemoveSpellCooldown(spellId);

        m_caster->CastSpell(unitTarget, spellId, true);
        return;
    }
    // just skip
    case 23770:                                         // Sayge's Dark Fortune of *
        // not exist, common cooldown can be implemented in scripts if need.
        return;
        // Brittle Armor - (need add max stack of 24575 Brittle Armor)
    case 29284:
        m_caster->CastSpell(unitTarget, 24575, true, m_CastItem, NULL, m_originalCasterGUID);
        return;
        // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
    case 29286:
        m_caster->CastSpell(unitTarget, 26464, true, m_CastItem, NULL, m_originalCasterGUID);
        return;
    }

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );
    if (!spellInfo)
    {
        sLog.outError("EffectTriggerSpell of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    // select formal caster for triggered spell
    Unit* caster = m_caster;

    // some triggered spells require specific equipment
    if (spellInfo->EquippedItemClass >=0 && m_caster->GetTypeId()==TYPEID_PLAYER)
    {
        // main hand weapon required
        if (spellInfo->AttributesEx3 & SPELL_ATTR_EX3_MAIN_HAND)
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(BASE_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }

        // offhand hand weapon required
        if (spellInfo->AttributesEx3 & SPELL_ATTR_EX3_REQ_OFFHAND)
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(OFF_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }
    }
    else
    {
        // Note: not exist spells with weapon req. and IsSpellHaveCasterSourceTargets == true
        // so this just for speedup places in else
        caster = IsSpellWithCasterSourceTargetsOnly(spellInfo) ? unitTarget : m_caster;
    }

    caster->CastSpell(unitTarget,spellInfo,true,NULL,NULL,m_originalCasterGUID);
}

void Spell::EffectTriggerMissileSpell(SpellEffectIndex effect_idx)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effect_idx];

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerMissileSpell of spell %u (eff: %u): triggering unknown spell id %u",
                      m_spellInfo->Id,effect_idx,triggered_spell_id);
        return;
    }

    if (m_CastItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, spellInfo, true, m_CastItem, 0, m_originalCasterGUID);
}

void Spell::EffectTeleportUnits(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->IsTaxiFlying())
        return;

    switch (m_spellInfo->EffectImplicitTargetB[eff_idx])
    {
    case TARGET_INNKEEPER_COORDINATES:
    {
        // Only players can teleport to innkeeper
        if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            return;

        ((Player*)unitTarget)->TeleportToHomebind(unitTarget==m_caster ? TELE_TO_SPELL : 0);
        return;
    }
    case TARGET_AREAEFFECT_INSTANT:                     // in all cases first TARGET_TABLE_X_Y_Z_COORDINATES
    case TARGET_TABLE_X_Y_Z_COORDINATES:
    {
        SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id);
        if(!st)
        {
            sLog.outError( "Spell::EffectTeleportUnits - unknown Teleport coordinates for spell ID %u", m_spellInfo->Id );
            return;
        }

        if(st->target_mapId==unitTarget->GetMapId())
            unitTarget->NearTeleportTo(st->target_X,st->target_Y,st->target_Z,st->target_Orientation,unitTarget==m_caster);
        else if(unitTarget->GetTypeId()==TYPEID_PLAYER)
            ((Player*)unitTarget)->TeleportTo(st->target_mapId,st->target_X,st->target_Y,st->target_Z,st->target_Orientation,unitTarget==m_caster ? TELE_TO_SPELL : 0);
        break;
    }
    case TARGET_EFFECT_SELECT:
    {
        // m_destN filled, but sometimes for wrong dest and does not have TARGET_FLAG_DEST_LOCATION

        float x = unitTarget->GetPositionX();
        float y = unitTarget->GetPositionY();
        float z = unitTarget->GetPositionZ();
        float orientation = m_caster->GetOrientation();

        m_caster->NearTeleportTo(x, y, z, orientation, unitTarget == m_caster);
        return;
    }
    default:
    {
        // If not exist data for dest location - return
        if(!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        {
            sLog.outError( "Spell::EffectTeleportUnits - unknown EffectImplicitTargetB[%u] = %u for spell ID %u", eff_idx, m_spellInfo->EffectImplicitTargetB[eff_idx], m_spellInfo->Id );
            return;
        }
        // Init dest coordinates
        float x = m_targets.m_destX;
        float y = m_targets.m_destY;
        float z = m_targets.m_destZ;
        float orientation = unitTarget->GetOrientation();
        // Teleport
        unitTarget->NearTeleportTo(x,y,z,orientation,unitTarget==m_caster);
        return;
    }
    }

    // post effects for TARGET_TABLE_X_Y_Z_COORDINATES
    switch ( m_spellInfo->Id )
    {
        // Dimensional Ripper - Everlook
    case 23442:
    {
        int32 r = irand(0, 119);
        if ( r >= 70 )                                  // 7/12 success
        {
            if ( r < 100 )                              // 4/12 evil twin
                m_caster->CastSpell(m_caster, 23445, true);
            else                                        // 1/12 fire
                m_caster->CastSpell(m_caster, 23449, true);
        }
        return;
    }
    }
}

void Spell::EffectApplyAura(SpellEffectIndex eff_idx)
{
    if(!unitTarget)
        return;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if ( (!unitTarget->isAlive() && !(IsDeathOnlySpell(m_spellInfo) || IsDeathPersistentSpell(m_spellInfo))) &&
            (unitTarget->GetTypeId() != TYPEID_PLAYER || !((Player*)unitTarget)->GetSession()->PlayerLoading()) )
        return;

    Unit* caster = GetAffectiveCaster();
    if(!caster)
    {
        // FIXME: currently we can't have auras applied explicitly by gameobjects
        // so for auras from wild gameobjects (no owner) target used
        if (m_originalCasterGUID.IsGameObject())
            caster = unitTarget;
        else
            return;
    }

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell: Aura is: %u", m_spellInfo->EffectApplyAuraName[eff_idx]);

    switch (m_spellInfo->Id)
    {
    case 13278:
    {
        CumulativeEffectAura* aur = new CumulativeEffectAura(m_spellInfo, eff_idx, &m_currentBasePoints[eff_idx], m_spellAuraHolder, unitTarget, caster, m_CastItem);
        aur->SetDamageModifier((float)urand(75, 90) / 1000.0f);
        m_spellAuraHolder->AddAura(aur, eff_idx);
        return;
    }
    case 22247:    // Suppression traps in BWL.
        if (unitTarget->GetTypeId() != TYPEID_PLAYER || unitTarget->HasStealthAura())
            return;
        break;
    case 30918:   //Improved Sprint
        //Don't need to apply any actual aura here, just remove snare and root effects from the target!
        unitTarget->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK,30918,true);

		// Some spells have to be manually removed
		if(unitTarget->HasAura(23331))			// explicitly remove Blastwave at Lashlayer(BWL)
			unitTarget->RemoveAurasDueToSpell(23331);
		if(unitTarget->HasAura(20229))			// explicitly remove Blastwave at Majordomo(MC)
			unitTarget->RemoveAurasDueToSpell(20229);
        return;
    }


    Aura* aur = CreateAura(m_spellInfo, eff_idx, &m_currentBasePoints[eff_idx], m_spellAuraHolder, unitTarget, caster, m_CastItem);

    bool aura_can_be_applied = true;
    if (!aur->IsPositive() && unitTarget->GetNegativeAuras().size() >= 16)
    {
        Unit::SpellAuraHolderList const& negative_auras = unitTarget->GetNegativeAuras();

        if (IsChanneledSpell(m_spellInfo)) // Separate handling for channeled spells.
        {
            unitTarget->RemoveSpellAuraHolder(negative_auras.back(), AuraRemoveMode::AURA_REMOVE_BY_CANCEL); // The last debuff slot is reserved for channeled spells.
        }
        else
        {
            // If the spell can stack and the spell is already applied on the target it shouldn't be checked towards the limit.
            bool stackable_aura_found = false;
            if (m_spellInfo->StackAmount &&
            std::find_if(negative_auras.begin(), negative_auras.end(), [&](SpellAuraHolder* current_holder) {
            return current_holder->GetSpellProto()->Id == m_spellInfo->Id;
            }) != negative_auras.end())
            {
                stackable_aura_found = true;
            }

            // Do a check against the debuff cap.
            if (!stackable_aura_found)
            {
                std::vector<uint32> const *overwrites_list = sObjectMgr.GetDebuffPriorityForSpell(m_spellInfo->Id);

                SpellAuraHolder* overwrite_aura = NULL;

                // Start by checking for unprotected spells to overwrite. If a spell is missing in the DB table we consider it unprotected.
                for (SpellAuraHolder* current_aura : negative_auras)
                {
                    if (!sObjectMgr.GetDebuffPriorityForSpell(current_aura->GetSpellProto()->Id))
                    {
                        overwrite_aura = current_aura;
                    }
                }

                // Check if any of the spells present can be overwritten by the casted spell.
                if (overwrites_list && !overwrite_aura)
                {
                    for (uint32 current_overwritable : *overwrites_list)
                    {
                        Unit::SpellAuraHolderList::const_iterator itr = std::find_if(negative_auras.begin(), negative_auras.end(), [&](SpellAuraHolder* current_holder) {
                            return current_holder->GetSpellProto()->Id == current_overwritable;
                        });
                        if (itr != negative_auras.end())
                        {
                            overwrite_aura = *itr;
                            break;
                        }

                    }
                }

                // If an aura has been found we remove it, otherwise we stop the current aura from being applied.
                if (overwrite_aura)
                    unitTarget->RemoveSpellAuraHolder(overwrite_aura, AuraRemoveMode::AURA_REMOVE_BY_CANCEL);
                else
                    aura_can_be_applied = false;
            }
        }
    }

    // Check if the current aura is allowed to be applied and if not delete it.
    if (aura_can_be_applied)
        m_spellAuraHolder->AddAura(aur, eff_idx);
    else
        delete aur;
}

void Spell::EffectPowerDrain(SpellEffectIndex eff_idx)
{
    if(m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers drain_power = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;
    if(unitTarget->getPowerType() != drain_power)
        return;
    if(damage < 0)
        return;

    int32 curPower = unitTarget->GetPower(drain_power);

    //add spell damage bonus
    damage = m_caster->SpellDamageBonusDone(unitTarget,m_spellInfo,uint32(damage),SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, uint32(damage),SPELL_DIRECT_DAMAGE);

    int32 new_damage;
    if(curPower < damage)
        new_damage = curPower;
    else
        new_damage = damage;

    unitTarget->ModifyPower(drain_power,-new_damage);

    // Don`t restore from self drain
    if(drain_power == POWER_MANA && m_caster != unitTarget)
    {
        float manaMultiplier = m_spellInfo->EffectMultipleValue[eff_idx];
        if(manaMultiplier==0)
            manaMultiplier = 1;

        if(Player *modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, manaMultiplier);

        int32 gain = int32(new_damage * manaMultiplier);

        m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, gain, POWER_MANA);
    }
}

void Spell::EffectSendEvent(SpellEffectIndex eff_idx)
{
    /*
    we do not handle a flag dropping or clicking on flag in battleground by sendevent system
    */
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart %u for spellid %u in EffectSendEvent ", m_spellInfo->EffectMiscValue[eff_idx], m_spellInfo->Id);

    if(m_spellInfo->Id == 25783 && m_caster)
    {
        Creature *pMaws = m_caster->SummonCreature(15571, 3507.87f, -6554.89f, -3.93f, 2.80f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

        if(pMaws)
        {
            pMaws->AI()->AttackStart(m_caster);
            pMaws->SetSplineFlags(SPLINEFLAG_FLYING);
        }

        return;
    }

    if (!sScriptMgr.OnProcessEvent(m_spellInfo->EffectMiscValue[eff_idx], m_caster, focusObject, true))
        m_caster->GetMap()->ScriptsStart(sEventScripts, m_spellInfo->EffectMiscValue[eff_idx], m_caster, focusObject);
}

void Spell::EffectPowerBurn(SpellEffectIndex eff_idx)
{
    if (m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers powertype = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;
    if (unitTarget->getPowerType()!=powertype)
        return;
    if (damage < 0)
        return;

    int32 curPower = int32(unitTarget->GetPower(powertype));

    int32 new_damage = (curPower < damage) ? curPower : damage;

    unitTarget->ModifyPower(powertype, -new_damage);
    float multiplier = m_spellInfo->EffectMultipleValue[eff_idx];

    if (Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    new_damage = int32(new_damage * multiplier);
    m_damage += new_damage;
}

void Spell::EffectHeal(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = GetAffectiveCaster();
        if (!caster)
            return;

        int32 addhealth = damage;

        // Swiftmend - consumes Regrowth or Rejuvenation
        if (m_spellInfo->Id == 18562)
        {
            Unit::AuraList const& RejorRegr = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
            // find most short by duration
            Aura *targetAura = NULL;
            for(Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
            {
                if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID &&
                        // Regrowth or Rejuvenation 0x40 | 0x10
                        ((*i)->GetSpellProto()->SpellFamilyFlags & UI64LIT(0x0000000000000050)))
                {
                    if (!targetAura || (*i)->GetAuraDuration() < targetAura->GetAuraDuration())
                        targetAura = *i;
                }
            }

            if (!targetAura)
            {
                sLog.outError("Target (GUID: %u TypeId: %u) has aurastate AURA_STATE_SWIFTMEND but no matching aura.", unitTarget->GetGUIDLow(), unitTarget->GetTypeId());
                return;
            }
            int idx = 0;
            while(idx < 3)
            {
                if(targetAura->GetSpellProto()->EffectApplyAuraName[idx] == SPELL_AURA_PERIODIC_HEAL)
                    break;
                idx++;
            }

            int32 tickheal = targetAura->GetModifier()->m_amount;
            int32 tickcount = GetSpellDuration(targetAura->GetSpellProto()) / targetAura->GetSpellProto()->EffectAmplitude[idx] - 1;

            unitTarget->RemoveAurasDueToSpell(targetAura->GetId());

            addhealth += tickheal * tickcount;
        }

        // For the shamans' Totem of Sustaining.
        if (m_spellInfo->SpellIconID == 964 && caster->HasAura(28856))
            m_healing += 53;

        SpellEntry const* spellInfo = m_spellInfo;
        if (m_spellInfo->Id == 19968 || m_spellInfo->Id == 19993)
            spellInfo = sSpellStore.LookupEntry(m_currentBasePoints[EFFECT_INDEX_1]);

        // If the caster has the debuff from Nefarian debuff 23402 should be applied to the target.
        if (caster->GetAura(23401, EFFECT_INDEX_0))
            caster->CastSpell(unitTarget, 23402, true);


        if (spellInfo)
        {
            addhealth = caster->SpellHealingBonusDone(unitTarget, spellInfo, addhealth, HEAL);
            addhealth = unitTarget->SpellHealingBonusTaken(caster, spellInfo, addhealth, HEAL);
        }

        // Lifegiving Gem
        if (m_spellInfo->Id == 23783)
        {
            // Divide by 1.15 to avoid taking the new max health from the health
            // increase effect.
            m_healing = unitTarget->GetMaxHealth() * 0.15f / 1.15f;
        }

        m_healing += addhealth;
    }
}

void Spell::EffectHealMechanical(SpellEffectIndex /*eff_idx*/)
{
    // Mechanic creature type should be correctly checked by targetCreatureType field
    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = GetAffectiveCaster();
        if (!caster)
            return;

        uint32 addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, damage, HEAL);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, addhealth, HEAL);

        caster->DealHeal(unitTarget, addhealth, m_spellInfo);
    }
}

void Spell::EffectHealthLeech(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    if (damage < 0)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "HealthLeech :%i", damage);

    uint32 curHealth = unitTarget->GetHealth();
    damage = m_caster->SpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage );
    if ((int32)curHealth < damage)
        damage = curHealth;

    float multiplier = m_spellInfo->EffectMultipleValue[eff_idx];

    if (Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    uint32 heal = uint32(damage*multiplier);
    if (m_caster->isAlive())
    {
        heal = m_caster->SpellHealingBonusTaken(m_caster, m_spellInfo, heal, HEAL);

        m_caster->DealHeal(m_caster, heal, m_spellInfo);
    }
}

void Spell::DoCreateItem(SpellEffectIndex /*eff_idx*/, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Tanaris Field Sampling item spells
    switch (m_spellInfo->Id)
    {
    case 11759:
    case 11760:
    case 11761:
        if (urand(0,2))
            return;
        break;
    default:
        break;
    }

    Player* player = (Player*)unitTarget;

    uint32 newitemid = itemtype;
    ItemPrototype const *pProto = ObjectMgr::GetItemPrototype( newitemid );
    if(!pProto)
    {
        player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
        return;
    }

    // bg reward have some special in code work
    uint32 bgType = 0;
    switch(m_spellInfo->Id)
    {
    case SPELL_AV_MARK_WINNER:
    case SPELL_AV_MARK_LOSER:
        bgType = BATTLEGROUND_AV;
        break;
    case SPELL_WS_MARK_WINNER:
    case SPELL_WS_MARK_LOSER:
        bgType = BATTLEGROUND_WS;
        break;
    case SPELL_AB_MARK_WINNER:
    case SPELL_AB_MARK_LOSER:
        bgType = BATTLEGROUND_AB;
        break;
    default:
        break;
    }

    uint32 num_to_add = damage;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->Stackable)
        num_to_add = pProto->Stackable;

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count=1;
    // the chance to create additional items
    float additionalCreateChance=0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum=0;
    // get the chance and maximum number for creating extra items
    if ( canCreateExtraItems(player, m_spellInfo->Id, additionalCreateChance, additionalMaxNum) )
    {
        // roll with this chance till we roll not to create or we create the max num
        while ( roll_chance_f(additionalCreateChance) && items_count<=additionalMaxNum )
            ++items_count;
    }

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    InventoryResult msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space );
    if ( msg != EQUIP_ERR_OK )
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError( msg, NULL, NULL, newitemid );
            return;
        }
    }

    if(num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem( dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if(!pItem)
        {
            player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetProto()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetProto()->Class != ITEM_CLASS_QUEST && (pItem->GetProto()->Flags & ITEM_FLAG_HIDE_CRAFTERS_NAME) == 0 )
            pItem->SetGuidValue(ITEM_FIELD_CREATOR, player->GetObjectGuid());

        // send info to the client
        if(pItem)
            player->SendNewItem(pItem, num_to_add, true, bgType == 0);

        // we succeeded in creating at least one item, so a levelup is possible
        if(bgType == 0)
            player->UpdateCraftSkill(m_spellInfo->Id);
    }

    // for battleground marks send by mail if not add all expected
    if(no_space > 0 && bgType)
    {
        if(BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(bgType)))
            bg->SendRewardMarkByMail(player, newitemid, no_space);
    }
}

void Spell::EffectCreateItem(SpellEffectIndex eff_idx)
{
    DoCreateItem(eff_idx,m_spellInfo->EffectItemType[eff_idx]);

    switch(m_spellInfo->Id)
    {
        case 15958: // Rookery egg
        {
            if(m_caster)
            {
                GameObject *rookeryEgg = m_caster->GetClosestGameObjectWithEntry(m_caster, 175124, 5.0f);

                if(rookeryEgg)
                    rookeryEgg->GetMap()->Remove(rookeryEgg, true);
            }
            break;
        }
        case 13982: // Altered Black Dragonflight Mold
        {
            if(unitTarget && m_caster)
            {
                unitTarget = m_caster;
                DoCreateItem(eff_idx,m_spellInfo->EffectItemType[eff_idx]);
            }
            break;
        }
    }
}

void Spell::EffectPersistentAA(SpellEffectIndex eff_idx)
{
    Unit* pCaster = GetAffectiveCaster();
    // FIXME: in case wild GO will used wrong affective caster (target in fact) as dynobject owner
    if (!pCaster)
        pCaster = m_caster;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

    if (Player* modOwner = pCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);

    DynamicObject* dynObj = new DynamicObject;
    if (!dynObj->Create(pCaster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), pCaster, m_spellInfo->Id,
                        eff_idx, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_duration, radius, DYNAMIC_OBJECT_AREA_SPELL))
    {
        delete dynObj;
        return;
    }

    pCaster->AddDynObject(dynObj);
    pCaster->GetMap()->Add(dynObj);
}

void Spell::EffectEnergize(SpellEffectIndex eff_idx)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    if(m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers power = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    // Some level depends spells
    int level_multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
    case 24571:                                         // Blood Fury
        level_diff = m_caster->getLevel() - 60;
        level_multiplier = 10;
        break;
    case 24532:                                         // Burst of Energy
        level_diff = m_caster->getLevel() - 60;
        level_multiplier = 4;
        break;
    default:
        break;
    }

    if (level_diff > 0)
        damage -= level_multiplier * level_diff;

    if(damage < 0)
        return;

    if(unitTarget->GetMaxPower(power) == 0)
        return;

    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, damage, power);
}

void Spell::SendLoot(ObjectGuid guid, LootType loottype, LockType lockType)
{
    if (gameObjTarget)
    {
        switch (gameObjTarget->GetGoType())
        {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
        case GAMEOBJECT_TYPE_QUESTGIVER:
        case GAMEOBJECT_TYPE_SPELL_FOCUS:
        case GAMEOBJECT_TYPE_GOOBER:
            gameObjTarget->Use(m_caster);
            return;

        case GAMEOBJECT_TYPE_CHEST:
            gameObjTarget->Use(m_caster);
            // Don't return, let loots been taken
            break;

        case GAMEOBJECT_TYPE_TRAP:
            if (gameObjTarget->GetEntry() == 178559) //Exception for Larva Spewer in Maraudon
            {
                gameObjTarget->SetGoState(GO_STATE_ACTIVE);
                return;
            }
            if (gameObjTarget->GetEntry() == 175124) //Rookery egg.
            {
                gameObjTarget->DisarmTrap();
                return;
            }
            if (lockType == LOCKTYPE_DISARM_TRAP)
            {
                gameObjTarget->DisarmTrap();
                return;
            }
            sLog.outError("Spell::SendLoot unhandled locktype %u for GameObject trap (entry %u) for spell %u.", lockType, gameObjTarget->GetEntry(), m_spellInfo->Id);
            return;
        default:
            sLog.outError("Spell::SendLoot unhandled GameObject type %u (entry %u).", gameObjTarget->GetGoType(), gameObjTarget->GetEntry());
            return;
        }
    }

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // Send loot
    ((Player*)m_caster)->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffectIndex eff_idx)
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        DEBUG_LOG( "WORLD: Open Lock - No Player Caster!");
        return;
    }

    Player* player = (Player*)m_caster;

    uint32 lockId = 0;
    ObjectGuid guid;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectInfo const* goInfo = gameObjTarget->GetGOInfo();
        // Arathi Basin banner opening !
        if ((goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune) ||
                (goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK))
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (BattleGround *bg = player->GetBattleGround())
            {
                // check if it's correct bg
                if (bg->GetTypeID() == BATTLEGROUND_AB || bg->GetTypeID() == BATTLEGROUND_AV)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND)
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (player->GetBattleGround())
                return;
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_CHEST)  // Handling for chest restocking.
        {
            if (gameObjTarget->GetChestRestockTime() == 0)
                gameObjTarget->SetChestRestockTime(time(NULL) + gameObjTarget->GetRespawnDelay());
            else if (gameObjTarget->GetChestRestockTime() <= time(NULL))
            {
                gameObjTarget->loot.clear();
                gameObjTarget->SetLootState(GO_READY);
                gameObjTarget->SetChestRestockTime(0);
            }
        }
        lockId = goInfo->GetLockId();
        guid = gameObjTarget->GetObjectGuid();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetProto()->LockID;
        guid = itemTarget->GetObjectGuid();
    }
    else
    {
        DEBUG_LOG( "WORLD: Open Lock - No GameObject/Item Target!");
        return;
    }

    SkillType skillId = SKILL_NONE;
    int32 reqSkillValue = 0;
    int32 skillValue;

    SpellCastResult res = CanOpenLock(eff_idx, lockId, skillId, reqSkillValue, skillValue);
    if (res != SPELL_CAST_OK)
    {
        SendCastResult(res);
        return;
    }

    // mark item as unlocked
    if (itemTarget)
        itemTarget->SetFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_UNLOCKED);

    SendLoot(guid, LOOT_SKINNING, LockType(m_spellInfo->EffectMiscValue[eff_idx]));

    // not allow use skill grow at item base open
    if (!m_CastItem && skillId != SKILL_NONE)
    {
        // update skill if really known
        if (uint32 pureSkillValue = player->GetPureSkillValue(skillId))
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player) &&
                        player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue))
                    gameObjTarget->AddToSkillupList(player);
            }
            else if (itemTarget)
            {
                // Do one skill-up
                player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue);
            }
        }
    }
}

void Spell::EffectSummonChangeItem(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)m_caster;

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGuid()!=player->GetObjectGuid())
        return;

    uint32 newitemid = m_spellInfo->EffectItemType[eff_idx];
    if (!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item *pNewItem = Item::CreateItem( newitemid, 1, player);
    if (!pNewItem)
        return;

    for(uint8 j= PERM_ENCHANTMENT_SLOT; j<=TEMP_ENCHANTMENT_SLOT; ++j)
    {
        if (m_CastItem->GetEnchantmentId(EnchantmentSlot(j)))
            pNewItem->SetEnchantment(EnchantmentSlot(j), m_CastItem->GetEnchantmentId(EnchantmentSlot(j)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(j)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(j)));
    }

    if (m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY))
    {
        double loosePercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY));
        player->DurabilityLoss(pNewItem, loosePercent);
    }

    if (player->IsInventoryPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreItem( m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true );
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->StoreItem( dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsBankPos (pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem( m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true );
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->BankItem( dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsEquipmentPos (pos))
    {
        uint16 dest;
        uint8 msg = player->CanEquipItem( m_CastItem->GetSlot(), dest, pNewItem, true );
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->EquipItem( dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectProficiency(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *p_target = (Player*)unitTarget;

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectApplyAreaAura(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    AreaAura* Aur = new AreaAura(m_spellInfo, eff_idx, &m_currentBasePoints[eff_idx], m_spellAuraHolder, unitTarget, m_caster, m_CastItem);
    m_spellAuraHolder->AddAura(Aur, eff_idx);
}

void Spell::EffectSummon(SpellEffectIndex eff_idx)
{
    if (!m_caster->GetPetGuid().IsEmpty())
        return;

    if (!unitTarget)
        return;
    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummon: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    uint32 level = m_caster->getLevel();
    Pet* spawnCreature = new Pet(SUMMON_PET);

    if (m_caster->GetTypeId()==TYPEID_PLAYER && spawnCreature->LoadPetFromDB((Player*)m_caster,pet_entry))
    {
        // Summon in dest location
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            spawnCreature->Relocate(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

        // set timer for unsummon
        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        return;
    }

    // Summon in dest location
    CreatureCreatePos pos (m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_caster, -m_caster->GetOrientation());

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        sLog.outErrorDb("Spell::EffectSummon: can't create creature with entry %u for spell %u", cInfo->Entry, m_spellInfo->Id);
        delete spawnCreature;
        return;
    }

    spawnCreature->SetSummonPoint(pos);

    // set timer for unsummon
    if (m_duration > 0)
        spawnCreature->SetDuration(m_duration);

    spawnCreature->SetOwnerGuid(m_caster->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    spawnCreature->setPowerType(POWER_MANA);
    spawnCreature->setFaction(m_caster->getFaction());
    spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    spawnCreature->SetCreatorGuid(m_caster->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    spawnCreature->InitStatsForLevel(level, m_caster);

    spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);

    spawnCreature->UpdateWalkMode(m_caster);

    spawnCreature->AIM_Initialize();
    spawnCreature->InitPetCreateSpells();
    spawnCreature->SetHealth(spawnCreature->GetMaxHealth());
    spawnCreature->SetPower(POWER_MANA, spawnCreature->GetMaxPower(POWER_MANA));

    //spawnCreature->SetName("");                           // generated by client

    map->Add((Creature*)spawnCreature);

    m_caster->SetPet(spawnCreature);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        spawnCreature->GetCharmInfo()->SetReactState( REACT_DEFENSIVE );
        spawnCreature->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_caster)->PetSpellInitialize();
    }
    else
        spawnCreature->GetCharmInfo()->SetReactState( REACT_AGGRESSIVE );

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned((Creature*)spawnCreature);
}

void Spell::EffectLearnSpell(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            EffectLearnPetSpell(eff_idx);

        return;
    }

    Player *player = (Player*)unitTarget;

    uint32 spellToLearn = m_spellInfo->EffectTriggerSpell[eff_idx];
    player->learnSpell(spellToLearn, false);

    DEBUG_LOG( "Spell: Player %u has learned spell %u from NpcGUID=%u", player->GetGUIDLow(), spellToLearn, m_caster->GetGUIDLow() );
}

void Spell::EffectDispel(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    // Shield Slam 50% chance dispel
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR && (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000100000000)) &&
            !roll_chance_i(50))
        return;

    // Fill possible dispel list
    std::list <std::pair<SpellAuraHolder* ,uint32> > dispel_list;

    // Create dispel mask by dispel type
    uint32 dispel_type = m_spellInfo->EffectMiscValue[eff_idx];
    uint32 dispelMask  = GetDispellMask( DispelType(dispel_type) );
    Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
    for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        SpellAuraHolder *holder = itr->second;
        if ((1<<holder->GetSpellProto()->Dispel) & dispelMask)
        {
            if(holder->GetSpellProto()->Dispel == DISPEL_MAGIC)
            {
                bool positive = true;
                if (!holder->IsPositive())
                    positive = false;

                // Do not dispel the dummy aura for mages' Combustion.
                if (holder->GetSpellProto()->Id == 11129)
                    continue;

                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if (positive == unitTarget->IsFriendlyTo(m_caster))
                    continue;
            }
            dispel_list.push_back(std::pair<SpellAuraHolder* ,uint32>(holder, holder->GetStackAmount()));
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!dispel_list.empty())
    {
        std::list<std::pair<SpellAuraHolder* ,uint32> > success_list;// (spell_id,casterGuid)
        std::list < uint32 > fail_list;                     // spell_id

        // some spells have effect value = 0 and all from its by meaning expect 1
        if(!damage)
            damage = 1;

        // Dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count=0; count < damage && !dispel_list.empty(); ++count)
        {
            // Random select buff for dispel
            std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator dispel_itr = dispel_list.begin();
            std::advance(dispel_itr,urand(0, dispel_list.size()-1));

            SpellAuraHolder *holder = dispel_itr->first;

            dispel_itr->second -= 1;

            // remove entry from dispel_list if nothing left in stack
            if (dispel_itr->second == 0)
                dispel_list.erase(dispel_itr);

            SpellEntry const* spellInfo = holder->GetSpellProto();
            // Base dispel chance
            // TODO: possible chance depend from spell level??
            int32 miss_chance = 0;
            // Apply dispel mod from aura caster
            if (Unit *caster = holder->GetCaster())
            {
                if ( Player* modOwner = caster->GetSpellModOwner() )
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_DISPEL_CHANCE, miss_chance, this);
            }
            // Try dispel
            if (roll_chance_i(miss_chance))
                fail_list.push_back(spellInfo->Id);
            else
            {
                bool foundDispelled = false;
                for (std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator success_iter = success_list.begin(); success_iter != success_list.end(); ++success_iter)
                {
                    if (success_iter->first->GetId() == holder->GetId() && success_iter->first->GetCasterGuid() == holder->GetCasterGuid())
                    {
                        success_iter->second += 1;
                        foundDispelled = true;
                        break;
                    }
                }
                if (!foundDispelled)
                    success_list.push_back(std::pair<SpellAuraHolder* ,uint32>(holder, 1));
            }
        }
        // Send success log and really remove auras
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLDISPELLOG, 8+8+4+1+4+count*5);
            data << unitTarget->GetPackGUID();              // Victim GUID
            data << m_caster->GetPackGUID();                // Caster GUID
            data << uint32(m_spellInfo->Id);                // Dispel spell id
            data << uint8(0);                               // not used
            data << uint32(count);                          // count
            for (std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellAuraHolder* dispelledHolder = j->first;
                data << uint32(dispelledHolder->GetId());   // Spell Id
                data << uint8(0);                           // 0 - dispelled !=0 cleansed
                unitTarget->RemoveAuraHolderDueToSpellByDispel(dispelledHolder->GetId(), j->second, dispelledHolder->GetCasterGuid(), m_caster);
            }
            m_caster->SendMessageToSet(&data, true);

            // On success dispel
            // Devour Magic
            if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->Category == SPELLCATEGORY_DEVOUR_MAGIC)
            {
                uint32 heal_spell = 0;
                switch (m_spellInfo->Id)
                {
                case 19505:
                    heal_spell = 19658;
                    break;
                case 19731:
                    heal_spell = 19732;
                    break;
                case 19734:
                    heal_spell = 19733;
                    break;
                case 19736:
                    heal_spell = 19735;
                    break;
                default:
                    DEBUG_LOG("Spell for Devour Magic %d not handled in Spell::EffectDispel", m_spellInfo->Id);
                    break;
                }
                if (heal_spell)
                    m_caster->CastSpell(m_caster, heal_spell, true);
            }
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispel
            WorldPacket data(SMSG_DISPEL_FAILED, 8+8+4+4*fail_list.size());
            data << m_caster->GetObjectGuid();              // Caster GUID
            data << unitTarget->GetObjectGuid();            // Victim GUID
            data << uint32(m_spellInfo->Id);                // Dispel spell id
            for (std::list< uint32 >::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectDualWield(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanDualWield(true);
}

void Spell::EffectPull(SpellEffectIndex /*eff_idx*/)
{
    // TODO: create a proper pull towards distract spell center for distract
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectDistract(SpellEffectIndex /*eff_idx*/)
{
    // Check for possible target
    if (!unitTarget || unitTarget->isInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
        return;

    float angle = unitTarget->GetAngle(m_targets.m_destX, m_targets.m_destY);

    unitTarget->clearUnitState(UNIT_STAT_MOVING);
    unitTarget->SetOrientation(angle);
    unitTarget->SendMonsterMove(unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), SPLINETYPE_FACINGANGLE, SPLINEFLAG_WALKMODE, 0, NULL, angle);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);
}

void Spell::EffectPickPocket(SpellEffectIndex /*eff_idx*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->IsFriendlyTo(unitTarget))
        return;

    // victim have to be alive and humanoid or undead
    if (unitTarget->isAlive() && (unitTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
    {
        int32 chance = 10 + int32(m_caster->getLevel()) - int32(unitTarget->getLevel());

        if (chance > irand(0, 12))
        {
            // Stealing successful
            //DEBUG_LOG("Sending loot from pickpocket");
            ((Player*)m_caster)->SendLoot(unitTarget->GetObjectGuid(),LOOT_PICKPOCKETING);
        }
        else
        {
            // Reveal action + get attack
            if (((Creature*)unitTarget)->AI())
                ((Creature*)unitTarget)->AI()->AttackedBy(m_caster);
        }
    }
}

void Spell::EffectAddFarsight(SpellEffectIndex eff_idx)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 duration = GetSpellDuration(m_spellInfo);
    DynamicObject* dynObj = new DynamicObject;

    // set radius to 0: spell not expected to work as persistent aura
    if(!dynObj->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster,
                       m_spellInfo->Id, eff_idx, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, duration, 0, DYNAMIC_OBJECT_FARSIGHT_FOCUS))    {
        delete dynObj;
        return;
    }

    m_caster->AddDynObject(dynObj);
    m_caster->GetMap()->Add(dynObj);

    ((Player*)m_caster)->GetCamera().SetView(dynObj);
}

void Spell::EffectSummonWild(SpellEffectIndex eff_idx)
{
    uint32 creature_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!creature_entry)
        return;

    // The following code is not used in the current function.
    // 	uint32 level = m_caster->getLevel();
    //
    // 	// level of creature summoned using engineering item based at engineering skill level
    // 	if (m_caster->GetTypeId()==TYPEID_PLAYER && m_CastItem)
    // 	{
    // 		ItemPrototype const *proto = m_CastItem->GetProto();
    // 		if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
    // 		{
    // 			uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINEERING);
    // 			if (skill202)
    // 			{
    // 				level = skill202/5;
    // 			}
    // 		}
    // 	}

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));
    int32 duration = GetSpellDuration(m_spellInfo);
    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_OR_DEAD_DESPAWN;

    int32 amount = damage > 0 ? damage : 1;

    for(int32 count = 0; count < amount; ++count)
    {
        float px, py, pz;
        // If dest location if present
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon 1 unit in dest location
            if (count == 0)
            {
                px = m_targets.m_destX;
                py = m_targets.m_destY;
                pz = m_targets.m_destZ;
            }
            // Summon in random point all other units if location present
            else
                m_caster->GetRandomPoint(center_x, center_y, center_z, radius, px, py, pz);
        }
        // Summon if dest location not present near caster
        else
        {
            if (radius > 0.0f)
            {
                // not using bounding radius of caster here
                m_caster->GetClosePoint(px, py, pz, 0.0f, radius);
            }
            else
            {
                if((m_spellInfo->Id == 16613 || m_spellInfo->Id == 16619 || m_spellInfo->Id == 16630 ||
                        m_spellInfo->Id == 16631) && m_caster && m_caster->GetClosestGameObjectWithEntry(m_caster, 175795, 30.0f)) // Temporal parasite
                {
                    if(GameObject* andorhalSilo = m_caster->GetClosestGameObjectWithEntry(m_caster, 175795, 30.0f))
                    {
                        if(!andorhalSilo->isSpawned() || andorhalSilo->getLootState() == GO_JUST_DEACTIVATED) // Don't spawn if silo is inactivated.
                        {
                            return;
                        }
                        else
                        {
                            m_caster->GetClosePoint(px, py, pz, 0.0f, 0.5f);

                            int towerDeactivate = urand(1,2);

                            if(towerDeactivate == 2)
                            {
                                andorhalSilo->SetLootState(GO_JUST_DEACTIVATED);
                                andorhalSilo->UpdateObjectVisibility();
                            }
                            else if(m_spellInfo->Id == 16631) // Make sure last parasite deactivates silo if not already deactivated.
                            {
                                andorhalSilo->SetLootState(GO_JUST_DEACTIVATED);
                                andorhalSilo->UpdateObjectVisibility();
                            }
                        }
                    }

                }
                else
                {
                    // EffectRadiusIndex 0 or 36
                    px = m_caster->GetPositionX();
                    py = m_caster->GetPositionY();
                    pz = m_caster->GetPositionZ();
                }
            }
        }

        if(Creature *summon = m_caster->SummonCreature(creature_entry, px, py, pz, m_caster->GetOrientation(), summonType, duration))
        {
            summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

            // UNIT_FIELD_CREATEDBY are not set for these kind of spells.
            // Does exceptions exist? If so, what are they?
            // summon->SetCreatorGuid(m_caster->GetObjectGuid());

            //Spell 16031: Releasing Corrupt Ooze
            if (m_spellInfo->Id == 16031)
            {
                summon->setFaction(m_caster->getFaction());
                Unit* U = m_targets.getUnitTarget();
                //if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
                if (U && U->GetTypeId() == TYPEID_UNIT)
                {
                    if (U->GetEntry() != 6557 || !U->isAlive())
                    {
                        summon->ForcedDespawn();
                        return;
                    }
                    Creature* creatureTarget = (Creature*)U;
                    if (creatureTarget && creatureTarget->AI())
                    {
                        creatureTarget->AI()->ResetToHome();
                        creatureTarget->AI()->AttackStart(summon);
                    }
                }
            }
        }
    }
}

void Spell::EffectSummonGuardian(SpellEffectIndex eff_idx)
{
    if (m_spellInfo->Id == 8376) //Earthgrab Totem
    {
        (*this.*SpellEffects[SPELL_EFFECT_SUMMON_TOTEM])(eff_idx);
        return;
    }

    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonGuardian: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    if(m_spellInfo->Id == 19805 || m_spellInfo->Id == 4071 || m_spellInfo->Id == 4072 || m_spellInfo->Id == 23004) { //Target Dummy - 19805 = Masterwork, 4071 = Standard, 4072 = Advanced, Alarm-o-Bot = 23004
        Creature* engineering_creature = new Creature();
        Map* map = m_caster->GetMap();

        CreatureCreatePos pos(m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

        if(!engineering_creature->Create(map->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cInfo))
        {
            sLog.outError("Spell::DoSummonGuardian: can't create creature entry %u for spell %u.", pet_entry, m_spellInfo->Id);
            delete engineering_creature;
            return;
        }


        engineering_creature->setFaction(m_caster->getFaction());
        engineering_creature->SetOwnerGuid(m_caster->GetGUID());
        engineering_creature->SetRespawnEnabled(false); //Disable respawn for the engineering creatures.


        engineering_creature->AIM_Initialize();

        map->Add(engineering_creature);


        return;
    }

    // set timer for unsummon
    int32 duration = CalculateSpellDuration(m_spellInfo, m_caster);

    // Search old Guardian only for players (if casted spell not have duration or cooldown)
    // FIXME: some guardians have control spell applied and controlled by player and anyway player can't summon in this time
    //        so this code hack in fact
    if (m_caster->GetTypeId() == TYPEID_PLAYER && (duration <= 0 || GetSpellRecoveryTime(m_spellInfo) == 0))
        if(m_caster->FindGuardianWithEntry(pet_entry))
            return;                                         // find old guardian, ignore summon

    // in another case summon new
    uint32 level = m_caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
            {
                level = skill202 / 5;
            }
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

    int32 amount;
    if (m_spellInfo->Id == 13166)  //If the spell is Gnomish Battle Chicken be sure to set the summoned amount to one instead of 800.
    {
        amount = 1;
    }
    else
    {
        amount = damage > 0 ? damage : 1;
    }

    for (int32 count = 0; count < amount; ++count)
    {
        Pet* spawnCreature = new Pet(GUARDIAN_PET);

        // If dest location if present
        // Summon 1 unit in dest location
        CreatureCreatePos pos(m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon in random point all other units if location present
            if (count > 0)
            {
                float x, y, z;
                m_caster->GetRandomPoint(center_x, center_y, center_z, radius, x, y, z);
                pos = CreatureCreatePos(m_caster->GetMap(), x, y, z, m_caster->GetOrientation());
            }
        }
        // Summon if dest location not present near caster
        else
            pos = CreatureCreatePos(m_caster, m_caster->GetOrientation());

        Map *map = m_caster->GetMap();
        uint32 pet_number = sObjectMgr.GeneratePetNumber();
        if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
        {
            sLog.outError("Spell::DoSummonGuardian: can't create creature entry %u for spell %u.", pet_entry, m_spellInfo->Id);
            delete spawnCreature;
            return;
        }

        spawnCreature->SetSummonPoint(pos);

        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        //spawnCreature->SetName("");                       // generated by client
        spawnCreature->SetOwnerGuid(m_caster->GetObjectGuid());
        spawnCreature->setPowerType(POWER_MANA);
        spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, spawnCreature->GetCreatureInfo()->npcflag);
        spawnCreature->setFaction(m_caster->getFaction());
        spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
        spawnCreature->SetCreatorGuid(m_caster->GetObjectGuid());
        spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

        spawnCreature->InitStatsForLevel(level, m_caster);
        spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);

        if (m_spellInfo->Id != 126)
        {
            spawnCreature->AIM_Initialize();
        }
        else	// The Eye of Kilrogg should have its ScriptdevAI even though it's a pet.
        {
            spawnCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
            spawnCreature->GetCharmInfo()->SetReactState(REACT_PASSIVE);
            spawnCreature->AIM_InitializeScriptdevAI();
        }

        m_caster->AddGuardian(spawnCreature);

        map->Add((Creature*)spawnCreature);

        // Notify Summoner
        if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
            ((Creature*)m_caster)->AI()->JustSummoned(spawnCreature);
    }

}

void Spell::EffectTeleUnitsFaceCaster(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (unitTarget->IsTaxiFlying())
        return;

    float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

    float fx, fy, fz;
    m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectBoundingRadius(), dis);

    unitTarget->NearTeleportTo(fx, fy, fz, -m_caster->GetOrientation(), unitTarget==m_caster);
}

void Spell::EffectLearnSkill(SpellEffectIndex eff_idx)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage < 0)
        return;

    uint32 skillid =  m_spellInfo->EffectMiscValue[eff_idx];
    uint16 skillval = ((Player*)unitTarget)->GetPureSkillValue(skillid);
    ((Player*)unitTarget)->SetSkill(skillid, skillval ? skillval : 1, damage * 75, damage);
}

void Spell::EffectAddHonor(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // honor-spells don't scale with level and won't be casted by an item
    // also we must use damage (spelldescription says +25 honor but damage is only 24)
    ((Player*)unitTarget)->AddHonorCP(float(damage),HONORABLE,0,0);
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "SpellEffect::AddHonor (spell_id %u) rewards %u honor points (non scale) for player: %u", m_spellInfo->Id, damage, ((Player*)unitTarget)->GetGUIDLow());
}

void Spell::EffectTradeSkill(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    // uint16 skillmax = ((Player*)unitTarget)->(skillid);
    // ((Player*)unitTarget)->SetSkill(skillid,skillval?skillval:1,skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    // not grow at item use at item case
    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(),p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1,itemTarget->GetEntry(),
                        item_owner->GetName(),item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,false);

    itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,true);
}

void Spell::EffectEnchantItemTmp(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    if (!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];

    // Shaman Rockbiter Weapon		[-ZERO] Constant AP bonus in 1.12.1
    //if (eff_idx==EFFECT_INDEX_0 && m_spellInfo->Effect[EFFECT_INDEX_1]==SPELL_EFFECT_DUMMY)
    //{
    //    int32 enchnting_damage = m_currentBasePoints[EFFECT_INDEX_1];

    //    // enchanting id selected by calculated damage-per-sec stored in Effect[1] base value
    //    // with already applied percent bonus from Elemental Weapons talent
    //    // Note: damage calculated (correctly) with rounding int32(float(v)) but
    //    // RW enchantments applied damage int32(float(v)+0.5), this create  0..1 difference sometime
    //    switch(enchnting_damage)
    //    {
    //        // Rank 1
    //        case  2: enchant_id =   29; break;              //  0% [ 7% ==  2, 14% == 2, 20% == 2]
    //        // Rank 2
    //        case  4: enchant_id =    6; break;              //  0% [ 7% ==  4, 14% == 4]
    //        case  5: enchant_id = 3025; break;              // 20%
    //        // Rank 3
    //        case  6: enchant_id =    1; break;              //  0% [ 7% ==  6, 14% == 6]
    //        case  7: enchant_id = 3027; break;              // 20%
    //        // Rank 4
    //        case  9: enchant_id = 3032; break;              //  0% [ 7% ==  6]
    //        case 10: enchant_id =  503; break;              // 14%
    //        case 11: enchant_id = 3031; break;              // 20%
    //        // Rank 5
    //        case 15: enchant_id = 3035; break;              // 0%
    //        case 16: enchant_id = 1663; break;              // 7%
    //        case 17: enchant_id = 3033; break;              // 14%
    //        case 18: enchant_id = 3034; break;              // 20%
    //        // Rank 6
    //        case 28: enchant_id = 3038; break;              // 0%
    //        case 29: enchant_id =  683; break;              // 7%
    //        case 31: enchant_id = 3036; break;              // 14%
    //        case 33: enchant_id = 3037; break;              // 20%
    //        // Rank 7
    //        case 40: enchant_id = 3041; break;              // 0%
    //        case 42: enchant_id = 1664; break;              // 7%
    //        case 45: enchant_id = 3039; break;              // 14%
    //        case 48: enchant_id = 3040; break;              // 20%
    //        // Rank 8
    //        case 49: enchant_id = 3044; break;              // 0%
    //        case 52: enchant_id = 2632; break;              // 7%
    //        case 55: enchant_id = 3042; break;              // 14%
    //        case 58: enchant_id = 3043; break;              // 20%
    //        // Rank 9
    //        case 62: enchant_id = 2633; break;              // 0%
    //        case 66: enchant_id = 3018; break;              // 7%
    //        case 70: enchant_id = 3019; break;              // 14%
    //        case 74: enchant_id = 3020; break;              // 20%
    //        default:
    //            sLog.outError("Spell::EffectEnchantItemTmp: Damage %u not handled in S'RW",enchnting_damage);
    //            return;
    //    }
    //}

    if (!enchant_id)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id",m_spellInfo->Id,eff_idx);
        return;
    }

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if(!pEnchant)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have nonexistent enchanting id %u ",m_spellInfo->Id,eff_idx,enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // rogue family enchantments exception by duration
    if(m_spellInfo->Id == 38615)                            // Poison
        duration = 1800;                                    // 30 mins
    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    else if(m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 3600;                                    // 1 hour
    // shaman family enchantments - only works for windfury
    else if(m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 300;                                     // 30 mins
    // other cases with this SpellVisual already selected
    else if(m_spellInfo->SpellVisual==215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses
    else if(m_spellInfo->SpellVisual==563)
        duration = 600;                                     // 10 mins
    // This doesn't work for rockbiter so what is it for?
    else if(m_spellInfo->SpellVisual==0)
        duration = 1800;                                    // 30 mins
    else if(m_spellInfo->Id == 29702)                       // Greater Ward of Shielding
        duration = 300;                                     // 5 mins
    else if(m_spellInfo->Id == 37360)                       // Consecrated Weapon
        duration = 300;                                     // 5 mins
    else if (m_spellInfo->Id == 8017 ||
             m_spellInfo->Id == 8018 ||
             m_spellInfo->Id == 8019 ||
             m_spellInfo->Id == 10399 ||
             m_spellInfo->Id == 16314 ||
             m_spellInfo->Id == 16315 ||
             m_spellInfo->Id == 16316)						// Rockbiter weapon
        duration = 300;
    else if (m_spellInfo->Id == 8024 ||
             m_spellInfo->Id == 8027 ||
             m_spellInfo->Id == 8030 ||
             m_spellInfo->Id == 16339 ||
             m_spellInfo->Id == 16341 ||
             m_spellInfo->Id == 16342)						// Flametongue weapon
        duration = 300;
    else if (m_spellInfo->Id == 8033 ||
             m_spellInfo->Id == 8038 ||
             m_spellInfo->Id == 10456 ||
             m_spellInfo->Id == 16355 ||
             m_spellInfo->Id == 16356)						// Frostbrand weapon
        duration = 300;

    // default case
    else
        duration = 3600;                                    // 1 hour

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if(!item_owner)
        return;

    if(item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1, itemTarget->GetEntry(),
                        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget,TEMP_ENCHANTMENT_SLOT, false);

    //Charges
    uint32 charges = 0;
    if (const SpellEntry* pSpell = sSpellStore.LookupEntry(pEnchant->spellid[0]))
        if (pSpell->SpellFamilyName == SPELLFAMILY_ROGUE && pSpell->SpellFamilyFlags & 268541952)
        {
            uint32 charges_growth = 0;
            switch (sSpellMgr.GetFirstSpellInChain(pSpell->Id))
            {
            case 8680:				//Instant Poison
                charges = 40;
                charges_growth = 15;
                break;
            case 2818:			   //Deadly Poison
            case 13218:			   //Wound Poison
                charges = 60;
                charges_growth = 15;
                break;
            case 5760:			  //Mind-Numbing Poison
                charges = 50;
                charges_growth = 25;
                break;
            default:
                break;
            }
            charges = charges + (sSpellMgr.GetSpellRank(pSpell->Id) - 1)*charges_growth;
        }

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration * 1000, charges);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);
}

void Spell::EffectTameCreature(SpellEffectIndex /*eff_idx*/)
{
    // Caster must be player, checked in Spell::CheckCast
    // Spell can be triggered, we need to check original caster prior to caster
    Player* plr = (Player*)GetAffectiveCaster();

    Creature* creatureTarget = (Creature*)unitTarget;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Pet* pet = new Pet(HUNTER_PET);

    if(!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return;
    }

    pet->SetOwnerGuid(plr->GetObjectGuid());
    pet->SetCreatorGuid(plr->GetObjectGuid());
    pet->setFaction(plr->getFaction());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    if (plr->IsPvP())
        pet->SetPvP(true);

    if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
    {
        sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!",creatureTarget->GetEntry());
        delete pet;
        return;
    }

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->AIM_Initialize();
    pet->InitPetCreateSpells();
    pet->SetHealth(pet->GetMaxHealth());

    // "kill" original creature
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL,creatureTarget->getLevel()-1);

    // add to world
    pet->GetMap()->Add((Creature*)pet);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL,creatureTarget->getLevel());

    // caster have pet now
    plr->SetPet(pet);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    plr->PetSpellInitialize();
}

void Spell::EffectSummonPet(SpellEffectIndex eff_idx)
{
    uint32 petentry = m_spellInfo->EffectMiscValue[eff_idx];

    Pet *OldSummon = m_caster->GetPet();

    // if pet requested type already exist
    if( OldSummon )
    {
        if(petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if( OldSummon->isDead() )
                return;

            OldSummon->GetMap()->Remove((Creature*)OldSummon,false);

            float px, py, pz;
            m_caster->GetClosePoint(px, py, pz, OldSummon->GetObjectBoundingRadius());

            OldSummon->Relocate(px, py, pz, OldSummon->GetOrientation());
            m_caster->GetMap()->Add((Creature*)OldSummon);

            if(m_caster->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled() )
            {
                ((Player*)m_caster)->PetSpellInitialize();
            }
            return;
        }

        if(m_caster->GetTypeId() == TYPEID_PLAYER)
            OldSummon->Unsummon(OldSummon->getPetType() == HUNTER_PET ? PET_SAVE_AS_DELETED : PET_SAVE_NOT_IN_SLOT, m_caster);
        else
            return;
    }

    CreatureInfo const* cInfo = petentry ? sCreatureStorage.LookupEntry<CreatureInfo>(petentry) : NULL;

    // == 0 in case call current pet, check only real summon case
    if (petentry && !cInfo)
    {
        sLog.outErrorDb("EffectSummonPet: creature entry %u not found for spell %u.", petentry, m_spellInfo->Id);
        return;
    }

    Pet* NewSummon = new Pet;

    // petentry==0 for hunter "call pet" (current pet summoned if any)
    if(m_caster->GetTypeId() == TYPEID_PLAYER && NewSummon->LoadPetFromDB((Player*)m_caster,petentry))
    {
        if(NewSummon->getPetType()==SUMMON_PET)
        {
            // Remove Demonic Sacrifice auras (known pet)
            Unit::AuraList const& auraClassScripts = m_caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
            for(Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr!=auraClassScripts.end();)
            {
                if((*itr)->GetModifier()->m_miscvalue == 2228)
                {
                    m_caster->RemoveAurasDueToSpell((*itr)->GetId());
                    itr = auraClassScripts.begin();
                }
                else
                    ++itr;
            }
        }

        return;
    }

    // not error in case fail hunter call pet
    if(!petentry)
    {
        delete NewSummon;
        return;
    }

    CreatureCreatePos pos(m_caster, m_caster->GetOrientation());

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!NewSummon->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        delete NewSummon;
        return;
    }

    NewSummon->SetSummonPoint(pos);

    uint32 petlevel = m_caster->getLevel();
    NewSummon->setPetType(SUMMON_PET);

    uint32 faction = m_caster->getFaction();
    if(m_caster->GetTypeId() == TYPEID_UNIT)
        NewSummon->GetCharmInfo()->SetReactState(REACT_AGGRESSIVE);

    NewSummon->SetOwnerGuid(m_caster->GetObjectGuid());
    NewSummon->SetCreatorGuid(m_caster->GetObjectGuid());
    NewSummon->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    NewSummon->setFaction(faction);
    NewSummon->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));
    NewSummon->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    NewSummon->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    NewSummon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    NewSummon->UpdateWalkMode(m_caster);

    NewSummon->GetCharmInfo()->SetPetNumber(pet_number, true);
    // this enables pet details window (Shift+P)

    if(m_caster->IsPvP())
        NewSummon->SetPvP(true);

    NewSummon->InitStatsForLevel(petlevel, m_caster);
    NewSummon->InitPetCreateSpells();

    if(NewSummon->getPetType()==SUMMON_PET)
    {
        // Remove Demonic Sacrifice auras (new pet)
        Unit::AuraList const& auraClassScripts = m_caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for(Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr!=auraClassScripts.end();)
        {
            if((*itr)->GetModifier()->m_miscvalue==2228)
            {
                m_caster->RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }

        // generate new name for summon pet
        std::string new_name=sObjectMgr.GeneratePetName(petentry);
        if(!new_name.empty())
            NewSummon->SetName(new_name);
    }
    else if(NewSummon->getPetType()==HUNTER_PET)
        NewSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_RENAME);

    NewSummon->AIM_Initialize();
    NewSummon->SetHealth(NewSummon->GetMaxHealth());
    NewSummon->SetPower(POWER_MANA, NewSummon->GetMaxPower(POWER_MANA));

    map->Add((Creature*)NewSummon);

    m_caster->SetPet(NewSummon);
    DEBUG_LOG("New Pet has guid %u", NewSummon->GetGUIDLow());

    // To make sure warlocks get spawned with the correct pet in BGs.
    Player* player_caster = dynamic_cast<Player*>(m_caster);
    if (player_caster)
        player_caster->SetWarlockPetBeforeDeath();

    if(m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        NewSummon->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_caster)->PetSpellInitialize();
    }
}

void Spell::EffectLearnPetSpell(SpellEffectIndex eff_idx)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;
    if(!pet->isAlive())
        return;

    SpellEntry const *learn_spellproto = sSpellStore.LookupEntry(m_spellInfo->EffectTriggerSpell[eff_idx]);
    if(!learn_spellproto)
        return;

    pet->SetTP(pet->m_TrainingPoints - pet->GetTPForSpell(learn_spellproto->Id));
    pet->learnSpell(learn_spellproto->Id);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    _player->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget)
        return;

    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->getVictim()==m_caster)
        {
            SendCastResult(SPELL_FAILED_DONT_REPORT);
            return;
        }
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->CanHaveThreatList() && unitTarget->getThreatManager().getCurrentVictim())
    {
        float current_caster_threat = unitTarget->getThreatManager().getThreat(m_caster);
        unitTarget->getThreatManager().addThreatDirectly(m_caster, -current_caster_threat);												// Remove the caster's current threat.
        unitTarget->getThreatManager().addThreatDirectly(m_caster,unitTarget->getThreatManager().getCurrentVictim()->getThreat());		// Add the current target's threat to the caster.
    }
}

void Spell::EffectWeaponDmg(SpellEffectIndex eff_idx)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        switch(m_spellInfo->Effect[j])
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            if (j < int(eff_idx))                             // we must calculate only at last weapon effect
                return;
            break;
        }
    }

    // some spell specific modifiers
    bool customBonusDamagePercentMod = false;
    float bonusDamagePercentMod  = 1.0f;                    // applied to fixed effect damage bonus if set customBonusDamagePercentMod
    float weaponDamagePercentMod = 1.0f;                    // applied to weapon damage (and to fixed effect damage bonus if customBonusDamagePercentMod not set
    float totalDamagePercentMod  = 1.0f;                    // applied to final bonus+weapon damage
    bool normalized = false;

    int32 spell_bonus = 0;                                  // bonus specific for spell
    switch(m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_ROGUE:
    {
        // Ambush
        if(m_spellInfo->SpellFamilyFlags & UI64LIT(0x00000200))
        {
            customBonusDamagePercentMod = true;
            bonusDamagePercentMod = 2.5f;               // 250%
        }
        break;
    }
    case SPELLFAMILY_PALADIN:
    {
        // Seal of Command - receive benefit from Spell Damage and Healing
        if(m_spellInfo->SpellFamilyFlags & UI64LIT(0x00000002000000))
        {
            spell_bonus += int32(0.20f*m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo)));
            spell_bonus += int32(0.29f*unitTarget->SpellBaseDamageBonusTaken(GetSpellSchoolMask(m_spellInfo)));
        }
        break;
    }
    }

    int32 fixed_bonus = 0;
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        switch(m_spellInfo->Effect[j])
        {
        case SPELL_EFFECT_WEAPON_DAMAGE:
        case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            fixed_bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);
            break;
        case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            fixed_bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);
            normalized = true;
            break;
        case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            weaponDamagePercentMod *= float(CalculateDamage(SpellEffectIndex(j), unitTarget)) / 100.0f;

            // applied only to prev.effects fixed damage
            if(customBonusDamagePercentMod)
                fixed_bonus = int32(fixed_bonus*bonusDamagePercentMod);
            else
                fixed_bonus = int32(fixed_bonus*weaponDamagePercentMod);
            break;
        default:
            break;                                      // not weapon damage effect, just skip
        }
    }

    // non-weapon damage
    int32 bonus = spell_bonus + fixed_bonus;

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if(bonus)
    {
        UnitMods unitMod;
        switch(m_attackType)
        {
        default:
        case BASE_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_MAINHAND;
            break;
        case OFF_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_OFFHAND;
            break;
        case RANGED_ATTACK:
            unitMod = UNIT_MOD_DAMAGE_RANGED;
            break;
        }

        float weapon_total_pct  = m_caster->GetModifierValue(unitMod, TOTAL_PCT);
        bonus = int32(bonus*weapon_total_pct);
    }

    // + weapon damage with applied weapon% dmg to base weapon damage in call
    bonus += int32(m_caster->CalculateDamage(m_attackType, normalized)*weaponDamagePercentMod);

    // total damage
    bonus = int32(bonus*totalDamagePercentMod);

    // prevent negative damage
    m_damage+= uint32(bonus > 0 ? bonus : 0);

    // Mangle (Cat): CP
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000040000000000)))
    {
        if(m_caster->GetTypeId()==TYPEID_PLAYER)
            ((Player*)m_caster)->AddComboPoints(unitTarget, 1);
    }

    // take ammo
    if(m_attackType == RANGED_ATTACK && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item *pItem = ((Player*)m_caster)->GetWeaponForAttack(RANGED_ATTACK, true, false);

        // wands don't have ammo
        if (!pItem || pItem->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if (pItem->GetProto()->InventoryType == INVTYPE_THROWN)
        {
            if(pItem->GetMaxStackCount()==1)
            {
                // decrease durability for non-stackable throw weapon
                ((Player*)m_caster)->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_RANGED);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                ((Player*)m_caster)->DestroyItemCount( pItem, count, true);
            }
        }
        else if(uint32 ammo = ((Player*)m_caster)->GetUInt32Value(PLAYER_AMMO_ID))
            ((Player*)m_caster)->DestroyItemCount(ammo, 1, true);
    }
}

void Spell::EffectThreat(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget || !unitTarget->isAlive() || !m_caster->isAlive())
        return;

    if(!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_caster, float(damage), false, GetSpellSchoolMask(m_spellInfo), m_spellInfo);
}

void Spell::EffectHealMaxHealth(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;
    uint32 heal = m_caster->GetMaxHealth();

    m_healing += heal;
}

void Spell::EffectInterruptCast(SpellEffectIndex eff_idx)
{
    if(m_spellInfo->Id == 19675 && eff_idx == EFFECT_INDEX_2) // Initial Feral charge interrupt.
    {
        if(m_caster->getVictim())
            unitTarget = m_caster->getVictim();
    }
    else if(m_spellInfo->Id == 19675 && eff_idx != EFFECT_INDEX_2) // Immobolizing part of charge should not interrupt.
    {
        return;
    }

    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    // TODO: not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            SpellEntry const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((curSpellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT) && curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE )
            {
                unitTarget->ProhibitSpellSchool(GetSpellSchoolMask(curSpellInfo), GetSpellDuration(m_spellInfo));
                unitTarget->InterruptSpell(CurrentSpellTypes(i),false);
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffectIndex eff_idx)
{
    uint32 gameobject_id = m_spellInfo->EffectMiscValue[eff_idx];

    if(gameobject_id == 180660) // Spell Place Loot - AQ quest chain.
    {
        gameobject_id = 500;
    }

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if( !target )
        target = m_caster;

    float x, y, z;
    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = target->GetMap();

    if(!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
                         x, y, z, target->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    // Wild object not have owner and check clickable by players
    map->Add(pGameObj);

    if(pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player *pl = (Player*)m_caster;
        BattleGround* bg = ((Player *)m_caster)->GetBattleGround();

        switch(pGameObj->GetMapId())
        {
        case 489:                                       //WS
        {
            if(bg && bg->GetTypeID()==BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
            {
                Team team = ALLIANCE;
                if(pl->GetTeam() == team)
                    team = HORDE;

                ((BattleGroundWS*)bg)->SetDroppedFlagGuid(pGameObj->GetObjectGuid(), team);
            }
            break;
        }
        }
    }


    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);

    if (m_spellInfo->Id == 19250 && focusObject)		//Placing Smokey's Explosives
        focusObject->SetOwnerGuid(m_caster->GetObjectGuid());
    if(m_spellInfo->Id == 19588 && m_caster && duration && pGameObj)
    {
        m_caster->SummonGameObject(177749, duration, pGameObj->GetPositionX(), pGameObj->GetPositionY(), pGameObj->GetPositionZ(), pGameObj->GetOrientation());
        m_caster->SummonCreature(800010,  pGameObj->GetPositionX(), pGameObj->GetPositionY(), pGameObj->GetPositionZ(), pGameObj->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, duration);
    }
    else if(m_spellInfo->Id == 25720 && m_caster)
    {
       Creature *weavilFLyingMachine = m_caster->SummonCreature(15553, 5096.95f, -5171.80f, 940.66f, 1.75f, TEMPSUMMON_TIMED_DESPAWN, 120000);
       
       if(weavilFLyingMachine)
       {
           weavilFLyingMachine->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
           weavilFLyingMachine->SetOwnerGuid(m_caster->GetObjectGuid());
           weavilFLyingMachine->SetHover(true);
           weavilFLyingMachine->SetSplineFlags(SPLINEFLAG_FLYING);
           weavilFLyingMachine->MonsterMove(5086.29f, -5114.77f, 935.78f, 5000);
       }
    }
}

void Spell::EffectScriptEffect(SpellEffectIndex eff_idx)
{
    // TODO: we must implement hunter pet summon at login there (spell 6962)

    switch(m_spellInfo->SpellFamilyName)
    {
    case SPELLFAMILY_GENERIC:
    {
        switch(m_spellInfo->Id)
        {
        case 8856:                                  // Bending Shinbone
        {
            if (!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            uint32 spell_id = 0;
            switch(urand(1,5))
            {
            case 1:
                spell_id = 8854;
                break;
            default:
                spell_id = 8855;
                break;
            }

            m_caster->CastSpell(m_caster,spell_id,true,NULL);
            return;
        }
        case 17512:                                 // Piccolo of the Flaming Fire
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            unitTarget->HandleEmoteCommand(EMOTE_STATE_DANCE);

            return;
        }
        case 18947:                                 // Inferno Dummy Effect (Baron Geddon)
        {
            Unit::AuraList const& auraList = m_caster->GetAurasByType(SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            for (Unit::AuraList::const_iterator itr = auraList.begin(); itr != auraList.end(); ++itr)
            {
                if ((*itr)->GetSpellProto()->SpellIconID == 937 && (*itr)->GetId() == 19695)
                    damage=500*(*itr)->GetAuraTicks();
            }
            m_caster->CastCustomSpell(m_caster, 19698, &damage, NULL, NULL, true);
            return;
        }
        case 22539:                                 // Shadow Flame (All script effects, not just end ones to
        case 22972:                                 // prevent player from dodging the last triggered spell)
        case 22975:
        case 22976:
        case 22977:
        case 22978:
        case 22979:
        case 22980:
        case 22981:
        case 22982:
        case 22983:
        case 22984:
        case 22985:
        {
            if (!unitTarget || !unitTarget->isAlive())
                return;

            // Onyxia Scale Cloak
            if (unitTarget->GetDummyAura(22683))
                return;

            // Shadow Flame
            m_caster->CastSpell(unitTarget, 22682, true);
            return;
        }
        case 24194:                                 // Uther's Tribute
        case 24195:                                 // Grom's Tribute
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            uint8 race = m_caster->getRace();
            uint32 spellId = 0;

            switch(m_spellInfo->Id)
            {
            case 24194:
                switch(race)
                {
                case RACE_HUMAN:
                    spellId = 24105;
                    break;
                case RACE_DWARF:
                    spellId = 24107;
                    break;
                case RACE_NIGHTELF:
                    spellId = 24108;
                    break;
                case RACE_GNOME:
                    spellId = 24106;
                    break;
                }
                break;
            case 24195:
                switch(race)
                {
                case RACE_ORC:
                    spellId = 24104;
                    break;
                case RACE_UNDEAD:
                    spellId = 24103;
                    break;
                case RACE_TAUREN:
                    spellId = 24102;
                    break;
                case RACE_TROLL:
                    spellId = 24101;
                    break;
                }
                break;
            }

            if (spellId)
                m_caster->CastSpell(m_caster, spellId, true);

            return;
        }
        case 24320:                                 // Poisonous Blood
        {
            unitTarget->CastSpell(unitTarget, 24321, true, NULL, NULL, m_caster->GetObjectGuid());
            return;
        }
        case 24324:                                 // Blood Siphon
        {
            if (m_currentBasePoints[eff_idx] == (int32) m_spellInfo->Id)
                return;

            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            unitTarget->StopMoving();
            unitTarget->AttackStop();
            unitTarget->InterruptNonMeleeSpells(false);

            if (unitTarget->HasAura(24321, EFFECT_INDEX_0))
            {
                unitTarget->RemoveAurasDueToSpell(24321);
                unitTarget->CastSpell(m_caster, 24323, true);
            }
            else
                unitTarget->CastSpell(m_caster, 24322, true);

            return;
        }
        case 24590:                                 // Brittle Armor - need remove one 24575 Brittle Armor aura
            unitTarget->RemoveAuraHolderFromStack(24575);
            return;
        case 24714:                                 // Trick
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            if (roll_chance_i(50))                  // Trick (can be different critter models). Upped this to 50% because it's cool
                m_caster->CastSpell(m_caster, 24753, true);
            else                                    // Random Costume, 6 different (plus add. for gender)
                m_caster->CastSpell(m_caster, 24720, true);

            return;
        }
        case 24717:                                 // Pirate Costume
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Pirate Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24708 : 24709, true);
            return;
        }
        case 24718:                                 // Ninja Costume
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Ninja Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24711 : 24710, true);
            return;
        }
        case 24719:                                 // Leper Gnome Costume
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Leper Gnome Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24712 : 24713, true);
            return;
        }
        case 24720:                                 // Random Costume
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spellId = 0;

            switch(urand(0, 6))
            {
            case 0:
                spellId = unitTarget->getGender() == GENDER_MALE ? 24708 : 24709;
                break;
            case 1:
                spellId = unitTarget->getGender() == GENDER_MALE ? 24711 : 24710;
                break;
            case 2:
                spellId = unitTarget->getGender() == GENDER_MALE ? 24712 : 24713;
                break;
            case 3:
                spellId = 24723;
                break;
            case 4:
                spellId = 24732;
                break;
            case 5:
                spellId = unitTarget->getGender() == GENDER_MALE ? 24735 : 24736;
                break;
            case 6:
                spellId = 24740;
                break;
            }

            m_caster->CastSpell(unitTarget, spellId, true);
            return;
        }
        case 24737:                                 // Ghost Costume
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Ghost Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24735 : 24736, true);
            return;
        }
        case 24751:                                 // Trick or Treat
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Tricked or Treated
            unitTarget->CastSpell(unitTarget, 24755, true);

            // Treat / Trick
            unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 24714 : 24715, true);
            return;
        }
        case 26004:                                 // Mistletoe
        {
            if (!unitTarget)
                return;

            unitTarget->HandleEmote(EMOTE_ONESHOT_CHEER);
            return;
        }
        case 26218:                                 // Mistletoe
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spells[3] = {26206, 26207, 45036};

            m_caster->CastSpell(unitTarget, spells[urand(0, 2)], true);
            return;
        }
        case 26275:                                 // PX-238 Winter Wondervolt TRAP
        {
            uint32 spells[4] = {26272, 26157, 26273, 26274};

            // check presence
            for(int j = 0; j < 4; ++j)
                if (unitTarget->HasAura(spells[j], EFFECT_INDEX_0))
                    return;

            unitTarget->CastSpell(unitTarget, spells[urand(0,3)], true);
            return;
        }
        case 26465:                                 // Mercurial Shield - need remove one 26464 Mercurial Shield aura
            unitTarget->RemoveAuraHolderFromStack(26464);
            return;
        case 26656:                                 // Summon Black Qiraji Battle Tank
        {
            if (!unitTarget)
                return;

            // Prevent stacking of mounts
            unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

            // Two separate mounts depending on area id (allows use both in and out of specific instance)
            if (unitTarget->GetAreaId() == 3428)
                unitTarget->CastSpell(unitTarget, 25863, false);
            else
                unitTarget->CastSpell(unitTarget, 26655, false);

            return;
        }

        case 28560:                                 // Summon Blizzard
        {
            if (!unitTarget)
                return;

            m_caster->SummonCreature(16474, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);
            return;
        }
        //case 30918:                                 // Improved Sprint
        //{
        //    if (!unitTarget)
        //        return;

        //    // Removes snares and roots.
        //    unitTarget->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK,30918,true);
        //    break;
        //}
        }
        break;
    }
    case SPELLFAMILY_SHAMAN:
    {
        if (m_spellInfo->Id == 20871) // Correct threat for Rockbiter Weapon Rank 7.
        {
            Unit* caster = GetCaster();
            if (caster)
            {
                float attack_speed = caster->GetAttackTime(BASE_ATTACK);
                if (unitTarget->CanHaveThreatList())
                    unitTarget->getThreatManager().addThreatDirectly(caster, 72 * attack_speed / 1000.f);

            }

            return;
        }
    }
    case SPELLFAMILY_WARLOCK:
    {
        switch(m_spellInfo->Id)
        {
        case  6201:                                 // Healthstone creating spells
        case  6202:
        case  5699:
        case 11729:
        case 11730:
        {
            if (!unitTarget)
                return;

            uint32 itemtype;
            uint32 rank = 0;
            Unit::AuraList const& mDummyAuras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
            {
                if ((*i)->GetId() == 18692)
                {
                    rank = 1;
                    break;
                }
                else if ((*i)->GetId() == 18693)
                {
                    rank = 2;
                    break;
                }
            }

            static uint32 const itypes[5][3] =
            {
                { 5512, 19004, 19005},              // Minor Healthstone
                { 5511, 19006, 19007},              // Lesser Healthstone
                { 5509, 19008, 19009},              // Healthstone
                { 5510, 19010, 19011},              // Greater Healthstone
                { 9421, 19012, 19013}               // Major Healthstone
            };

            switch(m_spellInfo->Id)
            {
            case  6201:
                itemtype=itypes[0][rank];
                break; // Minor Healthstone
            case  6202:
                itemtype=itypes[1][rank];
                break; // Lesser Healthstone
            case  5699:
                itemtype=itypes[2][rank];
                break; // Healthstone
            case 11729:
                itemtype=itypes[3][rank];
                break; // Greater Healthstone
            case 11730:
                itemtype=itypes[4][rank];
                break; // Major Healthstone
            default:
                return;
            }
            DoCreateItem( eff_idx, itemtype );
            return;
        }
        }
        break;
    }
    case SPELLFAMILY_PALADIN:
    {
        // Holy Light
        if (m_spellInfo->SpellIconID == 70)
        {
            if(!unitTarget || !unitTarget->isAlive())
                return;
            int32 heal = damage;
            int32 spellid = m_spellInfo->Id;            // send main spell id as basepoints for not used effect
            m_caster->CastCustomSpell(unitTarget,19968,&heal,&spellid,NULL,true);
        }
        // Flash of Light
        else if (m_spellInfo->SpellIconID  == 242)
        {
            if(!unitTarget || !unitTarget->isAlive())
                return;
            int32 heal = damage;
            int32 spellid = m_spellInfo->Id;            // send main spell id as basepoints for not used effect
            m_caster->CastCustomSpell(unitTarget,19993,&heal,&spellid,NULL,true);
        }
        else if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000800000))
        {
            if (!unitTarget || !unitTarget->isAlive())
                return;
            uint32 spellId2 = 0;

            // all seals have aura dummy
            Unit::AuraList const& m_dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator itr = m_dummyAuras.begin(); itr != m_dummyAuras.end(); ++itr)
            {
                SpellEntry const *spellInfo = (*itr)->GetSpellProto();

                // search seal (all seals have judgement's aura dummy spell id in 2 effect
                if ( !spellInfo || !IsSealSpell((*itr)->GetSpellProto()) || (*itr)->GetEffIndex() != 2 )
                    continue;

                // must be calculated base at raw base points in spell proto, GetModifier()->m_value for S.Righteousness modified by SPELLMOD_DAMAGE
                spellId2 = (*itr)->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_2);

                if(spellId2 <= 1)
                    continue;

                // found, remove seal
                m_caster->RemoveAurasDueToSpell((*itr)->GetId());

                break;
            }

            m_caster->CastSpell(unitTarget,spellId2,true);
            return;
        }
        break;
    }
    }

    // normal DB scripted effect
    if (!unitTarget)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart spellid %u in EffectScriptEffect ", m_spellInfo->Id);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
}

void Spell::EffectSanctuary(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget)
        return;
    //unitTarget->CombatStop();

    unitTarget->CombatStop();
    unitTarget->getHostileRefManager().deleteReferences();  // stop all fighting

    // Vanish allows to remove all threat and cast regular stealth so other spells can be used
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000000000800)))
    {
        ((Player *)m_caster)->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);

        // Interrup all spells being cast against the player.
        Unit::AttackerSet const& attackers = m_caster->getAttackers();
        if (!attackers.empty())
            std::for_each(attackers.begin(), attackers.end(), [&](Unit* current_attacker) {
            current_attacker->InterruptSpellsTargettingUnit(false, m_caster->GetObjectGuid());
        });
    }
}

void Spell::EffectAddComboPoints(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget)
        return;

    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if(damage <= 0)
        return;

    ((Player*)m_caster)->AddComboPoints(unitTarget, damage);
}

void Spell::EffectDuel(SpellEffectIndex eff_idx)
{
    if(!m_caster || !unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *caster = (Player*)m_caster;
    Player *target = (Player*)unitTarget;

    // caster or target already have requested duel
    if (caster->duel || target->duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetObjectGuid()))
        return;

    // Players can only fight a duel with each other outside (=not inside dungeons and not in capital cities)
    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(caster->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* targetAreaEntry = GetAreaEntryByAreaID(target->GetAreaId());
    if (targetAreaEntry && !(targetAreaEntry->flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = m_spellInfo->EffectMiscValue[eff_idx];

    Map *map = m_caster->GetMap();
    if(!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
                         m_caster->GetPositionX()+(unitTarget->GetPositionX()-m_caster->GetPositionX())/2 ,
                         m_caster->GetPositionY()+(unitTarget->GetPositionY()-m_caster->GetPositionY())/2 ,
                         m_caster->GetPositionZ(),
                         m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, m_caster->getFaction() );
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel()+1 );
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    m_caster->AddGameObject(pGameObj);
    map->Add(pGameObj);
    //END

    // Send request
    WorldPacket data(SMSG_DUEL_REQUESTED, 8 + 8);
    data << pGameObj->GetObjectGuid();
    data << caster->GetObjectGuid();
    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create duel-info
    DuelInfo *duel   = new DuelInfo;
    duel->initiator  = caster;
    duel->opponent   = target;
    duel->startTime  = 0;
    duel->startTimer = 0;
    caster->duel     = duel;

    DuelInfo *duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster;
    duel2->startTime  = 0;
    duel2->startTimer = 0;
    target->duel      = duel2;

    caster->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());
    target->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());
}

void Spell::EffectStuck(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!sWorld.getConfig(CONFIG_BOOL_CAST_UNSTUCK))
        return;

    Player* pTarget = (Player*)unitTarget;

    DEBUG_LOG("Spell Effect: Stuck");
    DETAIL_LOG("Player %s (guid %u) used auto-unstuck future at map %u (%f, %f, %f)", pTarget->GetName(), pTarget->GetGUIDLow(), m_caster->GetMapId(), m_caster->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

    if(pTarget->IsTaxiFlying())
        return;

    // homebind location is loaded always
    pTarget->TeleportToHomebind(unitTarget==m_caster ? TELE_TO_SPELL : 0);

    // Stuck spell trigger Hearthstone cooldown
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(8690);
    if(!spellInfo)
        return;
    Spell spell(pTarget, spellInfo, true);
    spell.SendSpellCooldown();
}

void Spell::EffectSummonPlayer(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if(unitTarget->GetDummyAura(23445))
        return;

    // If we're in an instance we drop the summoned player on the warlock to avoid exploits.
    if (m_caster->GetMap()->GetInstanceData() == nullptr)
    {
        float x, y, z;
        m_caster->GetClosePoint(x, y, z, unitTarget->GetObjectBoundingRadius());

        ((Player*)unitTarget)->SetSummonPoint(m_caster->GetMapId(),x,y,z);
    }
    else
        ((Player*)unitTarget)->SetSummonPoint(m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());

    WorldPacket data(SMSG_SUMMON_REQUEST, 8+4+4);
    data << m_caster->GetObjectGuid();                      // summoner guid
    data << uint32(m_caster->GetZoneId());                  // summoner zone
    data << uint32(MAX_PLAYER_SUMMON_DELAY*IN_MILLISECONDS); // auto decline after msecs
    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

static ScriptInfo generateActivateCommand()
{
    ScriptInfo si;
    si.command = SCRIPT_COMMAND_ACTIVATE_OBJECT;
    return si;
}

void Spell::EffectActivateObject(SpellEffectIndex eff_idx)
{
    if(!gameObjTarget)
        return;

    static ScriptInfo activateCommand = generateActivateCommand();

    int32 delay_secs = m_spellInfo->CalculateSimpleValue(eff_idx);

    gameObjTarget->GetMap()->ScriptCommandStart(activateCommand, delay_secs, m_caster, gameObjTarget);
}

void Spell::EffectSummonTotem(SpellEffectIndex eff_idx)
{
    int slot = 0;
    if (m_spellInfo->Id == 8376) //Earthgrab Totem
        slot = TOTEM_SLOT_NONE;
    else if (m_spellInfo->Id == 13166) //Gnomish Battle Chicken
    {
        (*this.*SpellEffects[SPELL_EFFECT_SUMMON_GUARDIAN])(eff_idx);
        return;
    }
    else
    {
        switch(m_spellInfo->Effect[eff_idx])
        {
        case SPELL_EFFECT_SUMMON_TOTEM:
            slot = TOTEM_SLOT_NONE;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT1:
            slot = TOTEM_SLOT_FIRE;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT2:
            slot = TOTEM_SLOT_EARTH;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT3:
            slot = TOTEM_SLOT_WATER;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT4:
            slot = TOTEM_SLOT_AIR;
            break;
        default:
            return;
        }
    }

    // unsummon old totem
    if (slot < MAX_TOTEM_SLOT)
        if (Totem *OldTotem = m_caster->GetTotem(TotemSlot(slot)))
            OldTotem->UnSummon();

    // FIXME: Setup near to finish point because GetObjectBoundingRadius set in Create but some Create calls can be dependent from proper position
    // if totem have creature_template_addon.auras with persistent point for example or script call
    float angle = slot < MAX_TOTEM_SLOT ? M_PI_F/MAX_TOTEM_SLOT - (slot*2*M_PI_F/MAX_TOTEM_SLOT) : 0;

    CreatureCreatePos pos(m_caster, m_caster->GetOrientation(), 2.0f, angle);

    CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(m_spellInfo->EffectMiscValue[eff_idx]);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature entry %u does not exist but used in spell %u totem summon.", m_spellInfo->Id, m_spellInfo->EffectMiscValue[eff_idx]);
        return;
    }

    Totem* pTotem = new Totem;

    if (!pTotem->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cinfo, m_caster))
    {
        delete pTotem;
        return;
    }

    pTotem->SetSummonPoint(pos);

    if (slot < MAX_TOTEM_SLOT)
        m_caster->_AddTotem(TotemSlot(slot),pTotem);

    //pTotem->SetName("");                                  // generated by client
    pTotem->SetOwner(m_caster);
    pTotem->SetTypeBySummonSpell(m_spellInfo);              // must be after Create call where m_spells initialized

    pTotem->SetDuration(m_duration);

    if (damage)                                             // if not spell info, DB values used
    {
        pTotem->SetMaxHealth(damage);
        pTotem->SetHealth(damage);
    }

    pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        pTotem->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

    if (m_caster->IsPvP())
        pTotem->SetPvP(true);

    // sending SMSG_TOTEM_CREATED before add to map (done in Summon)
    if (slot < MAX_TOTEM_SLOT && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_TOTEM_CREATED, 1 + 8 + 4 + 4);
        data << uint8(slot);
        data << pTotem->GetObjectGuid();
        data << uint32(m_duration);
        data << uint32(m_spellInfo->Id);
        ((Player*)m_caster)->SendDirectMessage(&data);
    }

    pTotem->Summon(m_caster);
}

void Spell::EffectEnchantHeldItem(SpellEffectIndex eff_idx)
{
    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = (Player*)unitTarget;
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if(!item )
        return;

    // must be equipped
    if(!item ->IsEquipped())
        return;

    if (m_spellInfo->EffectMiscValue[eff_idx])
    {
        uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];
        int32 duration = GetSpellDuration(m_spellInfo);     // Try duration index first...
        if(!duration)
            duration = m_currentBasePoints[eff_idx];        //Base points after ..
        if(!duration)
            duration = 10;                                  // 10 seconds for enchants which don't have listed duration

        SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if(!pEnchant)
            return;

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if(item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration*IN_MILLISECONDS, 0);
        item_owner->ApplyEnchantment(item, slot, true);
    }
}

void Spell::EffectDisEnchant(SpellEffectIndex /*eff_idx*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;
    if(!itemTarget || !itemTarget->GetProto()->DisenchantID)
        return;

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    ((Player*)m_caster)->SendLoot(itemTarget->GetObjectGuid(),LOOT_DISENCHANTING);

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)unitTarget;
    uint16 currentDrunk = player->GetDrunkValue();
    uint16 drunkMod = damage * 256;
    if (currentDrunk + drunkMod > 0xFFFF)
        currentDrunk = 0xFFFF;
    else
        currentDrunk += drunkMod;
    player->SetDrunkValue(currentDrunk, m_CastItem ? m_CastItem->GetEntry() : 0);
}

void Spell::EffectFeedPet(SpellEffectIndex eff_idx)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Item* foodItem = itemTarget;
    if(!foodItem)
        return;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;

    if(!pet->isAlive())
        return;

    int32 benefit = pet->GetCurrentFoodBenefitLevel(foodItem->GetProto()->ItemLevel);
    if(benefit <= 0)
        return;

    uint32 count = 1;
    _player->DestroyItemCount(foodItem,count,true);
    // TODO: fix crash when a spell has two effects, both pointed at the same item target

    m_caster->CastCustomSpell(m_caster, m_spellInfo->EffectTriggerSpell[eff_idx], &benefit, NULL, NULL, true);
}

void Spell::EffectDismissPet(SpellEffectIndex /*eff_idx*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Pet* pet = m_caster->GetPet();

    // not let dismiss dead pet
    if(!pet||!pet->isAlive())
        return;

    pet->Unsummon(PET_SAVE_NOT_IN_SLOT, m_caster);
}

void Spell::EffectSummonObject(SpellEffectIndex eff_idx)
{
    uint32 go_id = m_spellInfo->EffectMiscValue[eff_idx];

    uint8 slot = 0;
    switch (m_spellInfo->Effect[eff_idx])
    {
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
        slot = 0;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
        slot = 1;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
        slot = 2;
        break;
    case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
        slot = 3;
        break;
    default:
        return;
    }

    if (ObjectGuid guid = m_caster->m_ObjectSlotGuid[slot])
    {
        if (GameObject* obj = m_caster ? m_caster->GetMap()->GetGameObject(guid) : NULL)
            obj->SetLootState(GO_JUST_DEACTIVATED);
        m_caster->m_ObjectSlotGuid[slot].Clear();
    }

    GameObject* pGameObj = new GameObject;

    float x, y, z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    // Summon in random point all other units if location present
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = m_caster->GetMap();
    if(!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
                         x, y, z, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL,m_caster->getLevel());
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    map->Add(pGameObj);
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << ObjectGuid(pGameObj->GetObjectGuid());
    m_caster->SendMessageToSet(&data, true);

    m_caster->m_ObjectSlotGuid[slot] = pGameObj->GetObjectGuid();

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);
}

void Spell::EffectResurrect(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget)
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(unitTarget->isAlive())
        return;
    if(!unitTarget->IsInWorld())
        return;

    switch (m_spellInfo->Id)
    {
        // Defibrillate (Goblin Jumper Cables) have 33% chance on success
    case 8342:
        if (roll_chance_i(67))
        {
            m_caster->CastSpell(m_caster, 8338, true, m_CastItem);
            return;
        }
        break;
        // Defibrillate (Goblin Jumper Cables XL) have 50% chance on success
    case 22999:
        if (roll_chance_i(50))
        {
            m_caster->CastSpell(m_caster, 23055, true, m_CastItem);
            return;
        }
        break;
    default:
        break;
    }

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = pTarget->GetMaxHealth() * damage / 100;
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * damage / 100;

    pTarget->setResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectAddExtraAttacks(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget || !unitTarget->isAlive())
        return;

    if (m_spellInfo->Id == 20178) //reckoning
    {
        if (unitTarget->m_extraAttacks < 4)
            unitTarget->m_extraAttacks += damage;
    }
    else if (!unitTarget->m_extraAttacks)
        unitTarget->m_extraAttacks = damage;
}

void Spell::EffectParry(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanBlock(true);
}

void Spell::EffectLeapForward(SpellEffectIndex eff_idx)
{
    if(unitTarget->IsTaxiFlying())
        return;

    if( m_spellInfo->rangeIndex == 1)                       //self range
    {
        float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

        float direction = unitTarget->GetOrientation();
        float fx = unitTarget->GetPositionX() + dis * cos(direction);
        float fy = unitTarget->GetPositionY() + dis * sin(direction);
        float fz = unitTarget->GetPositionZ() + (m_caster->IsInWater() ? 0 : 5.0f);

        uint32 mapId = unitTarget->GetMapId();
        if (MMAP::MMapFactory::IsPathfindingEnabled(mapId))
        {
            MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();
            dtPolyRef polyRef;
            float ox,oy,oz;
            unitTarget->GetPosition(ox,oy,oz);

            if (!mmap->GetNearestValidPosition(unitTarget, 1, 1, 5, ox, oy, oz,&polyRef))
                return;

            if (!mmap->DrawRay(unitTarget, polyRef, ox,oy,oz+1.0f, fx, fy, fz)) 
                return;

            if (!m_caster->IsInWater())
            {
                PathInfo path(m_caster, fx, fy, fz);
                PointPath pointPath = path.getFullPath();

                fx = pointPath[pointPath.size()-1].x;
                fy = pointPath[pointPath.size()-1].y;
                fz = pointPath[pointPath.size()-1].z;
            }

            if(!m_caster->GetMap()->GetTerrain()->IsInWater(fx, fy, fz))
            {
                float ground_z = m_caster->GetMap()->GetTerrain()->GetHeight(fx, fy, MAX_HEIGHT);
                float floor_z = m_caster->GetMap()->GetTerrain()->GetHeight(fx, fy, fz);

                if(fabs(fz - floor_z) < fabs(ground_z - fz))
                {
                    fz = floor_z;
                }
                else
                {
                    fz = ground_z;
                }
            }

            unitTarget->NearTeleportTo(fx, fy, fz, unitTarget->GetOrientation(), unitTarget == m_caster);

        } 
        else
        {
            float ox, oy, oz;
            unitTarget->GetPosition(ox, oy, oz);
            float fx2, fy2, fz2;                                // getObjectHitPos overwrite last args in any result case

            if(VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(unitTarget->GetMapId(), ox,oy,oz+0.5f, fx,fy,oz+0.5f,fx2,fy2,fz2, -0.5f))
            {
                fx = fx2;
                fy = fy2;
                fz = fz2;
                unitTarget->UpdateAllowedPositionZ(fx, fy, fz);
            }

            unitTarget->NearTeleportTo(fx, fy, fz, unitTarget->GetOrientation(), unitTarget == m_caster);
        }

    }
}

void Spell::EffectReputation(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;

    int32  rep_change = m_currentBasePoints[eff_idx];
    uint32 faction_id = m_spellInfo->EffectMiscValue[eff_idx];

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if(!factionEntry)
        return;

    rep_change = _player->CalculateReputationGain(REPUTATION_SOURCE_SPELL, rep_change, faction_id);

    _player->GetReputationMgr().ModifyReputation(factionEntry, rep_change);
}

void Spell::EffectQuestComplete(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 quest_id = m_spellInfo->EffectMiscValue[eff_idx];
    ((Player*)unitTarget)->AreaExploredOrEventHappens(quest_id);
}

void Spell::EffectSelfResurrect(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->isAlive())
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if(!unitTarget->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if(damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->EffectMiscValue[eff_idx];
    }
    // percent case
    else
    {
        health = uint32(damage/100.0f*unitTarget->GetMaxHealth());
        if(unitTarget->GetMaxPower(POWER_MANA) > 0)
            mana = uint32(damage/100.0f*unitTarget->GetMaxPower(POWER_MANA));
    }

    Player *plr = ((Player*)unitTarget);
    plr->ResurrectPlayer(0.0f);

    plr->SetHealth( health );
    plr->SetPower(POWER_MANA, mana );
    plr->SetPower(POWER_RAGE, 0 );
    plr->SetPower(POWER_ENERGY, plr->GetMaxPower(POWER_ENERGY) );

    plr->SpawnCorpseBones();
}

void Spell::EffectSkinning(SpellEffectIndex /*eff_idx*/)
{
    if(unitTarget->GetTypeId() != TYPEID_UNIT )
        return;
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = (Creature*) unitTarget;
    int32 targetLevel = creature->getLevel();

    uint32 skill = creature->GetCreatureInfo()->GetRequiredLootSkill();

    ((Player*)m_caster)->SendLoot(creature->GetObjectGuid(),LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel-10)*10 : targetLevel*5;

    int32 skillValue = ((Player*)m_caster)->GetPureSkillValue(skill);

    // Double chances for elites
    ((Player*)m_caster)->UpdateGatherSkill(skill, skillValue, reqValue, creature->IsElite() ? 2 : 1 );
}

void Spell::EffectCharge(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget)
        return;

    //TODO: research more ContactPoint/attack distance.
    //3.666666 instead of ATTACK_DISTANCE(5.0f) in below seem to give more accurate result.
    float x, y, z, distance;
    unitTarget->GetAttackPoint(m_caster, x, y, z);
    distance = unitTarget->GetDistance(m_caster);

    float targetX, targetY, targetZ;
    unitTarget->GetPosition(targetX,targetY,targetZ);

    float ground_z = m_caster->GetMap()->GetTerrain()->GetHeight(targetX, targetY, MAX_HEIGHT);
    float floor_z = m_caster->GetMap()->GetTerrain()->GetHeight(targetX, targetY, targetZ);

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        ((Creature *)unitTarget)->StopMoving();

    uint32 mapId = unitTarget->GetMapId();

    if(m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if(!((Player*)m_caster)->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING))
        {
            if (MMAP::MMapFactory::IsPathfindingEnabled(mapId))
                {

                    MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();
                    dtPolyRef polyRef;

                    if (!mmap->GetNearestValidPosition(unitTarget,3,3,5,targetX,targetY,targetZ,&polyRef))
                        return;

                    if (!mmap->DrawRay(unitTarget,polyRef,targetX,targetY,targetZ,x,y,z))
                    {
                        x = targetX;    
                        y = targetY;
                        z = targetZ;
                        m_caster->MonsterMove(x, y, z, distance*m_caster->GetSpeed(MOVE_RUN)*7);
                    }
                    else
                    {
                        ground_z = m_caster->GetMap()->GetTerrain()->GetHeight(targetX, targetY, MAX_HEIGHT);
                        floor_z = m_caster->GetMap()->GetTerrain()->GetHeight(targetX, targetY, targetZ);

                            if(!unitTarget->IsInWater())
                            {

                                if(ground_z > targetZ || floor_z > targetZ)
                                {
                                    if(fabs(targetZ - floor_z) < fabs(ground_z - targetZ))
                                    {
                                        targetZ = floor_z;
                                    }
                                    else
                                    {
                                        targetZ = ground_z;
                                    }
                                }

                                float chargeTimer = m_caster->GetDistance(unitTarget);
                                m_caster->MonsterMoveByPath(targetX, targetY, targetZ+0.5f, chargeTimer, true, false);
                                ((Player*)m_caster)->SetChargeTarget(unitTarget->GetGUID());
                                ((Player*)m_caster)->SetCharging(true);
                            }
                            else
                            {
                                m_caster->MonsterMove(x, y, z, distance*m_caster->GetSpeed(MOVE_RUN)*7);
                            }
                     
                    }

                }
                else
                {
                               
                            if(ground_z > z || floor_z > z)                      
                            {                        
                                if(fabs(z - floor_z) < fabs(ground_z - z))                                               
                                {                         
                                    z = floor_z;                        
                                }                       
                                else                      
                                {                     
                                    z = ground_z;                     
                                }  
                            }

                            m_caster->MonsterMove(x, y, z, distance*m_caster->GetSpeed(MOVE_RUN)*7);
                        
                }
        }
        else
        {
            m_caster->MonsterMove(x, y, z, distance*m_caster->GetSpeed(MOVE_RUN)*7);
            ((Player*)m_caster)->SetChargeTarget(unitTarget->GetGUID());
            ((Player*)m_caster)->SetCharging(true);
        }
    }
    else
    {
        m_caster->MonsterMoveByPath(x, y, z, 25, false, true);
    }

    //>MonsterMoveByPath(x, y, z, 25, false, true);

    Player* player = dynamic_cast<Player*>(m_caster);
    if (player && player->GetSentinel())
        player->GetSentinel()->SetPlayerPosition(x, y);

    // not all charge effects used in negative spells
    if (unitTarget != m_caster && !IsPositiveSpell(m_spellInfo->Id))
        m_caster->Attack(unitTarget, true);

}

void Spell::EffectSummonCritter(SpellEffectIndex eff_idx)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = (Player*)m_caster;

    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if(!pet_entry)
        return;

    CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonCritter: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    Pet* old_critter = player->GetMiniPet();

    // for same pet just despawn
    if(old_critter && old_critter->GetEntry() == pet_entry)
    {
        player->RemoveMiniPet();
        return;
    }

    // despawn old pet before summon new
    if (old_critter)
        player->RemoveMiniPet();

    CreatureCreatePos pos(m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster->GetOrientation());
    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_caster, m_caster->GetOrientation());

    // summon new pet
    Pet* critter = new Pet(MINI_PET);

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!critter->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        sLog.outError("Spell::EffectSummonCritter, spellid %u: no such creature entry %u", m_spellInfo->Id, pet_entry);
        delete critter;
        return;
    }

    critter->SetSummonPoint(pos);

    //critter->SetName("");                                 // generated by client
    critter->SetOwnerGuid(m_caster->GetObjectGuid());
    critter->SetCreatorGuid(m_caster->GetObjectGuid());
    critter->setFaction(m_caster->getFaction());
    critter->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    critter->AIM_Initialize();
    critter->InitPetCreateSpells();                         // e.g. disgusting oozeling has a create spell as critter...
    critter->SelectLevel(critter->GetCreatureInfo());       // some summoned creaters have different from 1 DB data for level/hp
    critter->SetUInt32Value(UNIT_NPC_FLAGS, critter->GetCreatureInfo()->npcflag);
    // some mini-pets have quests

    // set timer for unsummon
    int32 duration = GetSpellDuration(m_spellInfo);
    if(duration > 0)
        critter->SetDuration(duration);

    player->_SetMiniPet(critter);

    map->Add((Creature*)critter);

    // Notify Summoner
    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(critter);
}

void Spell::EffectKnockBack(SpellEffectIndex eff_idx)
{
    if(!unitTarget)
        return;

    Player *pPlayer = dynamic_cast<Player*>(unitTarget);

    if (pPlayer && pPlayer->GetSentinel())
    {
        // Give the player a 60 second grace period to avoid triggering Sentinel.
        pPlayer->GetSentinel()->SetGraceTime();
    }

    unitTarget->KnockBackFrom(m_caster,float(m_spellInfo->EffectMiscValue[eff_idx])/10,float(damage)/10);
}

void Spell::EffectSendTaxi(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->ActivateTaxiPathTo(m_spellInfo->EffectMiscValue[eff_idx],m_spellInfo->Id);
}

void Spell::EffectPlayerPull(SpellEffectIndex eff_idx)
{
    if(!unitTarget)
        return;

    float dist = unitTarget->GetDistance2d(m_caster);
    if (damage && dist > damage)
        dist = float(damage);

    unitTarget->KnockBackFrom(m_caster,-dist,float(m_spellInfo->EffectMiscValue[eff_idx])/10);
}

void Spell::EffectDispelMechanic(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->EffectMiscValue[eff_idx];

    Unit::SpellAuraHolderMap& Auras = unitTarget->GetSpellAuraHolderMap();
    for(Unit::SpellAuraHolderMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
    {
        next = iter;
        ++next;
        SpellEntry const *spell = iter->second->GetSpellProto();
        if (iter->second->HasMechanic(mechanic))
        {
            unitTarget->RemoveAurasDueToSpell(spell->Id);
            if (Auras.empty())
                break;
            else
                next = Auras.begin();
        }
    }
}

///This happens when Revive Pet is cast
void Spell::EffectSummonDeadPet(SpellEffectIndex /*eff_idx*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *_player = (Player*)m_caster;
    Pet *pet = _player->GetPet();
    if(!pet)
        return;
    if(pet->isAlive())
        return;
    if(damage < 0)
        return;

    pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
    pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->SetDeathState( ALIVE );
    pet->clearUnitState(UNIT_STAT_ALL_STATE);
    pet->SetHealth( uint32(pet->GetMaxHealth()*(float(damage)/100)));

    pet->AIM_Initialize();

    // _player->PetSpellInitialize(); -- action bar not removed at death and not required send at revive
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
}

void Spell::EffectDestroyAllTotems(SpellEffectIndex /*eff_idx*/)
{
    for(int slot = 0;  slot < MAX_TOTEM_SLOT; ++slot)
        if (Totem* totem = m_caster->GetTotem(TotemSlot(slot)))
            totem->UnSummon();
}

void Spell::EffectDurabilityDamage(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[eff_idx];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityPointsLossAll(damage, (slot < -1));
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        ((Player*)unitTarget)->DurabilityPointsLoss(item, damage);
}

void Spell::EffectDurabilityDamagePCT(SpellEffectIndex eff_idx)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[eff_idx];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityLossAll(double(damage)/100.0f, (slot < -1));
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(damage <= 0)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        ((Player*)unitTarget)->DurabilityLoss(item, double(damage)/100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffectIndex /*eff_idx*/)
{
    if(!unitTarget)
        return;

    unitTarget->getThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(SpellEffectIndex eff_idx)
{
    uint32 name_id = m_spellInfo->EffectMiscValue[eff_idx];

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(name_id);

    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast",name_id, m_spellInfo->Id);
        return;
    }

    float fx, fy, fz;

    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        fx = m_targets.m_destX;
        fy = m_targets.m_destY;
        fz = m_targets.m_destZ;
    }
    //FIXME: this can be better check for most objects but still hack
    else if(m_spellInfo->EffectRadiusIndex[eff_idx] && m_spellInfo->speed==0)
    {
        float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));
        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }
    else
    {
        float min_dis = GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float max_dis = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float dis = rand_norm_f() * (max_dis - min_dis) + min_dis;

        // special code for fishing bobber (TARGET_SELF_FISHING), should not try to avoid objects
        // nor try to find ground level, but randomly vary in angle
        if (goinfo->type == GAMEOBJECT_TYPE_FISHINGNODE)
        {
            // calculate angle variation for roughly equal dimensions of target area
            float max_angle = (max_dis - min_dis)/(max_dis + m_caster->GetObjectBoundingRadius());
            float angle_offset = max_angle * (rand_norm_f() - 0.5f);
            m_caster->GetNearPoint2D(fx, fy, dis, m_caster->GetOrientation() + angle_offset);

            GridMapLiquidData liqData;
            if (!m_caster->GetTerrain()->IsInWater(fx, fy, m_caster->GetPositionZ() + 1.f, &liqData))
            {
                SendCastResult(SPELL_FAILED_NOT_FISHABLE);
                SendChannelUpdate(0);
                return;
            }

            fz = liqData.level;
            // finally, check LoS
            if (!m_caster->IsWithinLOS(fx, fy, fz))
            {
                SendCastResult(SPELL_FAILED_LINE_OF_SIGHT);
                SendChannelUpdate(0);
                return;
            }
        }
        else
            m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }

    Map *cMap = m_caster->GetMap();


    // if gameobject is summoning object, it should be spawned a bit in front of the caster's position
    if (goinfo->type == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
    {
        float dX = cosf(m_caster->GetOrientation());
        float dY = sinf(m_caster->GetOrientation());

        m_caster->GetPosition(fx, fy, fz);

        fx += 2.f * dX;
        fy += 2.f * dY;

        float ground_z = m_caster->GetMap()->GetTerrain()->GetHeight(fx, fy, MAX_HEIGHT);
        float floor_z = m_caster->GetMap()->GetTerrain()->GetHeight(fx, fy, fz);

        if(fabs(fz - floor_z) < fabs(ground_z - fz))
        {
            fz = floor_z;
        }
        else
        {
            fz = ground_z;
        }

    }

    GameObject* pGameObj = new GameObject;

    if(!pGameObj->Create(cMap->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), name_id, cMap,
                         fx, fy, fz, m_caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    switch(goinfo->type)
    {
    case GAMEOBJECT_TYPE_FISHINGNODE:
    {
        m_caster->SetChannelObjectGuid(pGameObj->GetObjectGuid());
        m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

        // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
        // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
        int32 lastSec = 0;
        switch(urand(0, 3))
        {
        case 0:
            lastSec =  3;
            break;
        case 1:
            lastSec =  7;
            break;
        case 2:
            lastSec = 13;
            break;
        case 3:
            lastSec = 17;
            break;
        }

        duration = duration - lastSec*IN_MILLISECONDS + FISHING_BOBBER_READY_TIME*IN_MILLISECONDS;
        break;
    }
    case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
    {
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            pGameObj->AddUniqueUse((Player*)m_caster);
            m_caster->AddGameObject(pGameObj);          // will removed at spell cancel
        }
        break;
    }
    case GAMEOBJECT_TYPE_FISHINGHOLE:
    case GAMEOBJECT_TYPE_CHEST:
    default:
        break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);

    pGameObj->SetOwnerGuid(m_caster->GetObjectGuid());

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    DEBUG_LOG("AddObject at SpellEfects.cpp EffectTransmitted");
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->Add(pGameObj);

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);
}

void Spell::EffectSkill(SpellEffectIndex /*eff_idx*/)
{
    DEBUG_LOG("WORLD: SkillEFFECT");
}

void Spell::EffectSummonDemon(SpellEffectIndex eff_idx)
{
    float px = m_targets.m_destX;
    float py = m_targets.m_destY;
    float pz = m_targets.m_destZ;

    Creature* Charmed = m_caster->SummonCreature(m_spellInfo->EffectMiscValue[eff_idx], px, py, pz, m_caster->GetOrientation(),TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,3600000);
    if (!Charmed)
        return;

    // might not always work correctly, maybe the creature that dies from CoD casts the effect on itself and is therefore the caster?
    Charmed->SetLevel(m_caster->getLevel());

    // TODO: Add damage/mana/hp according to level

    if (m_spellInfo->EffectMiscValue[eff_idx] == 89)        // Inferno summon
    {
        // Enslave demon effect, without mana cost and cooldown
        m_caster->CastSpell(Charmed, 20882, true);          // FIXME: enslave does not scale with level, level 62+ minions cannot be enslaved

        // Inferno effect
        Charmed->CastSpell(Charmed, 22703, true, 0);
    }
}

void Spell::EffectSpiritHeal(SpellEffectIndex /*eff_idx*/)
{
    // TODO player can't see the heal-animation - he should respawn some ticks later
    if (!unitTarget || unitTarget->isAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;
    if (m_spellInfo->Id == 22012 && !unitTarget->HasAura(2584))
        return;

    ((Player*)unitTarget)->ResurrectPlayer(1.0f);
    ((Player*)unitTarget)->SpawnCorpseBones();
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffectIndex /*eff_idx*/)
{
    DEBUG_LOG("Effect: SkinPlayerCorpse");
    if ( (m_caster->GetTypeId() != TYPEID_PLAYER) || (unitTarget->GetTypeId() != TYPEID_PLAYER) || (unitTarget->isAlive()) )
        return;

    ((Player*)unitTarget)->RemovedInsignia( (Player*)m_caster );
}
void Spell::EffectBind(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 area_id;
    WorldLocation loc;
    player->GetPosition(loc);
    area_id = player->GetAreaId();

    player->SetHomebindToLocation(loc,area_id);

    // binding
    WorldPacket data( SMSG_BINDPOINTUPDATE, (4+4+4+4+4) );
    data << float(loc.coord_x);
    data << float(loc.coord_y);
    data << float(loc.coord_z);
    data << uint32(loc.mapid);
    data << uint32(area_id);
    player->SendDirectMessage( &data );

    DEBUG_LOG("New Home Position X is %f", loc.coord_x);
    DEBUG_LOG("New Home Position Y is %f", loc.coord_y);
    DEBUG_LOG("New Home Position Z is %f", loc.coord_z);
    DEBUG_LOG("New Home MapId is %u", loc.mapid);
    DEBUG_LOG("New Home AreaId is %u", area_id);

    // zone update
    data.Initialize(SMSG_PLAYERBOUND, 8+4);
    data << player->GetObjectGuid();
    data << uint32(area_id);
    player->SendDirectMessage( &data );
}
