// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AdfectusUtilsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UAdfectusUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Creates a Rotator that can rotate an origin direction to a target direction.
	 * @param A Origin direction
	 * @param B Target direction (where the Original will be rotated to)
	 * @return A Rotator that can rotate vector A to vector B
	 */
	UFUNCTION(BlueprintCallable, Category="Math")
	static FRotator MakeRotatorFromDirections(const FVector A, const FVector B);

	/**
	 * @brief Detects a collision between two actors. The result collision regards the second actor.
	 * @param A First actor to extract location from
	 * @param B Second actor to extract location. HitOut will have this as Actor.
	 * @param OutHit Hit Result to be populated with the collision detection.
	 * @param PreferBLocation Whether the center of the collision detection should prefer the B location. 0 prefers the A location, 1 prefers the B location.
	 * @return True if a collision is detected, false otherwise.
	 */
	UFUNCTION(Category="Collision", BlueprintPure)
	static bool DetectCollision(const class AActor* A, const class AActor* B, struct FHitResult& OutHit,
	                            float PreferBLocation = 0.0f);

	/**
	 * Get the Exogenous value found between the value and expectation.
	 * @param Value Actual value read to compare with the expected value.
	 * @param Expected Expected value.
	 * @return Exogenous value found between the value and expectation.
	 */
	UFUNCTION(Category="Math", BlueprintPure)
	static float Exogenous(const float& Value, const float& Expected);

	/**
	 * Get the Endogenous value found between the value and expectation.
	 * @param Value Actual value read to compare with the expected value.
	 * @param Expected Expected value.
	 * @param DesiredValue Value you wish to reach.
	 * @return Endogenous value found between the value and expectation given a desired value.
	 */
	UFUNCTION(Category="Math", BlueprintPure)
	static float Endogenous(const float& Value, const float& Expected, const float& DesiredValue);

	/**
	 * Perform a derivative on the provided array of values.
	 * @param Values Values used to make the derivation.
	 * @param Out Array where the derived values will be stored.
	 */
	UFUNCTION(Category="Math", BlueprintCallable)
	static void Derivative(const TArray<float>& Values, TArray<float>& Out);

	UFUNCTION(Category="Local Player", BlueprintPure)
	static ULocalPlayer* GetLocalPlayer(APlayerController* Controller);
};
