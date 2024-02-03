// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EGameStage.generated.h"

/**
* Game Stage Enum
* States the stage in which a match is on.
*/

UENUM(BlueprintType, Category="Best Out Of")
enum class EGameStage : uint8
{
	New_Match UMETA(DisplayName = "New Match Stage"),
	Pre_Match UMETA(DisplayName = "Pre Match Stage"),
	Match_Start UMETA(DisplayName = "Match Start Stage"),
	Match_End UMETA(DisplayName = "Match End Stage"),
	Match_Other UMETA(DisplayName = "Other Match Stage (unknown behavior usage)")
};
