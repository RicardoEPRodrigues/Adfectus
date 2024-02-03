// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FightHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UFightHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void OnCharacterHealthChanged(class ACPPThirdPersonCharacter* Character);

	UFUNCTION()
	void OnPlayerWinRound(const uint8 Player1Wins, const uint8 Player2Wins);

	/**
	 * @brief Called to set player's character's health.
	 * @param PlayerIndex Player ID (0 for first player)
	 * @param Percentage Health percentage to be updated.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerHealth(int PlayerIndex, float Percentage);

	/**
	 * Called to set the number of rounds the game will be played.
	 * @param Rounds Number of round the game will be played.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetRounds(int Rounds);


	/**
	 * @brief Called to update the players wins
	 * @param Player1Wins Number of rounds won by player 1
	 * @param Player2Wins Number of rounds won by player 2
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerWins(const uint8 Player1Wins, const uint8 Player2Wins);

	/**
	 * Called to set the player actors.
	 * @param Player1 Player 1 Actor
	 * @param Player2 Player 2 Actor
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetPlayerActors(AActor* Player1, AActor* Player2);

	/**
	 * Switches the player frames to the other player.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchPlayerFrames();
};
