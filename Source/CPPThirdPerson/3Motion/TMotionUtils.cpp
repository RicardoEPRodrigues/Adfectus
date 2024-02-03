// Fill out your copyright notice in the Description page of Project Settings.


#include "TMotionUtils.h"

#include "TMEmotion.h"
#include "TMAction.h"
#include "TMPercept.h"

unsigned long UTMotionUtils::PerceptCount = 0;
unsigned long UTMotionUtils::EmotionCount = 0;
unsigned long UTMotionUtils::ActionCount = 0;

UTMEmotion* UTMotionUtils::DuplicateEmotion(const UTMEmotion* Emotion, UObject* Outer, const FString Name)
{
	if (!Emotion || !Emotion->IsValidLowLevel())
	{
		return nullptr;
	}

	if (!Outer)
	{
		Outer = Emotion->GetOuter();
	}

	FName FinalName;
	if (!Name.IsEmpty())
	{
		FinalName = FName(*Name);
	}
	else
	{
		FinalName = FName(FString::Printf(TEXT("GeneratedEmotion_%d"), ++EmotionCount));
	}

	return DuplicateObject(Emotion, Outer, FinalName);
}

UTMAction* UTMotionUtils::DuplicateAction(const UTMAction* Action, UObject* Outer, const FString Name)
{
	if (!Action || !Action->IsValidLowLevel())
	{
		return nullptr;
	}

	if (!Outer)
	{
		Outer = Action->GetOuter();
	}

	FName FinalName;
	if (!Name.IsEmpty())
	{
		FinalName = FName(*Name);
	}
	else
	{
		FinalName = FName(FString::Printf(TEXT("GeneratedAction_%d"), ++ActionCount));
	}

	return DuplicateObject(Action, Outer, FinalName);
}

UTMPercept* UTMotionUtils::CreatePercept(AActor* Actor, const float Time, UObject* Outer, UTMPerceptData* Data, const FString Name)
{
	if (!Actor || !Actor->IsValidLowLevel() || !Outer || !Outer->IsValidLowLevel() || !Data || !Data->IsValidLowLevel())
	{
		return nullptr;
	}

	FName FinalName;
	if (!Name.IsEmpty())
	{
		FinalName = FName(*Name);
	}
	else
	{
		FinalName = FName(FString::Printf(TEXT("GeneratedPercept_%d"), ++PerceptCount));
	}

	UTMPercept* Percept = NewObject<UTMPercept>(Outer, FinalName);

	Percept->Actor = Actor;
	Percept->Time = Time;
	Percept->Data = Data;

	return Percept;
}
