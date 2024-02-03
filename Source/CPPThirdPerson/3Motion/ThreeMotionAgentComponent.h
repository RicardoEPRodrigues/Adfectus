// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThreeMotionAgentComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CPPTHIRDPERSON_API UThreeMotionAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UThreeMotionAgentComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * @brief Reset the component to default state, including all its modules
	 */
	virtual void Reset();

	/**
	 * @brief Theory of mind that holds emotion, action, and other data from the agent.
	 */
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite, Instanced)
	class UTMTheoryOfMind* TheoryOfMind;

	/**
	 * List of available modules used by the agent to perceive the environment and other characters
	 */
	UPROPERTY(Category="3Motion|Modules", EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UTMModule*> Modules;
};
