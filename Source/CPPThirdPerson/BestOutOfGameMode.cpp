// Fill out your copyright notice in the Description page of Project Settings.


#include "BestOutOfGameMode.h"

#include "AdfectusGameInstance.h"
#include "AdfectusLogging.h"
#include "CPPThirdPersonCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/GameStateMessagesHUD.h"
#include "Widgets/EndGameWithWinnerHUD.h"
#include "Widgets/BestOutOfInGameHUD.h"
#include "Widgets/FightHUD.h"


ABestOutOfGameMode::ABestOutOfGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
	// 	TEXT("/Game/Adfectus/Blueprints/Characters/AdfectusCharacter_Player"));
	// if (PlayerPawnBPClass.Class != nullptr)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(
		TEXT("/Game/Adfectus/Blueprints/Characters/Adfectus_Controller_Player"));
	if (PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UGameStateMessagesHUD> MessagesHUDBPClass(
		TEXT(
			"WidgetBlueprint'/Game/Adfectus/Blueprints/HUD/BP_GameStateMessagesHUD_Master.BP_GameStateMessagesHUD_Master_C'"));
	if (MessagesHUDBPClass.Class != nullptr)
	{
		MessagesHUDClass = MessagesHUDBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UFightHUD> MatchHUDBPClass(
		TEXT(
			"WidgetBlueprint'/Game/Adfectus/Blueprints/HUD/Fight_HUD.Fight_HUD_C'"));
	if (MatchHUDBPClass.Class != nullptr)
	{
		MatchHUDClass = MatchHUDBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UEndGameWithWinnerHUD> EndGameHUDBPClass(
		TEXT(
			"WidgetBlueprint'/Game/Adfectus/Blueprints/HUD/BP_EndGame_HUD.BP_EndGame_HUD_C'"));
	if (EndGameHUDBPClass.Class != nullptr)
	{
		EndGameHUDClass = EndGameHUDBPClass.Class;
	}

	// PrimaryActorTick.bStartWithTickEnabled = true;
	// PrimaryActorTick.bCanEverTick = true;
}

void ABestOutOfGameMode::StartPlay()
{
	Super::StartPlay();

	if (UAdfectusGameInstance* GameInstance = Cast<UAdfectusGameInstance>(this->GetGameInstance()))
	{
		GameInstance->MainMenuLevelName = MainMenuLevelName;
		GameInstance->GameLevelName = GetWorld()->GetName();
	}
}

void ABestOutOfGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) 
{
	if (NewPlayer->GetPawn())
	{
		NewPlayer->GetPawn()->Destroy();
		NewPlayer->UnPossess();
	}
} 

void ABestOutOfGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (const ACPPThirdPersonCharacter* Character = UpdateCharacterFromController(NewPlayer))
	{
		// UE_LOG(LogAdfectus, Log, TEXT("%s: Respawning character %s."), *GetNameSafe(this),
		//        *Character->CharacterName)
	}
}

ACPPThirdPersonCharacter* ABestOutOfGameMode::UpdateCharacterFromController(AController* Controller)
{
	ACPPThirdPersonCharacter* Character = Controller->GetPawn<ACPPThirdPersonCharacter>();

	if (!Character)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("%s: Pawn is null or not a ACPPThirdPersonCharacter. Not updating"),
		       *GetNameSafe(this))
		return nullptr;
	}

	// Save character to be checked on later.
	CharactersByController.Add(Controller, Character);

	return Character;
}

void ABestOutOfGameMode::Logout(AController* Exiting)
{
	if (CharactersByController.Contains(Exiting) && CharactersByController[Exiting]->IsValidLowLevel())
	{
		CharactersByController[Exiting]->OnDeathEvent.RemoveDynamic(this, &ABestOutOfGameMode::OnCharacterDeath);
		CharactersByController[Exiting]->OnHealthChangedEvent.RemoveDynamic(
			MatchHUD, &UFightHUD::OnCharacterHealthChanged);
	}

	Super::Logout(Exiting);
}

void ABestOutOfGameMode::BeginPlay()
{
	Super::BeginPlay();

	MessagesHUD = CreateWidget<UGameStateMessagesHUD>(this->GetGameInstance(), MessagesHUDClass);
	if (MessagesHUD)
	{
		MessagesHUD->AddToViewport();
		MessagesHUD->OnAnimationEndEvent.AddDynamic(this, &ABestOutOfGameMode::OnAnimationEnd);
	}

	MatchHUD = CreateWidget<UFightHUD>(this->GetGameInstance(), MatchHUDClass);

	EndGameHUD = CreateWidget<UEndGameWithWinnerHUD>(this->GetGameInstance(), EndGameHUDClass);
	if (EndGameHUD)
	{
		EndGameHUD->OnRematchButtonEvent.AddDynamic(this, &ABestOutOfGameMode::OnEndGameRematchOptionPressed);
		EndGameHUD->OnQuitButtonEvent.AddDynamic(this, &ABestOutOfGameMode::OnEndGameQuitOptionPressed);
	}
	
	/*
	 * HACK There is a bug in UE5.1 that does not correctly bind the gamepad to the player controller.
	 * We have to create a controller. Then wait. Then delete the controller and create a new one.
	 */
	SecondPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 1);
	if (!SecondPlayerController)
	{
		SecondPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), 1);
	}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlayTimer, this, &ABestOutOfGameMode::DelayedBeginPlay,
											   0.1f);
	}
}

void ABestOutOfGameMode::DelayedBeginPlay()
{
	SecondPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 1);
	/*
	 * HACK See explanation above.
	 */
	if (SecondPlayerController)
	{
		UGameplayStatics::RemovePlayer(SecondPlayerController, false);
		SecondPlayerController = nullptr;
	}
	if (!SecondPlayerController)
	{
		SecondPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), 1);
	}
}

void ABestOutOfGameMode::StartMatch()
{
	const int NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	for (int i = 0 ; i < NumPlayers ; i++)
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i))
		{
			RoundWinsByPlayer.Add(PlayerController, 0);
			PlayersReady.Add(PlayerController, false);

			if (const ACPPThirdPersonCharacter* Character = UpdateCharacterFromController(PlayerController))
			{
				// UE_LOG(LogAdfectus, Log, TEXT("%s: New player joined as %s."), *GetNameSafe(this),
				// 	   *Character->CharacterName)
			}
		}
	}
	

	if (MatchHUD)
	{
		MatchHUD->SetRounds((MaxNumOfRounds / 2) + 1);
		const auto Player1 = Cast<AActor>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
		const auto Player2 = Cast<AActor>(SecondPlayerController->GetPawn());
		MatchHUD->SetPlayerActors(Player1, Player2);
	}

	CurrentGameStage = EGameStage::New_Match;
	OnGameStageChange.Broadcast(CurrentGameStage);
	EnablePlayersInput(false);
	// Wait a bit an announce the new match.
	FTimerDelegate NewMatchTimerDelegate;
	NewMatchTimerDelegate.BindLambda([&]
	{
		if (MessagesHUD)
		{
			MessagesHUD->PlayNewRoundMessage(1);
		}
	});
	FTimerHandle NewMatchTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(NewMatchTimerHandle, NewMatchTimerDelegate, 0.5f, false);

	for (auto Player : GameState->PlayerArray)
	{
		if (ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Player.Get()->GetPawn()))
		{
			Character->OnHealthChangedEvent.AddDynamic(MatchHUD, &UFightHUD::OnCharacterHealthChanged);
			Character->OnDeathEvent.AddDynamic(this, &ABestOutOfGameMode::OnCharacterDeath);
		}
	}
}

void ABestOutOfGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{	
	Super::EndPlay(EndPlayReason);

	if (MessagesHUD)
	{
		MessagesHUD->RemoveFromParent();
		MessagesHUD->OnAnimationEndEvent.RemoveDynamic(this, &ABestOutOfGameMode::OnAnimationEnd);
	}

	if (EndGameHUD)
	{
		EndGameHUD->RemoveFromParent();
		EndGameHUD->OnRematchButtonEvent.RemoveDynamic(this, &ABestOutOfGameMode::OnEndGameRematchOptionPressed);
		EndGameHUD->OnQuitButtonEvent.RemoveDynamic(this, &ABestOutOfGameMode::OnEndGameQuitOptionPressed);
	}

	if (EndPlayReason != EEndPlayReason::Quit && EndPlayReason != EEndPlayReason::EndPlayInEditor )
	{
		UGameplayStatics::RemovePlayer(SecondPlayerController, true);
		SecondPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 1);
		if (SecondPlayerController)
		{
			UGameplayStatics::RemovePlayer(SecondPlayerController, true);
			SecondPlayerController = nullptr;
		}
	}
}

void ABestOutOfGameMode::OnCharacterDeath(ACPPThirdPersonCharacter* Character)
{
	TTuple<AController*, ACPPThirdPersonCharacter*> Winner;
	TTuple<AController*, ACPPThirdPersonCharacter*> Loser;

	// Find the winner.
	for (const auto CharacterTuple : CharactersByController)
	{
		const ACPPThirdPersonCharacter* OtherCharacter = CharacterTuple.Value;
		if (!OtherCharacter)
		{
			UE_LOG(LogAdfectus, Error, TEXT("%s: Character is not valid!"), *GetNameSafe(this))
			continue;
		}

		if (OtherCharacter == Character)
		{
			Loser = CharacterTuple;
		}
		else
		{
			Winner = CharacterTuple;
		}

		PlayersReady.Add(CharacterTuple.Key, false);
	}

	if (!Winner.Key)
	{
		UE_LOG(LogAdfectus, Error, TEXT("%s: Winner is null, only one player?"), *GetNameSafe(this))
		return;
	}

	++NumOfRoundsPlayed;

	const int Wins = ++RoundWinsByPlayer[Winner.Key];

	int Player1Wins, Player2Wins;
	FString WinnerName;

	// One can use the name of the character for the win screen.
	// WinnerName = Winner.Value->CharacterName;

	if (Winner.Key == GetWorld()->GetFirstPlayerController())
	{
		WinnerName = "Player 1";
		Player1Wins = Wins;
		Player2Wins = RoundWinsByPlayer[Loser.Key];
	}
	else
	{
		WinnerName = "Player 2";
		Player2Wins = Wins;
		Player1Wins = RoundWinsByPlayer[Loser.Key];
	}

	MatchHUD->UpdatePlayerWins(Player1Wins, Player2Wins);

	UE_LOG(LogAdfectus, Log, TEXT("%s: Current Score >> %s %d | %d %s."), *GetNameSafe(this),
	       *Winner.Value->CharacterName, Wins, RoundWinsByPlayer[Loser.Key], *Loser.Value->CharacterName)

	if (Wins > (MaxNumOfRounds / 2))
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&,WinnerName]
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			if (MatchHUD)
			{
				MatchHUD->RemoveFromParent();
			}
			if (EndGameHUD)
			{
				EndGameHUD->SetWinnerName(WinnerName);
				EndGameHUD->AddToViewport();
				UGameplayStatics::SetGamePaused(GetWorld(), true);
				GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		});
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, RespawnDelayInSeconds, false);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationAfterDefeat);
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationAfterDefeat);
		FTimerDelegate EndMatchTimerDelegate;
		EndMatchTimerDelegate.BindLambda([&,WinnerName]
		{
			if (MatchHUD)
			{
				MatchHUD->RemoveFromParent();
			}
			if (MessagesHUD)
			{
				// Note that at the end of the EndMatch Animation the OnAnimationEnd is called with a EGameStage::Match_End
				MessagesHUD->PlayEndMatchMessage(WinnerName);
			}
		});
		FTimerHandle EndMatchTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(EndMatchTimerHandle, EndMatchTimerDelegate, .5f, false);

		// CurrentGameStage = EGameStage::Match_End;
		// OnGameStageChange.Broadcast(CurrentGameStage);
	}

	CurrentGameStage = EGameStage::Match_End;
	OnGameStageChange.Broadcast(CurrentGameStage);
}

void ABestOutOfGameMode::OnAnimationEnd(EGameStage Stage)
{
	switch (Stage)
	{
	case EGameStage::New_Match:
		{
			EnablePlayersInput(true);
			CurrentGameStage = EGameStage::Pre_Match;
			OnGameStageChange.Broadcast(CurrentGameStage);
			// Wait a bit and announce the pre match message.
			FTimerDelegate PreMatchTimerDelegate;
			PreMatchTimerDelegate.BindLambda([&]
			{
				if (MessagesHUD)
				{
					MessagesHUD->PlayPreMatchMessage();
				}
			});
			FTimerHandle PreMatchTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(PreMatchTimerHandle, PreMatchTimerDelegate, .5f, false);
		}
		break;
	case EGameStage::Pre_Match:
		{
			if (MatchHUD)
			{
				MatchHUD->AddToViewport();
			}
		}
		break;
	case EGameStage::Match_Start: break;
	case EGameStage::Match_End:
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			UE_LOG(LogAdfectus, Log, TEXT("%s: Respawning."), *GetNameSafe(this))

			// Reset Players
			for (const auto Tuple : CharactersByController)
			{
				Tuple.Value->SoftReset();
				Tuple.Value->DropWeapon(true);
				Tuple.Value->DropShield(true);

				const AActor* PlayerStart = nullptr;
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
					if (Tuple.Value->Controller != nullptr)
					{
						Tuple.Value->Controller->SetControlRotation(PlayerStart->GetActorRotation());
					}
				}
			}

			// ResetLevel();
			// Reset all actors (except controllers, the GameMode, and any other actors specified by ShouldReset())
			for (FActorIterator It(GetWorld()); It; ++It)
			{
				AActor* A = *It;
				bool IsPlayer = false;
				for (auto Tuple : CharactersByController)
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

void ABestOutOfGameMode::ActorReadyForMatch(AActor* Actor)
{
	for (auto Tuple : CharactersByController)
	{
		if (Tuple.Value == Actor)
		{
			PlayersReady.Add(Tuple.Key, true);
		}
	}

	bool AllReady = true;
	for (const auto Tuple : PlayersReady)
	{
		AllReady = AllReady && Tuple.Value;
	}
	if (AllReady)
	{
		if (MessagesHUD)
		{
			MessagesHUD->PlayMatchStartMessage();
		}
		CurrentGameStage = EGameStage::Match_Start;
		OnGameStageChange.Broadcast(CurrentGameStage);
	}
}

void ABestOutOfGameMode::EnablePlayersInput(bool Enable)
{
	for (auto Tuple : CharactersByController)
	{
		APlayerController* Controller = Cast<APlayerController>(Tuple.Key);
		if (Controller)
		{
			if (Enable)
			{
				Tuple.Value->EnableInput(Controller);
			}
			else
			{
				Tuple.Value->DisableInput(Controller);
			}
		}
	}
}

void ABestOutOfGameMode::OnEndGameRematchOptionPressed()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(GetWorld()->GetName()));
}

void ABestOutOfGameMode::OnEndGameQuitOptionPressed()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(MainMenuLevelName));
}
