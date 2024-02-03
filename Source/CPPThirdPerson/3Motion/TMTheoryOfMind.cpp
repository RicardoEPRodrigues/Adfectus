// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "TMTheoryOfMind.h"

#include "TMEmotion.h"
#include "TMLogging.h"
#include "TMPercept.h"

UTMTheoryOfMind::UTMTheoryOfMind()
{
	PerceptHistory.Reserve(PerceptHistoryWindow + 2);
}

void UTMTheoryOfMind::AddPerceptToHistory(UTMPercept* Percept)
{
	if (!Percept || !Percept->IsValidLowLevel())
	{
		UE_LOG(Log3Motion, Warning, TEXT("Percept is null or Garbage Collected."))
		return;
	}

	PerceptHistory.Add(Percept);
	if (Percept->Actor)
	{
		if (PerceptHistoryPerActor.Contains(Percept->Actor))
		{
			PerceptHistoryPerActor[Percept->Actor].Percepts.Add(Percept);
		}
		else
		{
			FPerceptContainer Container;
			Container.Percepts.Add(Percept);
			PerceptHistoryPerActor.Add(Percept->Actor, Container);
		}
	}

	_HasNewPercepts = true;
}

void UTMTheoryOfMind::RemoveOldPercepts()
{
	_HasNewPercepts = false;

	if (PerceptHistory.Num() > PerceptHistoryWindow)
	{
		for (int i = 0; i < PerceptHistory.Num() - PerceptHistoryWindow; ++i)
		{
			for (auto& Elem : PerceptHistoryPerActor)
			{
				if (Elem.Key != PerceptHistory[i]->Actor)
				{
					continue;
				}

				UTMPercept* PastPercept = PerceptHistory[i];
				Elem.Value.Percepts.RemoveAll([PastPercept](TWeakObjectPtr<UTMPercept> Other)
				{
					const UTMPercept* Perc = Other.Get();
					if (!Perc)
					{
						return true;
					}
					if (PastPercept && (PastPercept == Perc || *PastPercept == *Perc))
					{
						return true;
					}
					return false;
				});
			}
		}
		// UE_LOG(Log3Motion, Log, TEXT("%s is deleting old percepts. Count: %d. Window: %d"), *GetNameSafe(this),
		//        PerceptHistory.Num(), PerceptHistoryWindow)
		PerceptHistory.RemoveAt(0, PerceptHistory.Num() - PerceptHistoryWindow);
	}
}

bool UTMTheoryOfMind::HasNewPercepts()
{
	return !PerceptHistory.IsEmpty() && _HasNewPercepts;
}

void UTMTheoryOfMind::Reset()
{
	DesiredAction = nullptr;
	DesiredEmotions.Empty();
	CurrentAction = nullptr;
	CurrentEmotions.Empty();
	PerceptHistory.Empty();
	PerceptHistoryPerActor.Empty();
}

UTMEmotion* UTMTheoryOfMind::GetStrongestEmotion_Implementation()
{
	if (CurrentEmotions.IsEmpty())
	{
		return nullptr;
	}

	UTMEmotion* StrongestEmotion = CurrentEmotions[0];
	for (int i = 1; i < CurrentEmotions.Num(); i++)
	{
		if (CurrentEmotions[i]->Intensity > StrongestEmotion->Intensity)
		{
			StrongestEmotion = CurrentEmotions[i];
		}
	}

	return StrongestEmotion;
}
