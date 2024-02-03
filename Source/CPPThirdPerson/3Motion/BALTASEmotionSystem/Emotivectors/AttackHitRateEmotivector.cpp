// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "AttackHitRateEmotivector.h"

#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMLogging.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"
#include "CPPThirdPerson/3Motion/BALTASEmotionSystem/TMBaltasModule.h"
#include "CPPThirdPerson/3Motion/Modules/TMCharacterPerception.h"
#include "CPPThirdPerson/3Motion/Modules/TMCombatEventsPerception.h"

UAttackHitRateEmotivector::UAttackHitRateEmotivector() : UEmotivector()
{
	ClampValues = true;
	ClampInterval = FFloatInterval(0, 1);
}

void UAttackHitRateEmotivector::OnUpdate_Implementation()
{
	if (!OwnerAgentComponent->TheoryOfMind->HasNewPercepts()) return;

	const auto PerceptHistoryByActor = OwnerAgentComponent->TheoryOfMind->PerceptHistoryPerActor;

	// const auto PerceptHistory = OwnerAgentComponent->TheoryOfMind->PerceptHistory;
	const auto PerceptHistoryOwner = PerceptHistoryByActor.Contains(Owner)
		                                 ? PerceptHistoryByActor[Owner].Percepts
		                                 : TArray<TWeakObjectPtr<UTMPercept>>();
	if (PerceptHistoryOwner.IsEmpty())
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: PerceptHistoryOwner Is Empty"), *GetNameSafe(this))
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
	for (int i = PerceptHistoryOwner.Num() - 1; i >= 0; --i)
	{
		const UTMPercept* Percept = PerceptHistoryOwner[i].Get();
		if (!Percept)
		{
			continue;
		}

		if (Percept == LatestPercept)
		{
			break;
		}

		if (const UTMCombatEventPerceptData* Data = Cast<UTMCombatEventPerceptData>(Percept->Data))
		{
			// UE_LOG(Log3Motion, Warning, TEXT("%s: New COMBAT Percept: %s %s %s. Time %f"), *GetNameSafe(this),
			//        *GetNameSafe(Percept->Actor),
			//        *UEnum::GetValueAsString(Data->Type), *GetNameSafe(Data->Target), Percept->Time);

			switch (Data->Type)
			{
			case ECombatEventType::Hit:
				AddValue(ValueOnHit);
				UE_LOG(Log3Motion, Log, TEXT("%s: Attack HIT %s!"), *GetNameSafe(this->Owner), *GetNameSafe(Data->Target));
				break;
			case ECombatEventType::Block:
				AddValue(ValueOnBlock);
				UE_LOG(Log3Motion, Log, TEXT("%s: Attack BLOCK %s!"), *GetNameSafe(this->Owner), *GetNameSafe(Data->Target));
				break;
			case ECombatEventType::Miss:
				// Miss is not implemented
				// UE_LOG(Log3Motion, Warning, TEXT("%s: Attack Missed!"), *GetNameSafe(this));
				AddValue(ValueOnMiss);
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

			if (Action->Name.Contains("Attack", ESearchCase::IgnoreCase) && Action->Stage ==
				ETMActionStage::Follow_Through_Uninterruptible)
			{
				AttackEndPercept = Percept;
				AttackEndPerceptData = CharacterPerceptData;
				AttackEndPerceptIndex = i;
			}
		}
	}

	// If a Follow_Through_Uninterruptible is found, then see if there was a hit/block.
	if (AttackEndPercept)
	{
		for (int i = AttackEndPerceptIndex; i >= 0; --i)
		{
			const UTMPercept* Percept = PerceptHistoryOwner[i].Get();
			if (!Percept)
			{
				continue;
			}

			if (const UTMCombatEventPerceptData* Data = Cast<UTMCombatEventPerceptData>(Percept->Data))
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
				UE_LOG(Log3Motion, Log, TEXT("%s: Attack Missed!"), *GetNameSafe(this->Owner));
				AddValue(ValueOnMiss);
				break;
			}
		}
	}

	if (UTMPercept* LastPerceptViewed = PerceptHistoryOwner.Last().Get())
	{
		LatestPercept = LastPerceptViewed;
	}
}
