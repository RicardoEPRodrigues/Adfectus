// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "TMCombatEventsPerception.h"

#include "TMCharacterPerception.h"
#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMotionUtils.h"
#include "Kismet/GameplayStatics.h"

bool UTMCombatEventPerceptData::Equals_Implementation(const UTMPerceptData* Other)
{
	// if (const UTMCombatEventPerceptData* PerceptData = Cast<UTMCombatEventPerceptData>(Other))
	// {
	// 	// Either the pointers are equal or their values are equal.
	// 	const bool IsTypeEqual = this->Type == PerceptData->Type;
	//
	// 	const bool IsTargetEqual = this->Target == PerceptData->Target; 
	//
	// 	// The high tolerance allows us to not constantly fill the percepts with location data.
	// 	//const bool IsTransformEqual = this->Transform.Equals(PerceptData->Transform, 50);
	//
	// 	return IsTypeEqual && IsTargetEqual;
	// }

	// Always return false. This is because each event is already unique, so comparing with older events is unnecessary.
	return false;
}

void UTMCombatEventsPerception::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
{
	Super::Initialize_Implementation(OwnerComponent);

	const UWorld* World = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(World, ACPPThirdPersonCharacter::StaticClass(), RelevantActors);

	for (const auto Actor : RelevantActors)
	{
		ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Actor);
		if (!Character)
		{
			continue;
		}

		Character->CombatEvent_OnHit.AddDynamic(this, &UTMCombatEventsPerception::OnHitEvent);
		Character->CombatEvent_OnBlock.AddDynamic(this, &UTMCombatEventsPerception::OnBlockEvent);
	}
}

void UTMCombatEventsPerception::OnHitEvent(AActor* Attacker, AActor* Target, const float TargetMaxHealth,
                                           const float TargetPreviousHealth, const float TargetHealth,
                                           const float HealthDifference)
{
	if (UTMPercept* Percept = CreateCombatEventPercept(Attacker, Target, ECombatEventType::Hit, TargetMaxHealth,
	                                                   TargetPreviousHealth, TargetHealth,
	                                                   HealthDifference, OwnerAgentComponent))
	{
		StashPercepts.Add(Percept);
	}
}

void UTMCombatEventsPerception::OnBlockEvent(AActor* Attacker, AActor* Target, const float TargetMaxHealth,
                                             const float TargetPreviousHealth, const float TargetHealth,
                                             const float HealthDifference)
{
	if (UTMPercept* Percept = CreateCombatEventPercept(Attacker, Target, ECombatEventType::Block, TargetMaxHealth,
	                                                   TargetPreviousHealth, TargetHealth,
	                                                   HealthDifference, OwnerAgentComponent))
	{
		StashPercepts.Add(Percept);
	}
}

TArray<UTMPercept*> UTMCombatEventsPerception::FindPercepts_Implementation(const UWorld* World)
{
	// Update time of percepts to this moment.
	for (const auto Percept : StashPercepts)
	{
		Percept->Time = World->GetTimeSeconds();
	}
	TArray<UTMPercept*> ActorPercepts = StashPercepts;
	StashPercepts.Empty();
	return ActorPercepts;
}

UTMPercept* UTMCombatEventsPerception::CreateCombatEventPercept(AActor* Attacker, AActor* Target,
                                                                const ECombatEventType Type,
                                                                const float TargetMaxHealth,
                                                                const float TargetPreviousHealth,
                                                                const float TargetHealth, const float HealthDifference,
                                                                UObject* Outer)
{
	if (!Attacker)
	{
		return nullptr;
	}
	UTMCombatEventPerceptData* PerceptData = NewObject<UTMCombatEventPerceptData>(Outer);

	PerceptData->Type = Type;
	PerceptData->Transform = Attacker->GetTransform();

	if (Target)
	{
		PerceptData->Target = Target;
		PerceptData->TargetTransform = Target->GetTransform();
	}

	PerceptData->TargetMaxHealth = TargetMaxHealth;
	PerceptData->TargetPreviousHealth = TargetPreviousHealth;
	PerceptData->TargetHealth = TargetHealth;
	PerceptData->HealthDifference = HealthDifference;

	UTMPercept* Percept = UTMotionUtils::CreatePercept(
		Attacker,
		Outer->GetWorld()->GetTimeSeconds(),
		Outer,
		PerceptData
	);

	return Percept;
}
