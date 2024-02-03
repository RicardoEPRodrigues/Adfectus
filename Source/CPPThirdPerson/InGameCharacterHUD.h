// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPPThirdPersonCharacter.h"
#include "InGameCharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UInGameCharacterHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	ACPPThirdPersonCharacter* Character;

	/**
	 * Helper function to initialize Character by casting a Pawn into a Character.
	 */
	UFUNCTION(BlueprintCallable, Category = Character)
	virtual ACPPThirdPersonCharacter* SetCharacter(APawn* pawn);
	/**
	 * Return the name of the character.
	 * 
     * @return Name of the character.
	 */
	UFUNCTION(BlueprintPure, Category = Character)
	virtual FString GetCharacterName();

	/**
	 * Return the percentage of health of the character.
	 * 
	 * @note It is a simple division between current health and max health.
	 * @return Percentage of health the character has from from 0 to 1.
	 */
	UFUNCTION(BlueprintPure, Category = Character)
	virtual float GetHealthPercentage();

	/**
	 * Check whether or not the character has interactables nearby.
	 * 
     * @return True if there are Interactables nearby, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = Character)
	virtual bool HasInteractables();

	/**
	 * Return a reference to the last enemy character to be hit by this character.
	 * 
     * @return Reference to the last enemy character to be hit.
	 */
	UFUNCTION(BlueprintPure, Category = Enemy)
	virtual ACPPThirdPersonCharacter* GetLastHitEnemy();

	UFUNCTION(BlueprintPure, Category = Enemy)
	virtual FString GetLastHitEnemyName();

	UFUNCTION(BlueprintPure, Category = Enemy)
	virtual float GetLastHitEnemyHealthPercentage();
};
