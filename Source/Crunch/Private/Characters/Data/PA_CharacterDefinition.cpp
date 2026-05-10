// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Data/PA_CharacterDefinition.h"

#include "Characters/CCharacter.h"

FPrimaryAssetId UPA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType("CharacterDefinition");
}

UTexture2D* UPA_CharacterDefinition::LoadIcon() const
{
	return CharacterIcon.LoadSynchronous();
}

TSubclassOf<ACCharacter> UPA_CharacterDefinition::LoadCharacterClass() const
{
	return CharacterClass.LoadSynchronous();
}

TSubclassOf<UAnimInstance> UPA_CharacterDefinition::LoadDisplayAnimationBP() const
{
	return DisplayAnimBP.LoadSynchronous();
}

USkeletalMesh* UPA_CharacterDefinition::LoadDisplayMesh() const
{
	TSubclassOf<ACCharacter> LoadedCharacterClass = LoadCharacterClass();
	if (!LoadedCharacterClass) return nullptr;
	ACharacter* Character = Cast<ACharacter>(LoadedCharacterClass.GetDefaultObject());
	if (!Character) return nullptr;

	return Character->GetMesh()->GetSkeletalMeshAsset();
}
