#include "TMBaltasModule.h"

#include "BaltasPersonality.h"
#include "Emotivector.h"
#include "../TMLogging.h"
#include "../ThreeMotionAgentComponent.h"
#include "../../CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMEmotion.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"

UTMBaltasModule::UTMBaltasModule()
{
	Personality = LoadObject<UBaltasPersonality>(
		nullptr, TEXT(
			"BaltasPersonality'/Game/Adfectus/BALTAS/Blueprints/Personalities/BaltasPersonality_Default.BaltasPersonality_Default'"),
		nullptr, LOAD_None, nullptr);
}

void UTMBaltasModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
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

	for (UEmotivector* Emotivector : Emotivectors)
	{
		if (!Emotivector)
		{
			UE_LOG(Log3Motion, Warning, TEXT("Emotivector is not defined!"));
			continue;
		}
		Emotivector->Initialize(this);
	}

	// Successfully 
	// UE_LOG(Log3Motion, Log, TEXT("Successfully initialized %s on %s"), *GetNameSafe(this), *GetNameSafe(Owner))
}

void UTMBaltasModule::Execute_Implementation()
{
	SalientSensations.RemoveAll([&](const USensation* Sensation)
	{
		return Sensation->Lifetime <= 0;
	});

	for (const auto Emotivector : Emotivectors)
	{
		if (!Emotivector)
		{
			UE_LOG(Log3Motion, Warning, TEXT("Emotivector is not defined!"));
			return;
		}
		TArray<USensation*> Sensations = Emotivector->Update();
		SalientSensations.Append(Sensations);
	}

	TArray<TPair<USensation*, UTMEmotion*>> PossibleEmotions;
	for (const auto Sensation : SalientSensations)
	{
		Sensation->Lifetime -= GetWorld()->DeltaTimeSeconds;
		if (Sensation->Lifetime <= 0)
		{
			continue;
		}
		// const float SalienceMultiplier = Sensation->Lifetime / Sensation->InitialLifetime;
		// Sensation->Salience = Sensation->InitialSalience * SalienceMultiplier * Sensation->Importance * Sensation->Emotivector->Importance;
		Sensation->Salience = Sensation->InitialSalience * Sensation->Importance * Sensation->Emotivector->Importance;

		if (Personality)
		{
			for (UTMEmotion* AvailableEmotion : OwnerAgentComponent->TheoryOfMind->AvailableEmotions)
			{
				if (Personality->IsTraitActive(AvailableEmotion, Sensation))
				{
					// This is not copying the emotion, which can create issues.
					// Use UTMotionUtils::DuplicateEmotion if you want copies.
					PossibleEmotions.Add(TPair<USensation*, UTMEmotion*>(Sensation, AvailableEmotion));
				}
			}
		}
	}
	OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Empty();
	
	// UTMEmotion* EmotionToFeel = SelectMostSalientEmotion(PossibleEmotions);
	// UTMEmotion* EmotionToFeel = SelectMostCommonEmotion(PossibleEmotions);
	// UTMEmotion* EmotionToFeel = SelectRarityEmotion(PossibleEmotions);
	UTMEmotion* EmotionToFeel = SelectPowerfulEmotion(PossibleEmotions);

	if (EmotionToFeel)
	{
		// UE_LOG(Log3Motion, Log, TEXT("%s - %s -> Emotion: %s"), *GetNameSafe(this->Owner),
		// 	   *GetNameSafe(this), *EmotionToFeel->Name);
		OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Add(EmotionToFeel);
	}
}

void UTMBaltasModule::EndPlay_Implementation()
{
	for (const auto Emotivector : Emotivectors)
	{
		if (!Emotivector)
		{
			UE_LOG(Log3Motion, Warning, TEXT("Emotivector is not defined!"));
			return;
		}
		Emotivector->Reset();
	}
}

UTMEmotion* UTMBaltasModule::SelectMostSalientEmotion(TArray<TPair<USensation*, UTMEmotion*>> PossibleEmotions)
{
	UTMEmotion* EmotionToFeel = nullptr;
	if (PossibleEmotions.Num() > 0)
	{
		const TPair<USensation*, UTMEmotion*>* MostSalientEmotion = &PossibleEmotions[0];
		for (auto PossibleEmotion : PossibleEmotions)
		{
			if (MostSalientEmotion->Key->Salience < PossibleEmotion.Key->Salience)
			{
				MostSalientEmotion = &PossibleEmotion;
			}
		}
		EmotionToFeel = MostSalientEmotion->Value;
	}
	return EmotionToFeel;
}

UTMEmotion* UTMBaltasModule::SelectMostCommonEmotion(TArray<TPair<USensation*, UTMEmotion*>> PossibleEmotions)
{
	UTMEmotion* EmotionToFeel = nullptr;
	if (PossibleEmotions.Num() > 0)
	{
		TMap<UTMEmotion*, int> EmotionFrequency;
		for (const auto PossibleEmotion : PossibleEmotions)
		{
			if (UTMEmotion* Emotion = PossibleEmotion.Value)
			{
				if (EmotionFrequency.Contains(Emotion))
				{
					EmotionFrequency[Emotion] = EmotionFrequency[Emotion] + 1;
				}
				else
				{
					EmotionFrequency.Add(Emotion, 1);
				}
			}
		}
		if (!EmotionFrequency.IsEmpty())
		{
			int MostFrequent = -1;
			for (const auto Frequency : EmotionFrequency)
			{
				if (MostFrequent < Frequency.Value)
				{
					EmotionToFeel = Frequency.Key;
					MostFrequent = Frequency.Value;
				}
			}
		}
	}
	return EmotionToFeel;
}

UTMEmotion* UTMBaltasModule::SelectRarityEmotion(TArray<TPair<USensation*, UTMEmotion*>> PossibleEmotions)
{
	UTMEmotion* EmotionToFeel = nullptr;
	if (PossibleEmotions.Num() > 0)
	{
		TMap<UTMEmotion*, TPair<int, float>> EmotionFrequency;
		for (const auto PossibleEmotion : PossibleEmotions)
		{
			if (UTMEmotion* Emotion = PossibleEmotion.Value)
			{
				if (EmotionFrequency.Contains(Emotion))
				{
					TPair<int, float>& Container = EmotionFrequency[Emotion];
					Container.Key = Container.Key + 1;
					if (Container.Value < PossibleEmotion.Key->Salience)
					{
						Container.Value = PossibleEmotion.Key->Salience;
					}
				}
				else
				{
					EmotionFrequency.Add(Emotion, TPair<int, float>(1, PossibleEmotion.Key->Salience));
				}
			}
		}
		if (!EmotionFrequency.IsEmpty())
		{
			EmotionFrequency.ValueSort([](const TPair<int, float>& A, const TPair<int, float>& B)
			{
				if (A.Key == B.Key)
				{
					return A.Value > B.Value;
				}
				return A.Key > B.Key;
			});
			for (const auto Frequency : EmotionFrequency)
			{
				if (!EmotionToFeel)
				{
					EmotionToFeel = Frequency.Key;
					break;
				}
			}
		}
	}
	return EmotionToFeel;
}

UTMEmotion* UTMBaltasModule::SelectPowerfulEmotion(TArray<TPair<USensation*, UTMEmotion*>> PossibleEmotions)
{
	UTMEmotion* EmotionToFeel = nullptr;
	if (PossibleEmotions.Num() > 0)
	{
		TMap<UTMEmotion*, float> EmotionFrequency;
		for (const auto PossibleEmotion : PossibleEmotions)
		{
			if (UTMEmotion* Emotion = PossibleEmotion.Value)
			{
				if (EmotionFrequency.Contains(Emotion))
				{
					EmotionFrequency[Emotion] += PossibleEmotion.Key->Salience;
				}
				else
				{
					EmotionFrequency.Add(Emotion, PossibleEmotion.Key->Salience);
				}
			}
		}
		if (!EmotionFrequency.IsEmpty())
		{
			EmotionFrequency.ValueSort([](const float& A, const float& B)
			{
				return A > B;
			});
			for (const auto Frequency : EmotionFrequency)
			{
				if (!EmotionToFeel)
				{
					EmotionToFeel = Frequency.Key;
					break;
				}
			}
		}
	}
	return EmotionToFeel;
}
