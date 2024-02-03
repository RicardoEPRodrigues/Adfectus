// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../TMModule.h"
#include "TMSingleEmotionSelectModule.generated.h"

/**
 * Single Emotion Select
 * This is a React module for 3Motion that implements an Emotional System that selects the first and second emotion available.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMSingleEmotionSelectModule : public UTMModule
{
	GENERATED_BODY()

private:
	FTimerHandle FirstEmotionTimerHandle;
	FTimerHandle SecondEmotionTimerHandle;

	float LastTimeSeconds;

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

	/// Delay in seconds until the module generates a new first emotion.
	UPROPERTY(Category= "3Motion", BlueprintReadWrite, EditAnywhere)
	float FirstEmotionLoopDelay = 5;

	/// Delay in seconds until the module generates a new second emotion check.
	UPROPERTY(Category= "3Motion", BlueprintReadWrite, EditAnywhere)
	float SecondEmotionLoopDelay = 1;
public:

	virtual void Initialize_Implementation(class UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;
};
