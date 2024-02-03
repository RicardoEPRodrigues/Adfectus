#include "Personality.h"

FPersonalityTrait::FPersonalityTrait() : Interval(FFloatInterval(0.0f, 1.0f))
{
}

FPersonalityTrait::FPersonalityTrait(float Min, float Max)
{
	Min = FMath::Clamp(Min, 0.0f, 1.0f);
	Max = FMath::Clamp(Max, 0.0f, 1.0f);

	Interval = FFloatInterval(Min, Max);
}

bool UPersonality::IsTraitActive(const UTMEmotion* Emotion, const float WinLoseRatio)
{
	if (!Emotion) return false;

	const FPersonalityTrait* Trait = PersonalityTraits.Find(FName(Emotion->Name));

	if (!Trait) return false;

	return Trait->Interval.Contains(WinLoseRatio);
}