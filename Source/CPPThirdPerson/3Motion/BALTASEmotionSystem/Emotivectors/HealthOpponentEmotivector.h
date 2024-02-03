// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/BALTASEmotionSystem/Emotivector.h"
#include "UObject/Object.h"
#include "HealthOpponentEmotivector.generated.h"

/**
 * Emotivector that collects the success rate of a opponents attacks.
 * 0 if an attack never misses, 0.5 if all attacks miss, and 1 if all attacks are blocked.
 *
 * It also generates a sensation on the beginning of an attack if the predictions are too low or too high.
 * 
 * Depends on @class UTMCharacterPerception [TMCharacterPerception.h] and @class UTMCombatEventPerceptData [TMCombatEventPerceptData.h]
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UHealthOpponentEmotivector : public UEmotivector
{
	GENERATED_BODY()

	UPROPERTY()
	class UTMPercept* LatestPercept;

public:
	UHealthOpponentEmotivector();

	virtual void Initialize_Implementation(UTMBaltasModule* Module) override;

	virtual void Reset_Implementation() override;

	virtual void OnUpdate_Implementation() override;

	/**
	 * The duration of the sensation generated when the character believes that it will be able to hit the opponent. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float HitLifetime = 1.0f;
	/**
	 * The importance of the sensation generated when the character believes that it will be able to hit the opponent. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float HitImportance = 1.0f;

	/**
	 * The duration of the sensation generated when the character believes that it will not be able to hit the opponent (miss or block). 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float NotHitLifetime = 1.0f;
	/**
	 * The importance of the sensation generated when the character believes that it will not be able to hit the opponent (miss or block). 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float NotHitImportance = 1.0f;

	/**
	 * The duration of the sensation generated when the character believes the next attack will kill the opponent. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float FinalBlowLifetime = 1.0f;
	/**
	 * The importance of the sensation generated when the character believes the next attack will kill the opponent. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float FinalBlowImportance = 1.0f;

	/**
	 * The duration of the sensation generated when the opponent is feeling a negative emotion. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float OpponentEmotionReactionLifetime = 1.0f;
	/**
	 * The importance of the sensation generated when the opponent is feeling a negative emotion. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float OpponentEmotionReactionImportance = 1.0f;
};
