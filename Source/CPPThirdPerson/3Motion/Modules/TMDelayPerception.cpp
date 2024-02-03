// Fill out your copyright notice in the Description page of Project Settings.


#include "TMDelayPerception.h"

#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMAction.h"
#include "CPPThirdPerson/3Motion/TMLogging.h"
#include "CPPThirdPerson/3Motion/TMotionUtils.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"
#include "Kismet/GameplayStatics.h"

UTMDelayPerception::UTMDelayPerception()
{
}

void UTMDelayPerception::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
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

void UTMDelayPerception::Execute_Implementation()
{
	if (!Owner) return;

	// Remove all inactive Timers
	if (TimerHandles.Num() > 0)
	{
		TimerHandles.RemoveAll([&](const FTimerHandle Timer)
		{
			return !GetWorld()->GetTimerManager().IsTimerActive(Timer);
		});
	}

	// Find all percepts and add to percepts list
	const UWorld* world = GetWorld();
	if (!world) return;

	const TArray<UTMPercept*> ActorPercepts = FindPercepts(world);
	if (ActorPercepts.IsEmpty()) return;
	//UE_LOG(LogTemp, Warning, TEXT("Found percepts!"));
	Percepts.Append(ActorPercepts);

	// If there is a delay in perception then use TimerDelegate otherwise add percepts to history immediately
	if (PerceptionDelayInSeconds > 0.0f)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&, ActorPercepts]
		{
			PropagatePercepts(ActorPercepts);
		});

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, PerceptionDelayInSeconds, false);
		TimerHandles.Add(TimerHandle);
	}
	else
	{
		PropagatePercepts(ActorPercepts);
	}
}

void UTMDelayPerception::EndPlay_Implementation()
{
	for (FTimerHandle TimerHandle : TimerHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	TimerHandles.Empty();
	PreviousPercepts.Empty();
	Percepts.Empty();
}

TArray<UTMPercept*> UTMDelayPerception::FindPercepts_Implementation(const UWorld* World)
{
	return TArray<UTMPercept*>();
}

void UTMDelayPerception::PropagatePercepts(const TArray<UTMPercept*>& ActorPercepts)
{
	// UE_LOG(Log3Motion, Log, TEXT("Updating %d characters."), ActorPercepts.Num())
	for (auto&& Percept : ActorPercepts)
	{
		Percepts.Remove(Percept);
		if (!Percept || !Percept->IsValidLowLevel())
		{
			UE_LOG(Log3Motion, Warning, TEXT("Percept is null or Garbage Collected."))
			continue;
		}

		if (!Percept->Actor || Percept->Time < 0)
		{
			continue;
		}

		bool AddToHistory = false;
		if (!PreviousPercepts.Contains(Percept->Actor))
		{
			// It is a new percept from an previously unknown actor.
			PreviousPercepts.Add(Percept->Actor, Percept);
			AddToHistory = true;
		}
		else
		{
			TWeakObjectPtr<UTMPercept> PreviousPerceptPtr = PreviousPercepts[Percept->Actor];
			const UTMPercept* PreviousPercept = PreviousPerceptPtr.Get();
			if (PreviousPercept && !Percept->IsEquivalent(*PreviousPercept))
			{
				// It is a new percept from a known actor.
				PreviousPercepts[Percept->Actor] = Percept;
				AddToHistory = true;
			} 
		}
		// Else it may be a new percept, but it is equivalent to the old one.

		if (AddToHistory)
		{
			OwnerAgentComponent->TheoryOfMind->AddPerceptToHistory(Percept);

			// LOG For debug purposes.
			// #ifdef UE_BUILD_DEBUG
			// 				const float Time = Percept->Time;
			// 				if (Percept->Action || Percept->Emotion)
			// 				{
			// 					if (Percept->Action)
			// 					{
			// 						UE_LOG(Log3Motion, Log, TEXT("%s perceived that %s %s and is in stage %s at %f."),
			// 						       *Owner->GetFName().ToString(), *GetNameSafe(Percept->Actor),
			// 						       *Percept->Action->Description, *UEnum::GetValueAsString(Percept->Action->Stage), Time)
			// 					}
			//
			// 					if (Percept->Emotion)
			// 					{
			// 						UE_LOG(Log3Motion, Log, TEXT("%s perceived that %s is feeling at %f: %s"),
			// 						       *Owner->GetFName().ToString(),
			// 						       *Percept->Actor->GetFName().ToString(), Time, *Percept->Emotion->Name)
			// 					}
			// 				}
			// 				else
			// 				{
			// 					UE_LOG(Log3Motion, Log, TEXT("%s perceived something new from %s at %f."),
			// 					       *Owner->GetFName().ToString(),
			// 					       *Percept->Actor->GetFName().ToString(), Time)
			// 				}
			// #endif
		}
	}
}
