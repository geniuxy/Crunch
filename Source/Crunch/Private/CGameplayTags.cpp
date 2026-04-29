#include "CGameplayTags.h"

namespace CGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Role_Hero, "Crunch.Role.Hero"
	);

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
		Crunch_Stats_Health_Full, "Crunch.Stats.Health.Full"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Health_Empty, "Crunch.Stats.Health.Empty"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Mana_Full, "Crunch.Stats.Mana.Full"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Stats_Mana_Empty, "Crunch.Stats.Mana.Empty"
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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Crunch_Ability_BasicAttack_Event_Released, "Crunch.Ability.BasicAttack.Event.Released",
		"检测普攻键的松开事件，可以用于比如标志Shooting动作的结束"
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

	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_GroundBlast_Cooldown, "Crunch.Ability.GroundBlast.Cooldown"
	);

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Crunch_Ability_Shoot, "Crunch.Ability.Shoot", "用于在Shoot Ability中，触发子弹的生成"
	);
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Crunch_Ability_Shoot_Handl, "Crunch.Ability.Shoot.Handl", "用于在Shoot Ability中，表示子弹是从左手生成"
	);
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(
		Crunch_Ability_Shoot_Handr, "Crunch.Ability.Shoot.Handr", "用于在Shoot Ability中，表示子弹是从右手生成"
	);
	
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_SetByCaller_Experience, "Crunch.SetByCaller.Experience"
	);
	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_SetByCaller_Gold, "Crunch.SetByCaller.Gold"
	);

	/*
	 * Items
	 */

	UE_DEFINE_GAMEPLAY_TAG(
		Crunch_Ability_Items_Shoes_Cooldown, "Crunch.Ability.Items.Shoes.Cooldown"
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
