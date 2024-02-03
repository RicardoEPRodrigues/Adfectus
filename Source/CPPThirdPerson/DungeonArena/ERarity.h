// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ERarity.generated.h"

/**
* Rarity Enum
* States rarities that can be used in multiple objects.
*/

UENUM(BlueprintType)
enum class ERarity : uint8
{
	DA_Common UMETA(DisplayName = "Common"),
	DA_Uncommon UMETA(DisplayName = "Uncommon"),
	DA_Rare UMETA(DisplayName = "Rare"),
	DA_Epic UMETA(DisplayName = "Epic"),
	DA_Legendary UMETA(DisplayName = "Legendary")
};
