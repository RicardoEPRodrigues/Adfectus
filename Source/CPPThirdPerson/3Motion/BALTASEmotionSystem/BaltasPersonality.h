// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Emotivector.h"
#include "Engine/DataAsset.h"
#include "UObject/Object.h"
#include "BaltasPersonality.generated.h"

USTRUCT(BlueprintType)
struct FBaltasPersonalityTrait
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EmotionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESensationValence Valence = ESensationValence::Other;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESensationType Type = ESensationType::Other;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0.0f, ClampMax=1.0f))
	FFloatInterval SalienceInterval;
};

/**
 * Defines a Personality for the Baltas Module. It is a mapping between a Sensation and a Salience to output an emotion.
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UBaltasPersonality : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Map between emotion names and a personality trait.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBaltasPersonalityTrait> PersonalityTraits;

	/**
	 * Checks whether a particular personality trait is active given a personality and sensation.
	 * If the personality does not contain the trait then it returns false.
	*/
	UFUNCTION(BlueprintPure)
	bool IsTraitActive(const UTMEmotion* Emotion, const USensation* Sensation);
};
