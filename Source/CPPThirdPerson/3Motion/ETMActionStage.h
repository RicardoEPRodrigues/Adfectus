// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETMActionStage.generated.h"

/**
* 3Motion Action Stages Enum
* States the different possible stages of an action when divided.
*/

UENUM(BlueprintType)
enum class ETMActionStage : uint8
{
	None UMETA(DisplayName = "None"),
	Anticipation_Interruptible UMETA(DisplayName = "Anticipation Interruptible"),
	Anticipation_Uninterruptible UMETA(DisplayName = "Anticipation Uninterruptible"),
	Follow_Through_Interruptible UMETA(DisplayName = "Follow Through Interruptible"),
	Follow_Through_Uninterruptible UMETA(DisplayName = "Follow Through Uninterruptible"),
	Cancel UMETA(DisplayName = "Cancel"),
};
