// Fill out your copyright notice in the Description page of Project Settings.


#include "TMSingleEmotionSelectModule.h"
#include "../TMLogging.h"
#include "../ThreeMotionAgentComponent.h"
#include "../../CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMotionUtils.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "../TMTheoryOfMind.h"

void UTMSingleEmotionSelectModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
{
	OwnerAgentComponent = OwnerComponent;
	if (!OwnerAgentComponent)
	{
		UE_LOG(Log3Motion, Warning, TEXT("Owner Agent Component is Null!"))
		return;
	}

	Owner = OwnerAgentComponent->GetOwner<ACPPThirdPersonCharacter>();
	if (!Owner)
	{
		UE_LOG(Log3Motion, Warning, TEXT("Owner is not a CPPThirdPersonCharacter!"))
		return;
	}
	// Successfully 
	// UE_LOG(Log3Motion, Log, TEXT("Successfully initialized %s on %s"), *GetNameSafe(this), *GetNameSafe(Owner))


	if (!Owner)
	{
		return;
	}
	if (OwnerAgentComponent->TheoryOfMind->AvailableEmotions.Num() <= 0)
	{
		return;
	}

	// Every 5 second move to first emotion
	FTimerDelegate FirstEmotionTimerDelegate;
	FirstEmotionTimerDelegate.BindLambda([&]
	{
		if (!IsEnabled)
		{
			return;
		}

		if (OwnerAgentComponent->TheoryOfMind->AvailableEmotions.Num() > 0)
		{
			OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Empty();
			OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Add(
				OwnerAgentComponent->TheoryOfMind->AvailableEmotions[0]);
			// UE_LOG(Log3Motion, Log, TEXT("%s is feeling %s"), *GetNameSafe(Owner),
			//        *OwnerAgentComponent->DesiredEmotion->Description)
		}
	});
	const float FirstDelay = FMath::RandRange(0.f, FirstEmotionLoopDelay);
	GetWorld()->GetTimerManager().SetTimer(FirstEmotionTimerHandle, FirstEmotionTimerDelegate, FirstEmotionLoopDelay,
	                                       true, FirstDelay);


	// Every Y second, if a new percepts arrives, switch to second emotion
	if (OwnerAgentComponent->TheoryOfMind->AvailableEmotions.Num() > 1)
	{
		FTimerDelegate SecondEmotionTimerDelegate;
		SecondEmotionTimerDelegate.BindLambda([&]
		{
			if (!IsEnabled)
			{
				return;
			}

			if (OwnerAgentComponent->TheoryOfMind->AvailableEmotions.Num() > 1)
			{
				UTMPercept* Percept = nullptr;
				// Find a recent percept that is earlier than the last time we searched.
				const int Count = OwnerAgentComponent->TheoryOfMind->PerceptHistory.Num();
				for (int i = Count - 1; i >= FMath::Max(Count - 10, 0); --i)
				{
					if (OwnerAgentComponent->TheoryOfMind->PerceptHistory[i]->Time > LastTimeSeconds)
					{
						Percept = OwnerAgentComponent->TheoryOfMind->PerceptHistory[i];
						break;
					}
				}
				if (Percept)
				{
					OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Empty();
					UTMEmotion* Emotion = UTMotionUtils::DuplicateEmotion(
						OwnerAgentComponent->TheoryOfMind->AvailableEmotions[1]);
					Emotion->Reasons.Add(Percept);
					OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Add(Emotion);
				}

				LastTimeSeconds = GetWorld()->GetTimeSeconds();
			}
		});

		LastTimeSeconds = GetWorld()->GetTimeSeconds();
		const float SecondEmotionFirstDelay = FMath::RandRange(0.f, SecondEmotionLoopDelay);
		GetWorld()->GetTimerManager().SetTimer(SecondEmotionTimerHandle, SecondEmotionTimerDelegate,
		                                       SecondEmotionLoopDelay, true, SecondEmotionFirstDelay);
	}
}

void UTMSingleEmotionSelectModule::Execute_Implementation()
{
}

void UTMSingleEmotionSelectModule::EndPlay_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(FirstEmotionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SecondEmotionTimerHandle);
}
