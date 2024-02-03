// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPPThirdPerson/EGameStage.h"
#include "BestOutOfInGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UBestOutOfInGameHUD : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

public:
	virtual void FinishDestroy() override;

	UPROPERTY(Category="Best Out Of", BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<class ABestOutOfGameMode> GameModePtr;

	UFUNCTION()
	void OnGameStageChanged(EGameStage Stage);

	UFUNCTION()
	void OnCharacterHealthChanged(class ACPPThirdPersonCharacter* Character);

	/**
	 * @brief Called to set player's character's health.
	 * @param PlayerIndex Player ID (0 for first player)
	 * @param Percentage Health percentage to be updated.
	 */
	UFUNCTION(Category="Best Out Of", BlueprintImplementableEvent)
	void UpdatePlayerHealth(int PlayerIndex, float Percentage);

	/**
	 * @brief Called to set player's number of rounds won.
	 * @param PlayerIndex Player ID (0 for first player)
	 * @param Wins Number of won rounds.
	 */
	UFUNCTION(Category="Best Out Of", BlueprintImplementableEvent)
	void UpdatePlayerWins(int PlayerIndex, int Wins);
};
