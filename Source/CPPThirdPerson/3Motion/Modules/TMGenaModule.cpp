// Fill out your copyright notice in the Description page of Project Settings.


#include "TMGenaModule.h"
#include "../TMLogging.h"
#include "../ThreeMotionAgentComponent.h"
#include "../../CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../TMTheoryOfMind.h"

void UTMGenaModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
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

	Owner->OnInterruptibleAnticipationEvent.AddDynamic(this, &UTMGenaModule::OnActorInterruptibleAnticipation);
	Owner->OnUninterruptibleAnticipationEvent.AddDynamic(this, &UTMGenaModule::OnActorUninterruptibleAnticipation);
	Owner->OnUninterruptibleFollowthroughEvent.AddDynamic(this, &UTMGenaModule::OnActorUninterruptibleFollowthrough);
	Owner->OnInterruptibleFollowthroughEvent.AddDynamic(this, &UTMGenaModule::OnActorInterruptibleFollowthrough);
	Owner->OnInterruptedEvent.AddDynamic(this, &UTMGenaModule::UTMGenaModule::OnActorInterrupted);

	// Successfully 
	// UE_LOG(Log3Motion, Log, TEXT("Successfully initialized %s on %s"), *GetNameSafe(this), *GetNameSafe(Owner))
}

void UTMGenaModule::Execute_Implementation()
{
	if (!Owner)
	{
		return;
	}

	// If character is dead it is not doing anything.
	if (!Owner->IsAlive())
	{
		if (Action)
		{
			Action = nullptr;
			OwnerAgentComponent->TheoryOfMind->DesiredAction = nullptr;
		}
		return;
	}

	// If there is an action that is not Walk and is not at an interruptible or canceled stage, then return.
	if (Action && (!Action->Name.Contains("Walk") && !Action->Name.Contains("Sprint")) &&
		(Action->Stage != ETMActionStage::Follow_Through_Interruptible && Action->Stage != ETMActionStage::Cancel))
	{
		return;
	}

	const bool IsMoving = !Owner->GetCharacterMovement()->Velocity.IsNearlyZero();
	if (IsMoving)
	{
		const bool IsSprinting = Owner->IsSprinting;
		// Get action from available actions that matches to Light or Heavy Attack.
		UTMAction** ActionPtr = OwnerAgentComponent->TheoryOfMind->AvailableActions.FindByPredicate(
			[IsSprinting](const UTMAction* AvailableAction)
			{
				return IsSprinting
					       ? AvailableAction->Name.Contains("Sprint")
					       : AvailableAction->Name.Contains("Walk");
			});

		if (!ActionPtr || !(*ActionPtr))
		{
			//UE_LOG(Log3Motion, Warning, TEXT("%s: No action available found for the walk."), *GetNameSafe(this))
			return;
		}

		// Copy it and modify its state to Interruptible Anticipation.
		// Store it to be reused.
		Action = DuplicateObject(*ActionPtr, (*ActionPtr)->GetOuter());
		Action->Stage = ETMActionStage::None;

		// Set CurrentAction to be this new action.
		OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	}
	else if (Action &&
		(Action->Name.Contains("Walk") || Action->Name.Contains("Sprint")))
	{
		OwnerAgentComponent->TheoryOfMind->DesiredAction = nullptr;
	}
}

void UTMGenaModule::EndPlay_Implementation()
{
	Action = nullptr;
}

void UTMGenaModule::OnActorInterruptibleAnticipation()
{
	if (!IsEnabled)
	{
		return;
	}


	if (!Owner)
	{
		return;
	}

	const bool IsAttacking = Owner->IsAttacking;
	const bool IsHeavyAttacking = Owner->IsHeavyAttacking;
	const bool IsBlocking = Owner->IsBlocking;

	if (!IsAttacking && !IsHeavyAttacking && !IsBlocking)
	{
		return;
	}

	// Get action from available actions that matches to Light or Heavy Attack.
	UTMAction** ActionPtr = OwnerAgentComponent->TheoryOfMind->AvailableActions.FindByPredicate(
		[IsAttacking,IsBlocking](const UTMAction* AvailableAction)
		{
			if (IsBlocking)
			{
				return AvailableAction->Name.Contains("Block");
			}
			return IsAttacking
				       ? AvailableAction->Name.Contains("Light Attack")
				       : AvailableAction->Name.Contains("Heavy Attack");
		});

	if (!ActionPtr || !(*ActionPtr))
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No action available found for the attack."), *GetNameSafe(this))
		return;
	}

	// Copy it and modify its state to Interruptible Anticipation.
	// Store it to be reused.
	Action = DuplicateObject(*ActionPtr, (*ActionPtr)->GetOuter());
	Action->Stage = ETMActionStage::Anticipation_Interruptible;

	// Set CurrentAction to be this new action.
	OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	// UE_LOG(Log3Motion, Log, TEXT("%s: Interruptible Anticipation triggered for %s. Action %s"), *GetNameSafe(this),
	//        *GetNameSafe(Owner), *Action->Name)
}

void UTMGenaModule::OnActorUninterruptibleAnticipation()
{
	if (!IsEnabled)
	{
		return;
	}


	if (!Owner || !Action)
	{
		return;
	}

	Action->Stage = ETMActionStage::Anticipation_Uninterruptible;

	// Set CurrentAction to be this new action.
	OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	// UE_LOG(Log3Motion, Log, TEXT("%s: Uninterruptible Anticipation triggered for %s"), *GetNameSafe(this),
	//        *GetNameSafe(Owner))
}

void UTMGenaModule::OnActorUninterruptibleFollowthrough()
{
	if (!IsEnabled)
	{
		return;
	}

	if (!Owner || !Action)
	{
		return;
	}

	Action->Stage = ETMActionStage::Follow_Through_Uninterruptible;

	// Set CurrentAction to be this new action.
	OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	// UE_LOG(Log3Motion, Log, TEXT("%s: Uninterruptible Follow-Through triggered for %s"), *GetNameSafe(this),
	//        *GetNameSafe(Owner))
}

void UTMGenaModule::OnActorInterruptibleFollowthrough()
{
	if (!IsEnabled)
	{
		return;
	}


	if (!Owner || !Action)
	{
		return;
	}

	Action->Stage = ETMActionStage::Follow_Through_Interruptible;

	// Set CurrentAction to be this new action.
	OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	// UE_LOG(Log3Motion, Log, TEXT("%s: Interruptible Follow-Through triggered for %s"), *GetNameSafe(this),
	//        *GetNameSafe(Owner))
}

void UTMGenaModule::OnActorInterrupted()
{
	if (!IsEnabled)
	{
		return;
	}

	if (!Owner || !Action)
	{
		return;
	}

	Action->Stage = ETMActionStage::Cancel;

	// Set CurrentAction to be this new action.
	OwnerAgentComponent->TheoryOfMind->DesiredAction = Action;
	// UE_LOG(Log3Motion, Log, TEXT("%s: Interrupted action triggered for %s"), *GetNameSafe(this), *GetNameSafe(Owner))
}
