#pragma once

#include "TMDelayPerception.h"
#include "CPPThirdPerson/3Motion/TMAction.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "TMCharacterPerception.generated.h"

UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UTMCharacterPerceptData : public UTMPerceptData
{
	GENERATED_BODY()
public:
	/// Action being acted by the Actor of this percept
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	UTMAction* Action;

	/// Emotions being felt by the Actor of this percept
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	TArray<UTMEmotion*> Emotions;

	/// Character health at the time of this percept
	float Health;

	/// Character health at the time of this percept
	float NormalizedHealth;

	/// Character's transformations at the time of this percept
	FTransform Transform;

	virtual bool Equals_Implementation(const UTMPerceptData* Other) override;
};

/**
 * This module emulates the perception of events from other characters,
 * such as action, action stage, health, position etc...
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMCharacterPerception : public UTMDelayPerception
{
	GENERATED_BODY()

protected:
	virtual TArray<UTMPercept*> FindPercepts_Implementation(const UWorld* World) override;
};
