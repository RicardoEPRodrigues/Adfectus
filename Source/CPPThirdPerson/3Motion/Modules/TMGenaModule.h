// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "TMGenaModule.generated.h"

/**
 * GEnA - Genial Emotional informed Actions
 * This is a Decide module for 3Motion that implements a Decision-making System.
 * Our current implementation only formats an action depending on the player input.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMGenaModule : public UTMModule
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

	UPROPERTY()
	class UTMAction* Action;

public:

	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;

	UFUNCTION()
	void OnActorInterruptibleAnticipation();
	UFUNCTION()
	void OnActorUninterruptibleAnticipation();
	UFUNCTION()
	void OnActorUninterruptibleFollowthrough();
	UFUNCTION()
	void OnActorInterruptibleFollowthrough();
	UFUNCTION()
	void OnActorInterrupted();
};
