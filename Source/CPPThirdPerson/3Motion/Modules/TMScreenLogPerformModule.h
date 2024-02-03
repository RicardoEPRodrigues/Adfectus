// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "TMScreenLogPerformModule.generated.h"

/**
 * Screen Logging Perform Module
 * This is a perform module for 3Motion that displays textually what action is being performed.
 * It checks if the Owner character is controlled by the player and portrays its context if so.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMScreenLogPerformModule : public UTMModule
{
	GENERATED_BODY()

	int PrintPercept(const class UTMPercept* Percept, int key) const;

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

	/// Number of iterations on the Percepts History by Actor.
	/// Essentially it reflects the number of Actors to display in the log.
	UPROPERTY(Category="3Motion", BlueprintReadWrite, EditAnywhere)
	int MaxActorPerceptsIterations = 3;
	
public:

	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	int32 PlayerIndex = 0;
	
	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;
};
