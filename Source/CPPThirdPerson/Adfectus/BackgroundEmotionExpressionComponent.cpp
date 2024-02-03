// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "BackgroundEmotionExpressionComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "CPPThirdPerson/AdfectusLogging.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"

#define EMOTION_NEUTRAL_INDEX 0
#define EMOTION_CONFIDENCE_INDEX 1
#define EMOTION_DEFENSIVENESS_INDEX 2
#define EMOTION_FEAR_INDEX 3
#define EMOTION_RAGE_INDEX 4

UBackgroundEmotionExpressionComponent::UBackgroundEmotionExpressionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BackgroundFX = LoadObject<UNiagaraSystem>(
		nullptr, TEXT(
			"NiagaraSystem'/Game/Adfectus/HUDAssets/BackgroundExpression/FX_BackgroundExpression.FX_BackgroundExpression'"),
		nullptr, LOAD_None, nullptr);
}

void UBackgroundEmotionExpressionComponent::BeginPlay()
{
	if (!BackgroundFX)
	{
		return;
	}

	BackgroundNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		BackgroundFX, this, NAME_None, FVector(0), FRotator(0), EAttachLocation::Type::SnapToTarget,
		false, false);

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

void UBackgroundEmotionExpressionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!BackgroundNiagaraComp)
	{
		return;
	}

	if (!AgentComponent)
	{
		return;
	}

	float EmotionIndex = EMOTION_NEUTRAL_INDEX;

	if (const UTMEmotion* Emotion = AgentComponent->TheoryOfMind->GetStrongestEmotion())
	{
		if (Emotion->Name.Equals("Confidence"))
		{
			EmotionIndex = EMOTION_CONFIDENCE_INDEX;
		}
		else if (Emotion->Name.Equals("Defensiveness"))
		{
			EmotionIndex = EMOTION_DEFENSIVENESS_INDEX;
		}
		else if (Emotion->Name.Equals("Fear"))
		{
			EmotionIndex = EMOTION_FEAR_INDEX;
		}
		else if (Emotion->Name.Equals("Rage"))
		{
			EmotionIndex = EMOTION_RAGE_INDEX;
		}
	}

	if (PreviousEmotionIndex != EmotionIndex)
	{
		PreviousEmotionIndex = EmotionIndex;

		if (PreviousEmotionIndex != EMOTION_NEUTRAL_INDEX)
		{			
			BackgroundNiagaraComp->SetNiagaraVariableFloat(FString("EmotionIndex"), PreviousEmotionIndex);
			BackgroundNiagaraComp->Activate(true);
		}
	}
}
