// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreeMotionAgentComponent.h"

#include "TMLogging.h"
#include "TMPercept.h"
#include "TMTheoryOfMind.h"
#include "Modules/TMDelayPerception.h"


// Sets default values for this component's properties
UThreeMotionAgentComponent::UThreeMotionAgentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TheoryOfMind = LoadObject<UTMTheoryOfMind>(
		nullptr, TEXT(
			"TMTheoryOfMind'/Game/Adfectus/Blueprints/Characters/3Motion/BP_TheoryOfMind.BP_TheoryOfMind'"),
		nullptr, LOAD_None, nullptr);
}


// Called when the game starts
void UThreeMotionAgentComponent::BeginPlay()
{
	Super::BeginPlay();

	// For each Module initialize with a reference to this component.
	for (auto& module : Modules)
	{
		module->Initialize(this);
	}
}


// Called every frame
void UThreeMotionAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update each Module
	for (auto& module : Modules)
	{
		if (module->IsEnabled)
		{
			module->Execute();
		}
	}

	TheoryOfMind->RemoveOldPercepts();
}

void UThreeMotionAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// EndPlay on each Module
	for (auto& module : Modules)
	{
		module->EndPlay();
	}
}

void UThreeMotionAgentComponent::Reset()
{
	// EndPlay on each Module
	for (auto& module : Modules)
	{
		module->EndPlay();
	}

	TheoryOfMind->Reset();
}
