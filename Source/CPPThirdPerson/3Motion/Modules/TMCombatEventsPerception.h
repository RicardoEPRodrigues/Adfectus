// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TMDelayPerception.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "TMCombatEventsPerception.generated.h"

UENUM(BlueprintType)
enum class ECombatEventType : uint8
{
	Hit UMETA(DisplayName="Hit"),
	Miss UMETA(DisplayName="Miss"),
	Block UMETA(DisplayName="Block"),
	Other UMETA(DisplayName="Other")
};

UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UTMCombatEventPerceptData : public UTMPerceptData
{
	GENERATED_BODY()

public:
	/**
	 * Type of combat event.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	ECombatEventType Type = ECombatEventType::Other;

	/**
	 * For whom the attack was intended. It assumes the percept owner is the attacker.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	class AActor* Target = nullptr;

	/// Character's transformations at the time of this percept
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	FTransform Transform;

	/// Character's transformations at the time of this percept
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	FTransform TargetTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	float TargetMaxHealth = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	float TargetPreviousHealth = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	float TargetHealth = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	float HealthDifference = 0.0f;

	virtual bool Equals_Implementation(const UTMPerceptData* Other) override;
};

/**
 * This module captures the perception of combat related events from the characters.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMCombatEventsPerception : public UTMDelayPerception
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<AActor*> RelevantActors;

	virtual TArray<UTMPercept*> FindPercepts_Implementation(const UWorld* World) override;

public:
	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;

	UFUNCTION(BlueprintCallable, Category="3Motion")
	void OnHitEvent(class AActor* Attacker, class AActor* Target, const float TargetMaxHealth,
	                const float TargetPreviousHealth, const float TargetHealth, const float HealthDifference);
	UFUNCTION(BlueprintCallable, Category="3Motion")
	void OnBlockEvent(class AActor* Attacker, class AActor* Target, const float TargetMaxHealth,
	                  const float TargetPreviousHealth, const float TargetHealth, const float HealthDifference);

	UFUNCTION(BlueprintCallable, Category="3Motion")
	static UTMPercept* CreateCombatEventPercept(class AActor* Attacker, class AActor* Target,
	                                            const ECombatEventType Type, const float TargetMaxHealth,
	                                            const float TargetPreviousHealth, const float TargetHealth,
	                                            const float HealthDifference, UObject* Outer);

	/**
	 * Percepts stored to be deployed when FindPercepts is called.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="3Motion")
	TArray<class UTMPercept*> StashPercepts;
};
