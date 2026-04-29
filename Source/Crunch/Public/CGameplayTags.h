#pragma once

#include "NativeGameplayTags.h"

/*
 * Tips:
 * 1. 控制台输入 AbilitySystem.DebugAbilityTags 可以看到视野中的单位所携带的tag
 */
namespace CGameplayTags
{
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Role_Hero);
	
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Dead);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Stun);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Aim);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Crosshair);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_DisableAim);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Health_Full);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Health_Empty);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Mana_Full);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Stats_Mana_Empty);

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change_Combo01);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change_Combo02);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change_Combo03);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change_Combo04);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Combo_Event_Change_End);

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_BasicAttack);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_BasicAttack_Event_Pressed);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_BasicAttack_Event_Released);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_BasicAttack_Event_Damage);

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_UpperCut_Event_Damage);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_UpperCut_Cooldown);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Passive_Launched_Event_Activate);

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_GroundBlast_Cooldown);

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Shoot);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Shoot_Handl);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Shoot_Handr);
	
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_SetByCaller_Experience);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_SetByCaller_Gold);

	/*
	 * Items
	 */

	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crunch_Ability_Items_Shoes_Cooldown);

	/*
	 * GameplayCue Tags
	 */
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_CameraShake);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Hit_Impact_Crunch_Punch);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Hit_Impact_Minion_Punch);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Hit_Reaction);
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Hit_Impact_GroundBlast);
}
