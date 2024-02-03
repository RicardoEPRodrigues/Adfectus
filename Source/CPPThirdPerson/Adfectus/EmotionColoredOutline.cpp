// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "EmotionColoredOutline.h"

#include "CPPThirdPerson/AdfectusLogging.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"


// Sets default values for this component's properties
UEmotionColoredOutline::UEmotionColoredOutline()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEmotionColoredOutline::BeginPlay()
{
	Super::BeginPlay();

	if (OutlinedMeshes.IsEmpty())
	{
		UE_LOG(LogAdfectus, Warning, TEXT("No Body Mesh initialized!"));
	}

	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	AgentComponent = Owner->FindComponentByClass<UThreeMotionAgentComponent>();
	if (!AgentComponent)
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Failed to find an 3motion Agent Component!"));
	}
}


// Called every frame
void UEmotionColoredOutline::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!AgentComponent)
	{
		return;
	}


	const UTMEmotion* Emotion = AgentComponent->TheoryOfMind->GetStrongestEmotion();

	if (!Emotion)
	{
		for (auto Mesh : OutlinedMeshes)
		{
			Mesh->SetRenderCustomDepth(false);
		}
	}
	else
	{
		for (auto Mesh : OutlinedMeshes)
		{
			Mesh->SetRenderCustomDepth(true);
			if (Emotion->Name.Equals("Confidence"))
			{
				Mesh->SetCustomDepthStencilValue(StencilStepConfidence);
			}
			else if (Emotion->Name.Equals("Defensiveness"))
			{
				Mesh->SetCustomDepthStencilValue(StencilStepDefensiveness);
			}
			else if (Emotion->Name.Equals("Fear"))
			{
				Mesh->SetCustomDepthStencilValue(StencilStepFear);
			}
			else if (Emotion->Name.Equals("Rage"))
			{
				Mesh->SetCustomDepthStencilValue(StencilStepRage);
			}
		}
	}
}
