// Fill out your copyright notice in the Description page of Project Settings.

#include "AdfectusGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LatentActionManager.h"
#include "Engine/World.h"
#include "Engine.h"
#include "LatentActions.h"
// #include "AdfectusPlayerController.h"
#include "CPPThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

UAdfectusGameInstance::UAdfectusGameInstance() : IsLoadingAsync(true), IsLocalMultiplayer(true)
{
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
	// 	TEXT("/Game/Adfectus/Blueprints/Characters/AdfectusCharacter_Player"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	Pawn = nullptr;
}

void UAdfectusGameInstance::TransitionLevel(FName previousLevelName, FName levelName, FLatentActionInfo LatentInfo)
{
	transitionLevel = levelName;
	CurrentLevel = levelName;
	transitionLatentInfo = LatentInfo;

	if (!previousLevelName.IsNone())
	{
		FLatentActionInfo UnloadLatentInfo;
		UnloadLatentInfo.CallbackTarget = this;
		UnloadLatentInfo.ExecutionFunction = "OnTransitionLevelUnloadFinished";
		UnloadLatentInfo.Linkage = 1;
		UnloadLatentInfo.UUID = 1;
		UGameplayStatics::UnloadStreamLevel(this, previousLevelName, UnloadLatentInfo, false);
	}
	else
	{
		OnTransitionLevelUnloadFinished();
	}
}

void UAdfectusGameInstance::OnTransitionLevelUnloadFinished()
{
	UGameplayStatics::LoadStreamLevel(this, transitionLevel, true, false, transitionLatentInfo);
}

void UAdfectusGameInstance::LoadLevelHidden(FName levelName, FLatentActionInfo LatentInfo)
{
	UGameplayStatics::LoadStreamLevel(this, levelName, false, false, LatentInfo);
}

void UAdfectusGameInstance::SetLevelVisibility(FName levelName, bool visibility)
{
	ULevelStreaming* levelStreaming = UGameplayStatics::GetStreamingLevel(this, levelName);
	levelStreaming->SetShouldBeVisible(visibility);
}

void UAdfectusGameInstance::ReviveAllPlayers()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController)
		{
			continue;
		}
		// AAdfectusPlayerController* controller = Cast<AAdfectusPlayerController>(PlayerController);

		ACPPThirdPersonCharacter* character = Cast<ACPPThirdPersonCharacter>(PlayerController->GetPawn());
		if (!character)
		{
			continue;
		}

		character->Revive();
	}
}

void UAdfectusGameInstance::RemoveSplitscreenPlayers()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		// Remove only local split screen players
		APlayerController* Controller = Iterator->Get();
		if (Controller && !Controller->IsPrimaryPlayer())
		{
			UGameplayStatics::RemovePlayer(Controller, true);
		}
	}
}
