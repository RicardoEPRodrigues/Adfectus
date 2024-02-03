// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/BALTASEmotionSystem/Emotivector.h"
#include "UObject/Object.h"
#include "AttackHitRateOpponentEmotivector.generated.h"

/**
 * Emotivector that collects the success rate of a opponents attacks.
 * 0 if an attack never misses, 0.5 if all attacks miss, and 1 if all attacks are blocked.
 *
 * It also generates a sensation on the beginning of an attack if the predictions are too low or too high.
 * 
 * Depends on @class UTMCharacterPerception [TMCharacterPerception.h] and @class UTMCombatEventPerceptData [TMCombatEventPerceptData.h]
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UAttackHitRateOpponentEmotivector : public UEmotivector
{
	GENERATED_BODY()

	UPROPERTY()
	class UTMPercept* LatestPercept;

public:
	UAttackHitRateOpponentEmotivector();
	
	virtual void OnUpdate_Implementation() override;

	/**
	 * Value to add when an attack hit is perceived.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="BALTAS")
	float ValueOnHit = 0.0f;
	/**
	 * Value to add when an attack miss is perceived.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="BALTAS")
	float ValueOnMiss = 0.5f;
	/**
	 * Value to add when an attack block is perceived.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="BALTAS")
	float ValueOnBlock = 1.0f;
};
