// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameCharacterHUD.h"

#include "AdfectusLogging.h"

ACPPThirdPersonCharacter* UInGameCharacterHUD::SetCharacter(APawn* pawn)
{
	if (pawn == nullptr)
	{
		return nullptr;
	}

	Character = Cast<ACPPThirdPersonCharacter>(pawn);

	if (Character == nullptr)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Unable to cast Pawn to Character. Character is set to NULL."));
	}

	return Character;
}

FString UInGameCharacterHUD::GetCharacterName()
{
	if (Character == nullptr)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Character is NULL for HUD."));
		return "Character NULL";
	}

	return Character->CharacterName;
}

float UInGameCharacterHUD::GetHealthPercentage()
{
	if (Character == nullptr)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Character is NULL for HUD."));
		return 0;
	}

	if (Character->MaxHealth <= 0)
	{
		return 0;
	}

	return Character->Health / Character->MaxHealth;
}

bool UInGameCharacterHUD::HasInteractables()
{
	if (Character == nullptr)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Character is NULL for HUD."));
		return false;
	}

	return Character->HasInteractables();
}

ACPPThirdPersonCharacter* UInGameCharacterHUD::GetLastHitEnemy()
{
	if (Character == nullptr)
	{
		return nullptr;
	}

	ACPPThirdPersonCharacter* enemy = Character->GetLastHitEnemy();

	return enemy;
}

FString UInGameCharacterHUD::GetLastHitEnemyName()
{
	ACPPThirdPersonCharacter* enemy = GetLastHitEnemy();
	if (enemy == nullptr)
	{
		return "Enemy NULL";
	}

	return enemy->CharacterName;
}

float UInGameCharacterHUD::GetLastHitEnemyHealthPercentage()
{
	ACPPThirdPersonCharacter* enemy = GetLastHitEnemy();
	if (enemy == nullptr)
	{
		return 0;
	}

	if (enemy->MaxHealth <= 0)
	{
		return 0;
	}

	return enemy->Health / enemy->MaxHealth;
}
