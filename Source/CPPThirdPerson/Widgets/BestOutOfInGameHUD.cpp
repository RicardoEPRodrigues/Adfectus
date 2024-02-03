// Fill out your copyright notice in the Description page of Project Settings.


#include "BestOutOfInGameHUD.h"
#include "../BestOutOfGameMode.h"
#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"

void UBestOutOfInGameHUD::NativeConstruct()
{
	if (ABestOutOfGameMode* GameMode = GameModePtr.Get())
	{
		GameMode->OnGameStageChange.AddDynamic(this, &UBestOutOfInGameHUD::OnGameStageChanged);

		// This iteration assumes that the first player is always the first in the TMap.
		for (const auto Pair : GameMode->CharactersByController)
		{
			Pair.Value->OnHealthChangedEvent.AddDynamic(this, &UBestOutOfInGameHUD::OnCharacterHealthChanged);
		}
	}

	OnCharacterHealthChanged(nullptr);
	OnGameStageChanged(EGameStage::Match_Other);

	Super::NativeConstruct();
}

void UBestOutOfInGameHUD::FinishDestroy()
{
	Super::FinishDestroy();

	if (ABestOutOfGameMode* GameMode = GameModePtr.Get())
	{
		GameMode->OnGameStageChange.RemoveDynamic(this, &UBestOutOfInGameHUD::OnGameStageChanged);

		// This iteration assumes that the first player is always the first in the TMap.
		for (const auto Pair : GameMode->CharactersByController)
		{
			Pair.Value->OnHealthChangedEvent.RemoveDynamic(this, &UBestOutOfInGameHUD::OnCharacterHealthChanged);
		}
	}
}

void UBestOutOfInGameHUD::OnCharacterHealthChanged(ACPPThirdPersonCharacter*)
{
	if (ABestOutOfGameMode* GameMode = GameModePtr.Get())
	{
		// This iteration assumes that the first player is always the first in the TMap.
		for (const auto Pair : GameMode->CharactersByController)
		{
			const int Id = UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(Pair.Key));
			if (Id < 0)
			{
				continue;
			}
			const float Percentage = Pair.Value->Health / Pair.Value->MaxHealth;
			UpdatePlayerHealth(Id, Percentage);
		}
	}
}

void UBestOutOfInGameHUD::OnGameStageChanged(EGameStage Stage)
{
	if (ABestOutOfGameMode* GameMode = GameModePtr.Get())
	{
		// This iteration assumes that the first player is always the first in the TMap.
		for (const auto Pair : GameMode->RoundWinsByPlayer)
		{
			const int Id = UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(Pair.Key));
			if (Id < 0)
			{
				continue;
			}
			UpdatePlayerWins(Id, Pair.Value);
		}
	}
}
