// Fill out your copyright notice in the Description page of Project Settings.


#include "TMScreenLogPerformModule.h"

#include "TMCharacterPerception.h"
#include "../TMLogging.h"
#include "../ThreeMotionAgentComponent.h"
#include "../../CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMAction.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "Kismet/GameplayStatics.h"
#include "../TMTheoryOfMind.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"

void UTMScreenLogPerformModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
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
}

void UTMScreenLogPerformModule::Execute_Implementation()
{
	// Start Logging
	const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, PlayerIndex);
	if (!PlayerChar || PlayerChar != Owner)
	{
		return;
	}

	int key = 1;
	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::White, FString::Printf(TEXT("%s"), *GetNameSafe(Owner)), false);
	if (OwnerAgentComponent->TheoryOfMind->CurrentAction)
	{
		GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Cyan, FString::Printf(TEXT("Action: %s in %s"),
			                                 *OwnerAgentComponent->TheoryOfMind->CurrentAction->Name,
			                                 *UEnum::GetValueAsString(
				                                 OwnerAgentComponent->TheoryOfMind->CurrentAction->Stage)),
		                                 false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Cyan, TEXT("Action: No Action"), false);
	}
	if (!OwnerAgentComponent->TheoryOfMind->CurrentEmotions.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Cyan, FString::Printf(
			                                 TEXT("Emotion: %s with %f intensity"),
			                                 *OwnerAgentComponent->TheoryOfMind->CurrentEmotions[0]->Name,
			                                 OwnerAgentComponent->TheoryOfMind->CurrentEmotions[0]->Intensity), false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Cyan, TEXT("Emotion: No Emotion"), false);
	}
	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Cyan, TEXT(" "), false);

	// Display current knowledge about X characters.
	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, TEXT("Percept History:"), false);
	int i = 0;
	for (auto Elem : OwnerAgentComponent->TheoryOfMind->PerceptHistoryPerActor)
	{
		if (Elem.Value.Percepts.Num() > 0)
		{
			UTMPercept* PastPercept = Elem.Value.Percepts.Last().Get();
			if (PastPercept)
			{
				key = PrintPercept(PastPercept, key);
			}
		}

		if (++i > MaxActorPerceptsIterations)
		{
			break;
		}
	}
}

void UTMScreenLogPerformModule::EndPlay_Implementation()
{
}

int UTMScreenLogPerformModule::PrintPercept(const class UTMPercept* Percept, int key) const
{
	if (!Percept)
	{
		return key;
	}

	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green,
	                                 FString::Printf(TEXT("\t Actor: %s"), *GetNameSafe(Percept->Actor)), false);
	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green,
	                                 FString::Printf(TEXT("\t Time: %f seconds"), Percept->Time), false);

	if (const UTMCharacterPerceptData* PerceptData = Cast<UTMCharacterPerceptData>(Percept->Data))
	{
		if (PerceptData->Action)
		{
			GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, FString::Printf(TEXT("Action: %s in %s"),
												 *PerceptData->Action->Name,
												 *UEnum::GetValueAsString(PerceptData->Action->Stage)),
											 false);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, TEXT("Action: No Action"), false);
		}
		if (!PerceptData->Emotions.IsEmpty())
		{
			if (PerceptData->Emotions.Num() == 1)
			{
				GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, FString::Printf(
													 TEXT("Emotion: %s with %f intensity"),
													 *PerceptData->Emotions[0]->Name,
													 PerceptData->Emotions[0]->Intensity), false);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, TEXT("Emotions:"), false);
				for (const auto Emotion : PerceptData->Emotions)
				{
					GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, FString::Printf(
														 TEXT("\t%s with %f intensity"),
														 *Emotion->Name,
														 Emotion->Intensity), false);
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, TEXT("Emotions: No Emotion"), false);
		}
	}
	
	GEngine->AddOnScreenDebugMessage(key++, 0, FColor::Green, TEXT(" "), false);

	return key;
}
