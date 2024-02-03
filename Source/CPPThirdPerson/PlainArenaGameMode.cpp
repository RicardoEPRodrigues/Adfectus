#include "PlainArenaGameMode.h"

#include "AdfectusLogging.h"
#include "CPPThirdPersonCharacter.h"
#include "EngineUtils.h"
#include "3Motion/Personality.h"
#include "3Motion/ThreeMotionAgentComponent.h"
#include "3Motion/BALTASEmotionSystem/TMBaltasModule.h"
#include "3Motion/Modules/TMWinLoseRatioModule.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/GameStateMessagesHUD.h"
#include "Widgets/EndGameWithWinnerHUD.h"
#include "Widgets/FightHUD.h"

APlainArenaGameMode::APlainArenaGameMode() : ABestOutOfGameMode()
{
	MainMenuLevelName = FString(TEXT("PlainArenaMainMenuMap"));
}

void APlainArenaGameMode::SwapCharacters(APlayerController* ControllerA, APlayerController* ControllerB)
{
	if (!ControllerA || !ControllerB) return;

	ACPPThirdPersonCharacter* CharacterA = Cast<ACPPThirdPersonCharacter>(ControllerA->GetCharacter());
	ACPPThirdPersonCharacter* CharacterB = Cast<ACPPThirdPersonCharacter>(ControllerB->GetCharacter());

	if (!CharacterA || !CharacterB) return;

	ControllerA->UnPossess();
	ControllerB->UnPossess();

	ControllerA->Possess(CharacterB);
	ControllerB->Possess(CharacterA);

	CharactersByController.Reset();
	CharactersByController.Add(ControllerA, CharacterB);
	CharactersByController.Add(ControllerB, CharacterA);

	MatchHUD->SwitchPlayerFrames();
	MatchHUD->SetPlayerActors(CharacterB, CharacterA);

	const UThreeMotionAgentComponent* TMAgentComponentA = Cast<UThreeMotionAgentComponent>(
		CharacterA->GetComponentByClass(UThreeMotionAgentComponent::StaticClass()));
	const UThreeMotionAgentComponent* TMAgentComponentB = Cast<UThreeMotionAgentComponent>(
		CharacterB->GetComponentByClass(UThreeMotionAgentComponent::StaticClass()));

	if (!TMAgentComponentA || !TMAgentComponentB) return;

	// {
	// 	UTMWinLoseRatioModule* WinLoseModuleA = nullptr;
	// 	for (const auto Module : TMAgentComponentA->Modules)
	// 	{
	// 		if (UTMWinLoseRatioModule* WinLoseModule = Cast<UTMWinLoseRatioModule>(Module))
	// 		{
	// 			WinLoseModuleA = WinLoseModule;
	// 		}
	// 	}
	// 	UTMWinLoseRatioModule* WinLoseModuleB = nullptr;
	// 	for (const auto Module : TMAgentComponentB->Modules)
	// 	{
	// 		if (UTMWinLoseRatioModule* WinLoseModule = Cast<UTMWinLoseRatioModule>(Module))
	// 		{
	// 			WinLoseModuleB = WinLoseModule;
	// 		}
	// 	}
	// 	if (!WinLoseModuleA || !WinLoseModuleB) return;
	//
	// 	UPersonality* PersonalityA = WinLoseModuleA->Personality;
	// 	UPersonality* PersonalityB = WinLoseModuleB->Personality;
	//
	// 	WinLoseModuleA->Personality = PersonalityB;
	// 	WinLoseModuleB->Personality = PersonalityA;
	// }
	//
	// {
	// 	UTMBaltasModule* WinLoseModuleA = nullptr;
	// 	for (const auto Module : TMAgentComponentA->Modules)
	// 	{
	// 		if (UTMBaltasModule* WinLoseModule = Cast<UTMBaltasModule>(Module))
	// 		{
	// 			WinLoseModuleA = WinLoseModule;
	// 		}
	// 	}
	// 	UTMBaltasModule* WinLoseModuleB = nullptr;
	// 	for (const auto Module : TMAgentComponentB->Modules)
	// 	{
	// 		if (UTMBaltasModule* WinLoseModule = Cast<UTMBaltasModule>(Module))
	// 		{
	// 			WinLoseModuleB = WinLoseModule;
	// 		}
	// 	}
	// 	if (!WinLoseModuleA || !WinLoseModuleB) return;
	//
	// 	UBaltasPersonality* PersonalityA = WinLoseModuleA->Personality;
	// 	UBaltasPersonality* PersonalityB = WinLoseModuleB->Personality;
	//
	// 	WinLoseModuleA->Personality = PersonalityB;
	// 	WinLoseModuleB->Personality = PersonalityA;
	// }
}

void APlainArenaGameMode::OnAnimationEnd(EGameStage Stage)
{
	// NOTE This game mode skips the pre-match call and jumps straight into the match.
	switch (Stage)
	{
	case EGameStage::New_Match:
		{
			EnablePlayersInput(true);

			if (MatchHUD)
			{
				MatchHUD->AddToViewport();
			}

			for (auto Tuple : CharactersByController)
			{
				PlayersReady.Add(Tuple.Key, true);
			}

			if (MessagesHUD)
			{
				MessagesHUD->PlayMatchStartMessage();
			}

			CurrentGameStage = EGameStage::Match_Start;
			OnGameStageChange.Broadcast(CurrentGameStage);
		}
		break;
	case EGameStage::Pre_Match: break;
	case EGameStage::Match_Start: break;
	case EGameStage::Match_End:
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			// UE_LOG(LogAdfectus, Log, TEXT("%s: Respawning."), *GetNameSafe(this))

			// Reset Players
			for (const auto Tuple : CharactersByController)
			{
				Tuple.Value->SoftReset();
				// Super game mode would remove weapons. We don't want that.

				const AActor* PlayerStart;
				// Choose Player Start location depending on player index.
				if (Tuple.Key == UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PlayerStart = K2_FindPlayerStart(Tuple.Key, TEXT("Player1StartTag"));
				}
				else
				{
					PlayerStart = K2_FindPlayerStart(Tuple.Key, TEXT("Player2StartTag"));
				}

				if (PlayerStart)
				{
					// RestartPlayerAtPlayerStart(Tuple.Key, PlayerStart);
					Tuple.Value->SetActorLocation(PlayerStart->GetActorLocation());
					Tuple.Value->SetActorRotation(PlayerStart->GetActorRotation());

					if (Tuple.Key != nullptr)
					{
						Tuple.Key->SetControlRotation(PlayerStart->GetActorRotation());
					}
				}
			}

			// Swap characters for every game, so that a player has the change to try both characters
			SwapCharacters(GameState->PlayerArray[0]->GetPlayerController(),
			               GameState->PlayerArray[1]->GetPlayerController());

			// ResetLevel();
			// Reset all actors (except controllers, the GameMode, and any other actors specified by ShouldReset())
			for (FActorIterator It(GetWorld()); It; ++It)
			{
				AActor* A = *It;
				bool IsPlayer = false;
				for (const auto Tuple : CharactersByController)
				{
					if (A == Tuple.Value)
					{
						IsPlayer = true;
					}
				}
				if (IsValid(A) && A != this && !A->IsA<AController>() && ShouldReset(A) && !IsPlayer)
				{
					A->Reset();
				}
			}

			CurrentGameStage = EGameStage::New_Match;
			OnGameStageChange.Broadcast(CurrentGameStage);
			EnablePlayersInput(false);
			// Wait a bit and announce the new match.
			FTimerDelegate NewMatchTimerDelegate;
			NewMatchTimerDelegate.BindLambda([&]
			{
				if (MessagesHUD)
				{
					MessagesHUD->PlayNewRoundMessage(NumOfRoundsPlayed);
				}
			});
			FTimerHandle NewMatchTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(NewMatchTimerHandle, NewMatchTimerDelegate, 1.f, false);
		}
		break;
	case EGameStage::Match_Other:
	default:
		break;
	}
}
