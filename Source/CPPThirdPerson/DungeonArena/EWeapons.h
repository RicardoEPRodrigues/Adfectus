// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWeapons.generated.h"

/**
* Weapons Enum
* States every usable weapons, including not weapon (Unarmed)
*/

UENUM(BlueprintType)
enum class EWeapons : uint8
{
	DA_Unarmed UMETA(DisplayName = "Unarmed"),
	DA_Axe UMETA(DisplayName = "Axe"),
	DA_Crossbow UMETA(DisplayName = "Crossbow"),
	DA_Dagger UMETA(DisplayName = "Dagger"),
	DA_Hammer UMETA(DisplayName = "Hammer"),
	DA_Spear UMETA(DisplayName = "Spear"),
	DA_Staff UMETA(DisplayName = "Staff"),
	DA_Sword UMETA(DisplayName = "Sword")
};
