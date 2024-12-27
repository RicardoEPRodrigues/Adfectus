// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "EmojiFacialExpressionWidgetComponent.h"

#include "CPPThirdPerson/AdfectusLogging.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"

#define EMOTION_NEUTRAL_INDEX 0
#define EMOTION_CONFIDENCE_INDEX 1
#define EMOTION_DEFENSIVENESS_INDEX 2
#define EMOTION_FEAR_INDEX 3
#define EMOTION_RAGE_INDEX 4

UEmojiFacialExpressionWidgetComponent::UEmojiFacialExpressionWidgetComponent()
{
	EmojiMaterial = LoadObject<UMaterialInterface>(
		nullptr, TEXT(
			"MaterialInstanceConstant'/Game/Adfectus/HUDAssets/EmojiFacialExpression/MI_EmojiFacialExpression.MI_EmojiFacialExpression'"),
		nullptr, LOAD_None,
		nullptr);
}

void UEmojiFacialExpressionWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (EmojiMaterial)
	{
		SetMaterial(0, EmojiMaterial);
	}
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue("EmotionIndex", EMOTION_NEUTRAL_INDEX);
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

void UEmojiFacialExpressionWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MaterialInstance)
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
		MaterialInstance->SetScalarParameterValue("EmotionIndex", PreviousEmotionIndex);
	}
}
