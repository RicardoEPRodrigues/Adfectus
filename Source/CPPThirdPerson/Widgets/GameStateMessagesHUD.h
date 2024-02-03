// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPPThirdPerson/EGameStage.h"
#include "GameStateMessagesHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UGameStateMessagesHUD : public UUserWidget
{
	GENERATED_BODY()
	
	public:

	/**
	 * @brief To be called when the player starts a new round.
	 */
	UFUNCTION(Category="Gameplay", BlueprintNativeEvent)
	void PlayNewRoundMessage(int RoundNumber);
	
	/**
	 * @brief To be called when the player is preparing for a new round.
	 * Played after the @ref PlayNewRoundMessage
	 */
	UFUNCTION(Category="Gameplay", BlueprintNativeEvent)
	void PlayPreMatchMessage();

	/**
	 * @brief To be called when the player is about to start a new combat round.
	 * Played after the @ref PlayPreMatchMessage
	 */
	UFUNCTION(Category="Gameplay", BlueprintNativeEvent)
	void PlayMatchStartMessage();
	
	/**
	 * @brief To be called when a round ends with a winner.
	 * Played after the @ref PlayMatchStartMessage
	 */
	UFUNCTION(Category="Gameplay", BlueprintNativeEvent)
	void PlayEndMatchMessage(const FString & WinnerName);
	
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAnimationEndDelegate, EGameStage, NewStage);
	
	UPROPERTY(BlueprintCallable, Category="Gameplay")
	FAnimationEndDelegate OnAnimationEndEvent;
};
