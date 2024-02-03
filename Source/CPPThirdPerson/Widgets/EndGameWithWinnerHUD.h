// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWithWinnerHUD.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UEndGameWithWinnerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief Called to set the winner's name.
	 */
	UFUNCTION(Category="Gameplay", BlueprintNativeEvent)
	void SetWinnerName(const FString & WinnerName);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonPressedDelegate);
	
	UPROPERTY(BlueprintCallable, Category="Gameplay")
	FButtonPressedDelegate OnRematchButtonEvent;
	
	UPROPERTY(BlueprintCallable, Category="Gameplay")
	FButtonPressedDelegate OnQuitButtonEvent;
};
