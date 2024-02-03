// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "TMDelayPerception.generated.h"

/**
 * This module emulates a delayed perception of events by the actor.
 * By defining a float delay, the actor only perceives new changes after that delay.
 *
 * Perception modules should extent this module and implement FindPercepts to capture
 * any necessary percepts.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMDelayPerception : public UTMModule
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

	UPROPERTY()
	TMap<class AActor*, TWeakObjectPtr<class UTMPercept>> PreviousPercepts;

	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	/// This array is needed to avoid the UE Garbage Collector deleting the percepts while they are awaiting perception. 
	UPROPERTY()
	TArray<class UTMPercept*> Percepts;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "3Motion")
	TArray<UTMPercept*> FindPercepts(const UWorld* World);
	
	void PropagatePercepts(const TArray<UTMPercept*>& ActorPercepts);


public:
	UTMDelayPerception();

	/// This variable defines for how long a new percept takes to be perceived.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	float PerceptionDelayInSeconds = .2f;

	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;
};
