#pragma once

#include "CoreMinimal.h"
#include "TMEmotion.h"
#include "Personality.generated.h"

USTRUCT(BlueprintType)
struct FPersonalityTrait
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0.0f, ClampMax=1.0f))
	FFloatInterval Interval;

	FPersonalityTrait();
	FPersonalityTrait(float Min, float Max);
};

/**
 * @brief A personality is a set of personality traits described as intervals within [0,1].
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UPersonality : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FPersonalityTrait> PersonalityTraits;

	/**
	 * Checks whether a particular personality trait is active given a personality and win-lose ratio.
	 * If the personality does not contain the trait then it returns false.
	*/
	UFUNCTION(BlueprintPure)
	bool IsTraitActive(const UTMEmotion* Emotion, const float WinLoseRatio);
};