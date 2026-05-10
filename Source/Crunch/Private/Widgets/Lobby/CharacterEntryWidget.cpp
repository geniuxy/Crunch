// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/CharacterEntryWidget.h"

#include "Characters/Data/PA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefinition = Cast<UPA_CharacterDefinition>(ListItemObject);
	if (CharacterDefinition)
	{
		CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(
			IconTextureMatParamName, CharacterDefinition->LoadIcon()
		);
		CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterDisplayName()));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, bIsSelected ? 0.f : 1.f);
}
