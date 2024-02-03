// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "TMDirectMapPerformModule.h"
#include "../TMLogging.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "../../CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"

void UTMDirectMapPerformModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
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

	// Successfully 
	// UE_LOG(Log3Motion, Log, TEXT("Successfully initialized %s on %s"), *GetNameSafe(this), *GetNameSafe(Owner))
}

void UTMDirectMapPerformModule::Execute_Implementation()
{
	// Directly map Desired Action/Emotion to Current Action/Emotion
	const auto TheoryOfMind = OwnerAgentComponent->TheoryOfMind;
	TheoryOfMind->CurrentAction = TheoryOfMind->DesiredAction;
	TheoryOfMind->CurrentEmotions = TheoryOfMind->DesiredEmotions;
}

void UTMDirectMapPerformModule::EndPlay_Implementation()
{
}
