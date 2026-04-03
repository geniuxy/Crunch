#include "CGameplayTags.h"

namespace CGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Dead, "Crunch.Stats.Dead"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Stun, "Crunch.Stats.Stun"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Aim, "Crunch.Stats.Aim"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_DisableAim, "Crunch.Stats.DisableAim"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change, "Crunch.Ability.Combo.Event.Change"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change_Combo01, "Crunch.Ability.Combo.Event.Change.Combo01"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change_Combo02, "Crunch.Ability.Combo.Event.Change.Combo02"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change_Combo03, "Crunch.Ability.Combo.Event.Change.Combo03"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change_Combo04, "Crunch.Ability.Combo.Event.Change.Combo04"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Combo_Event_Change_End, "Crunch.Ability.Combo.Event.Change.End"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_BasicAttack, "Crunch.Ability.BasicAttack"
	);
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Crunch_Ability_BasicAttack_Event_Pressed, "Crunch.Ability.BasicAttack.Event.Pressed",
		"可以用在其他Ability需要普攻Combo时，对技能输入的检测，比如UpperCut时需要输入普攻进行Combo"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_BasicAttack_Event_Damage, "Crunch.Ability.BasicAttack.Event.Damage"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_UpperCut_Event_Damage, "Crunch.Ability.UpperCut.Event.Damage"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_UpperCut_Cooldown, "Crunch.Ability.UpperCut.Cooldown"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Passive_Launched_Event_Activate, "Crunch.Ability.Passive.Launched.Event.Activate"
	);

	/*
	 * GameplayCue Tags
	 */
	UE_DEFINE_GAMEPLAY_TAG(
		GameplayCue_CameraShake, "GameplayCue.CameraShake"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		GameplayCue_Hit_Impact_Crunch_Punch, "GameplayCue.Hit.Impact.Crunch.Punch"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		GameplayCue_Hit_Impact_Minion_Punch, "GameplayCue.Hit.Impact.Minion.Punch"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		GameplayCue_Hit_Reaction, "GameplayCue.Hit.Reaction"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		GameplayCue_Hit_Impact_GroundBlast, "GameplayCue.Hit.Impact.GroundBlast"
	);
}
