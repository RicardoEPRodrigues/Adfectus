// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AdfectusGameInstance.generated.h"

UENUM(BlueprintType)
enum class EGameVersion : uint8
{
	Tutorial = 0,
	Version_A = 1,
	Version_B = 2,
	Version_C = 3
};


/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UAdfectusGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	struct FLatentActionInfo transitionLatentInfo;
	FName transitionLevel;

protected:
	UPROPERTY(Category = Levels, EditAnywhere, BlueprintReadWrite)
	bool IsLoadingAsync;

	UPROPERTY()
	APawn* Pawn;

public:
	UAdfectusGameInstance();

	/**
	 * If true a second player is created when starting the main map.
	  */
	UPROPERTY(Category = Multiplayer, EditAnywhere, BlueprintReadWrite)
	bool IsLocalMultiplayer;

	UPROPERTY(Category = Levels, EditAnywhere, BlueprintReadWrite)
	FName CurrentLevel;

	/**
	 * Indicates the current selected version of the game
	 */
	UPROPERTY(Category = Levels, EditAnywhere, BlueprintReadWrite)
	EGameVersion CurrentVersion = EGameVersion::Version_C;

	/**
	 * Indicates the current selected version of the game
	 */
	UPROPERTY(Category = Levels, EditAnywhere, BlueprintReadWrite)
	FString MainMenuLevelName = "MainMenuMap";
	
	/**
	 * Indicates the current selected version of the game
	 */
	UPROPERTY(Category = Levels, EditAnywhere, BlueprintReadWrite)
	FString GameLevelName = "PvPArena";
	
	UFUNCTION(BlueprintCallable, Category = Levels, meta = (Latent, LatentInfo = "LatentInfo"))
	virtual void TransitionLevel(FName previousLevelName, FName levelName, struct FLatentActionInfo LatentInfo);

	UFUNCTION(Category = Levels)
	void OnTransitionLevelUnloadFinished();

	UFUNCTION(BlueprintCallable, Category = Levels, meta = (Latent, LatentInfo = "LatentInfo"))
	virtual void LoadLevelHidden(FName levelName, struct FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, Category = Levels)
	virtual void SetLevelVisibility(FName levelName, bool visibility);


	UFUNCTION(BlueprintCallable, Category = Characters)
	virtual void ReviveAllPlayers();

	UFUNCTION(BlueprintCallable, Category = Characters)
	virtual void RemoveSplitscreenPlayers();
};
