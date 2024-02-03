// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "HealthEmotivector.h"

#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMLogging.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"
#include "CPPThirdPerson/3Motion/BALTASEmotionSystem/TMBaltasModule.h"
#include "CPPThirdPerson/3Motion/Modules/TMCharacterPerception.h"
#include "CPPThirdPerson/3Motion/Modules/TMCombatEventsPerception.h"
#include "CPPThirdPerson/Utils/AdfectusUtilsLibrary.h"

UHealthEmotivector::UHealthEmotivector() : UEmotivector()
{
	ClampValues = true;
	ClampInterval = FFloatInterval(0, 1);
}

void UHealthEmotivector::Initialize_Implementation(UTMBaltasModule* Module)
{
	Super::Initialize_Implementation(Module);

	AddValue(1);
}

void UHealthEmotivector::Reset_Implementation()
{
	Super::Reset_Implementation();
	AddValue(1);
}

void UHealthEmotivector::OnUpdate_Implementation()
{
	if (!OwnerAgentComponent->TheoryOfMind->HasNewPercepts()) return;

	const auto PerceptHistoryByActor = OwnerAgentComponent->TheoryOfMind->PerceptHistoryPerActor;

	auto PerceptHistoryOpponent = TArray<TWeakObjectPtr<UTMPercept>>();
	for (const TTuple<AActor*, FPerceptContainer>& HistoryByActor : PerceptHistoryByActor)
	{
		if (HistoryByActor.Key == Owner)
		{
			continue;
		}
		PerceptHistoryOpponent = HistoryByActor.Value.Percepts;
		break;
	}
	if (PerceptHistoryOpponent.IsEmpty())
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: PerceptHistoryOpponent Is Empty"), *GetNameSafe(this))
		return;
	}


	/*
	 * An attack misses if it reaches the ETMActionStage::Follow_Through_Uninterruptible and no combat event is triggered.
	 * An attack hits a hit combat event happens.
	 * An attack is blocked if a block combat event happens.
	 *
	 * The logic is implemented as follows:
	 *		check for combat events
	 *			add hit/block value
	 *		check for an attack in ETMActionStage::Follow_Through_Uninterruptible
	 *		if one is found
	 *			loop back until you find a combat event or the beginning of that action (the attack in ETMActionStage::Anticipation_Uninterruptible)
	 *			if combat event
	 *				then a type of hit is found and we can exit.
	 *			if beginning of action
	 *				no combat event means a miss!
	 *				add miss value
	 */

	const UTMPercept* AttackEndPercept = nullptr;
	const UTMCharacterPerceptData* AttackEndPerceptData = nullptr;
	int AttackEndPerceptIndex = -1;
	for (int i = PerceptHistoryOpponent.Num() - 1; i >= 0; --i)
	{
		const UTMPercept* Percept = PerceptHistoryOpponent[i].Get();
		if (!Percept)
		{
			continue;
		}

		if (Percept == LatestPercept)
		{
			break;
		}

		if (const UTMCombatEventPerceptData* Data = Cast<UTMCombatEventPerceptData>(Percept->Data);
			Data && Data->Target == Owner)
		{
			// UE_LOG(Log3Motion, Warning, TEXT("%s: New COMBAT Percept: %s %s %s. Time %f"), *GetNameSafe(this),
			//        *GetNameSafe(Percept->Actor),
			//        *UEnum::GetValueAsString(Data->Type), *GetNameSafe(Data->Target), Percept->Time);

			switch (Data->Type)
			{
			case ECombatEventType::Hit:
				AddValue(Data->TargetHealth / Data->TargetMaxHealth);
			// UE_LOG(Log3Motion, Log, TEXT("%s: Opponent Attack HIT!"), *GetNameSafe(this));
				break;
			case ECombatEventType::Block:
				AddValue(Data->TargetHealth / Data->TargetMaxHealth);
			// UE_LOG(Log3Motion, Log, TEXT("%s: Opponent Attack BLOCK!"), *GetNameSafe(this));
				break;
			case ECombatEventType::Miss:
				// Miss is not implemented
				// UE_LOG(Log3Motion, Warning, TEXT("%s: Attack Missed!"), *GetNameSafe(this));
				AddValue(Data->TargetHealth / Data->TargetMaxHealth);
			default:
				break;
			}
		}
		else if (const UTMCharacterPerceptData* CharacterPerceptData = Cast<UTMCharacterPerceptData>(Percept->Data);
			!AttackEndPercept && CharacterPerceptData)
		{
			const UTMAction* Action = CharacterPerceptData->Action;
			if (!Action)
			{
				continue;
			}

			// UE_LOG(Log3Motion, Log, TEXT("%s: New CHARACTER Percept: %s - %s. Time %f"), *GetNameSafe(this),
			//        *Action->Name,
			//        *UEnum::GetValueAsString(Action->Stage), Percept->Time);

			if (Action->Name.Contains("Attack", ESearchCase::IgnoreCase))
			{
				if (Action->Stage == ETMActionStage::Follow_Through_Uninterruptible)
				{
					AttackEndPercept = Percept;
					AttackEndPerceptData = CharacterPerceptData;
					AttackEndPerceptIndex = i;
				}
				// React to a beginning of an attack.
				else if (Action->Stage == ETMActionStage::Anticipation_Interruptible && !Predictions.IsEmpty() && !
					MetaPredictions.IsEmpty())
				{
					const float Value = Values.Last();
					const float Prediction = Predictions.Last();
					const float PredictionError = MetaPredictions.Last();

					// If the expected behavior is believed to happen, then react.
					// The believe is true if the probability far enough from the value. Enough is defined by the prediction error.
					if (FMath::Abs(Value - Prediction) > PredictionError)
					{
						// The character believes that the opponent will not be able to hit it (miss or block).
						if (Prediction > Value)
						{
							USensation* Sensation = MakeSensation(ESensationValence::Reward,
																  ESensationType::BetterThanExpected,
																  UAdfectusUtilsLibrary::Exogenous(Value, Prediction),
																  NotHitLifetime, NotHitImportance);
							SalientSensations.Add(Sensation);
						}
						// The character believes that the opponent will be able to hit it.
						else
						{
							USensation* Sensation = MakeSensation(ESensationValence::Punishment,
																  ESensationType::WorseThanExpected,
																  UAdfectusUtilsLibrary::Exogenous(Value, Prediction),
																  HitLifetime, HitImportance);
							SalientSensations.Add(Sensation);
						}
					}

					// The character believe the next attack will kill it!
					if (Prediction - PredictionError < 0.0f)
					{
						USensation* Sensation = MakeSensation(ESensationValence::Punishment,
						                                      ESensationType::WorseThanExpected,
						                                      UAdfectusUtilsLibrary::Exogenous(Value, Prediction),
						                                      FinalBlowLifetime, FinalBlowImportance);
						SalientSensations.Add(Sensation);
					}
					
					// if opponent's emotion is good, then we feel bad.
					const UTMEmotion* OpponentEmotion = nullptr;
					if (!CharacterPerceptData->Emotions.IsEmpty())
					{
						OpponentEmotion = CharacterPerceptData->Emotions[0];
					}
					if (OpponentEmotion)
					{
						// Positive affect emotions
						if (OpponentEmotion->Name.Equals("Confidence"))
						{
							USensation* Sensation = MakeSensation(ESensationValence::Punishment,
																  ESensationType::WorseThanExpected,
																  UAdfectusUtilsLibrary::Exogenous(Value, Prediction),
																  OpponentEmotionReactionLifetime, OpponentEmotionReactionImportance);
							SalientSensations.Add(Sensation);
						}
					}
				}
			}
		}
	}

	// If a Follow_Through_Uninterruptible is found, then see if there was a hit/block.
	if (AttackEndPercept)
	{
		for (int i = AttackEndPerceptIndex; i >= 0; --i)
		{
			const UTMPercept* Percept = PerceptHistoryOpponent[i].Get();
			if (!Percept)
			{
				continue;
			}

			if (const UTMCombatEventPerceptData* Data = Cast<UTMCombatEventPerceptData>(Percept->Data);
				Data && Data->Target == Owner)
			{
				// Found a hit/block
				break;
			}

			const UTMCharacterPerceptData* Data = Cast<UTMCharacterPerceptData>(Percept->Data);
			if (!Data) continue;

			const UTMAction* Action = Data->Action;
			if (!Action)
			{
				continue;
			}

			if (Action->Name.Equals(AttackEndPerceptData->Action->Name) && Action->Stage ==
				ETMActionStage::Anticipation_Interruptible)
			{
				// Found the beginning of the attack.
				// It means the attack missed.
				// UE_LOG(Log3Motion, Log, TEXT("%s: Opponent Attack Missed!"), *GetNameSafe(this));
				AddValue(Values.Last());
				break;
			}
		}
	}

	if (UTMPercept* LastPerceptViewed = PerceptHistoryOpponent.Last().Get())
	{
		LatestPercept = LastPerceptViewed;
	}
}
