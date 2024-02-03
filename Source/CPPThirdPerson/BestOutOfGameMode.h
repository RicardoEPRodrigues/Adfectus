// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameStage.h"
#include "GameFramework/GameModeBase.h"
#include "BestOutOfGameMode.generated.h"

/**
 * @brief This game mode implements a Best Out of X mechanic (e.g. Best out of 3 rounds).
 * Essentially it will check if the characters die, and if so it counts points for the other player.
 * After the game is finished, it will reset the level or return to the main menu.
 */
UCLASS()
class CPPTHIRDPERSON_API ABestOutOfGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	/**
	 * Called after BeginPlay to avoid issues with uninitialized variables.
	 */
	UFUNCTION()
	void DelayedBeginPlay();

	/**
	 * Timer to call DelayedBeginPlay.
	 */
	FTimerHandle DelayedBeginPlayTimer;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	APlayerController* SecondPlayerController;

	/**
	 * @brief Time to wait until respawning the players.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	float RespawnDelayInSeconds = 2;

	/**
	 * @brief After a player is defeated, time can be dilated for dramatic effect. That dilation can be defined here.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0.1", UIMin = "0.1"))
	float TimeDilationAfterDefeat = .5f;

	/**
	 * @brief HUD Class to be created to display game stages during gameplay.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameStateMessagesHUD> MessagesHUDClass;

	UPROPERTY()
	class UGameStateMessagesHUD* MessagesHUD;

	/**
	 * @brief HUD Class to be created to display health and others during combat.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UFightHUD> MatchHUDClass;

	UPROPERTY()
	class UFightHUD* MatchHUD;

	/**
	 * @brief HUD Class to be created to display health and others during combat.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UEndGameWithWinnerHUD> EndGameHUDClass;

	UPROPERTY()
	class UEndGameWithWinnerHUD* EndGameHUD;

public:
	ABestOutOfGameMode();

	virtual void StartPlay() override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	/**
	 * Starts the match. This is called after the players are ready.
	 */
	UFUNCTION(BlueprintCallable)
	void StartMatch();

	// TODO Think of other way to get the Main Menu level name.
	UPROPERTY(Category="Gameplay", EditAnywhere, BlueprintReadWrite)
	FString MainMenuLevelName = FString(TEXT("MainMenuMap"));

	/**
	 * @brief Hold the value of the current stage the match is on.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	EGameStage CurrentGameStage = EGameStage::Match_Start;

	/**
	 * @brief Maximum number of rounds to be played until a victor is decided.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	int MaxNumOfRounds = 5;

	/**
	 * @brief Number of rounds played until now. Its value starts at 1.
	 */
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	int NumOfRoundsPlayed = 1;

	// TODO Fix data storage. Instead of having multiple arrays of data, implement a custom Player State and a custom struct to hold all needed info.

	/**
	 * @brief Mapping of which characters belong to each controller. Characters cannot be null.
	 */
	UPROPERTY(Category="Best Out Of", VisibleAnywhere, BlueprintReadOnly)
	TMap<class AController*, class ACPPThirdPersonCharacter*> CharactersByController;

	/**
	 * @brief Number of Wins in each round organized by player.
	*/
	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	TMap<class AController*, int> RoundWinsByPlayer;

	UPROPERTY(Category="Best Out Of", EditAnywhere, BlueprintReadWrite)
	TMap<class AController*, bool> PlayersReady;

	class ACPPThirdPersonCharacter* UpdateCharacterFromController(class AController* Controller);

	UFUNCTION()
	virtual void OnCharacterDeath(class ACPPThirdPersonCharacter* Character);

	UFUNCTION()
	virtual void OnAnimationEnd(EGameStage Stage);

	UFUNCTION(Category="Best Out Of", BlueprintCallable)
	void ActorReadyForMatch(class AActor* Actor);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameStageChangeDelegate, EGameStage, NewStage);

	/**
	 * @brief Called when the stage of the game changes.
	 */
	UPROPERTY(Category="Best Out Of", BlueprintCallable, BlueprintAssignable)
	FGameStageChangeDelegate OnGameStageChange;

	virtual void EnablePlayersInput(bool Enable);

	UFUNCTION(Category="Best Out Of", BlueprintCallable)
	virtual void OnEndGameRematchOptionPressed();

	UFUNCTION(Category="Best Out Of", BlueprintCallable)
	virtual void OnEndGameQuitOptionPressed();
};
