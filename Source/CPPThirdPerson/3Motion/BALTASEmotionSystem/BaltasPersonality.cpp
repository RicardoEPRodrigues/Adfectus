// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "BaltasPersonality.h"

#include "CPPThirdPerson/3Motion/TMEmotion.h"

bool UBaltasPersonality::IsTraitActive(const UTMEmotion* Emotion, const USensation* Sensation)
{
	if (!Emotion || !Sensation) return false;

	for (auto Trait : PersonalityTraits)
	{
		if (Trait.EmotionName.Equals(Emotion->Name) &&
			Trait.Valence == Sensation->Valence &&
			Trait.Type == Sensation->Type)
		{
			float Salience = Sensation->Salience;
			if (Salience > 1)
			{
				Salience = 1;
			}
			return Trait.SalienceInterval.Contains(Salience);
		}
	}
	return false;
}
