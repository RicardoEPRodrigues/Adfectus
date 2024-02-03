// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TMPercept.h"
#include "UObject/Object.h"
#include "TMotionUtils.generated.h"

/**
 * A utilities class to hold static functions to ease development.
 */
UCLASS()
class CPPTHIRDPERSON_API UTMotionUtils : public UObject
{
	GENERATED_BODY()

	static unsigned long PerceptCount;
	static unsigned long EmotionCount;
	static unsigned long ActionCount;

public:
	/**
	 * @brief Duplicates an emotion.
	 * @param Emotion Emotion to duplicate.
	 * @param Outer Outer UObject that gives context to this new emotion. If null is provided it will make use of the Emotion->GetOuter().
	 * @param Name Optional name to give to the emotion. If the string is empty, a name will be generated.
	 * @return A new Emotion duplicated from the provided emotion. Can be null if Emotion is null or not valid.
	 */
	static class UTMEmotion* DuplicateEmotion(const class UTMEmotion* Emotion, UObject* Outer = nullptr,
	                                          FString Name = FString());
	/**
	 * @brief Duplicates an action.
	 * @param Action Action to duplicate.
	 * @param Outer Outer UObject that gives context to this new action. If null is provided it will make use of the Action->GetOuter().
	 * @param Name Optional name to give to the action. If the string is empty, a name will be generated.
	 * @return A new Action duplicated from the provided action. Can be null if Action is null or not valid.
	 */
	static class UTMAction* DuplicateAction(const class UTMAction* Action, UObject* Outer = nullptr,
	                                        FString Name = FString());

	/**
	 * @brief Creates a new Percept object based on the input parameters.
	 * @param Actor Actor that owns and from which originates this percept.
	 * @param Time World time of the moment this percept was created (not when it was perceived)
	 * @param Outer Outer UObject that gives context to this new percept.
	 * @param Data UDataAsset that represents the data associated with this percept 
	 * @param Name Optional name to give to the percept. If the string is empty, a name will be generated.
	 * @return A new Percept with the information provided. Can be null if actor or outer are invalid.
	 */
	static class UTMPercept* CreatePercept(AActor* Actor, float Time, UObject* Outer, UTMPerceptData* Data, FString Name = FString());
};
